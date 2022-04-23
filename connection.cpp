#include "./include/connection.hpp"

connection::connection(config &cf, fd_set &rfds) : server_size(0), fd_arr(), read_fds(rfds)
{
	int serv_sock;
	int reuse = 1;
	struct sockaddr_in	serv_adr;
	//지금은 하나의 서버만 생성한다고 가정해서 작성하지만 나중엔 Config를 받아서 서버를 모두 생성하도록 수정
	for (int i = 0; i < (int)cf.v_s.size(); i++)
	{
		std::cout << "server " << i << std::endl;
		//1. 서버 소켓 하나 생성
		if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			throw (connection::socket_error());
		server_size++;
		fd_arr.push_back(fd_info(serv_sock, &cf.v_s[i]));
		//2. socket에 IP, Port 번호 할당
		memset(&serv_adr, 0, sizeof(serv_adr));
		serv_adr.sin_family = AF_INET;
		std::cout << "IP: " << cf.v_s[i].v_listen[1] << ", Port: " << cf.v_s[i].v_listen[0] << std::endl;
		serv_adr.sin_addr.s_addr = inet_addr(cf.v_s[i].v_listen[1].c_str());
		int port = convert_to_num(cf.v_s[i].v_listen[0], 10);
		serv_adr.sin_port = htons(port); //일단은 포트 번호 100로 설정
		if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) //바로 주소 재사용가능하도록 설정
			throw (connection::setsockopt_error());
		if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == -1) //바로 포트 재사용가능하도록 설정
			throw (connection::setsockopt_error());
		if (bind(serv_sock, reinterpret_cast<struct sockaddr*>(&serv_adr), sizeof(serv_adr)) == -1)
			throw (connection::bind_error());
		//3. server socket에 클라이언트 접속 요청 대기할 수 있도록 - 일단 10개의 수신 대기열
		if (listen(serv_sock, 128) == -1)
			throw (connection::listen_error());
		FD_SET(serv_sock, &read_fds); //read_fds에서 서버 fd 활성화
		//fcntl(serv_sock, F_SETFL, O_NONBLOCK);
		//소켓 프로그래밍에서 4.클라이언트 접속 요청 수락하는 부분, 5.클라이언트와 연결된 소켓으로 데이터 송수신하는 부분은 다른 메서드에서
	}
}

connection::~connection(void)
{
	//std::cout << "connection destructor\n";
	for (int i = 0; i < (int)fd_arr.size(); i++)
		close(fd_arr[i].fd);
	fd_arr.clear();
}

connection::iterator	connection::fd_arr_begin(void)
{
	return (fd_arr.begin());
}

connection::iterator	connection::fd_arr_end(void)
{
	return (fd_arr.end());
}

bool					connection::is_server_socket(int fd)
{
	//서버 소켓에 입력이 들어오는 건 클라리언트 접속 요청이기 때문에 현재 fd가 서버 소켓인지 체크해주는 메서드
	for (int i = 0; i < server_size; i++)
	{
		if (fd_arr[i].fd == fd)
			return (true);
	}
	return (false);
}

int						connection::max_fd(void)
{
	int	max = 0;
	for (int i = 0; i < (int)fd_arr.size(); i++)
	{
		if (max < fd_arr[i].fd)
			max = fd_arr[i].fd;
	}
	return (max);
}

fd_info					&connection::info_of_fd(int fd)
{
	int	i;
	for (i = 0; i < (int)fd_arr.size(); i++)
	{
		if (fd_arr[i].fd == fd)
			break ;
	}
	return (fd_arr[i]);
}

void					connection::connect_client(int serv_sock)
{
	int					clnt_sock;
	struct sockaddr_in	clnt_adr;
	socklen_t			adr_sz = sizeof(clnt_adr);

	//4.클라이언트 접속 요청 수락하는 부분
	if ((clnt_sock = accept(serv_sock, reinterpret_cast<struct sockaddr*>(&clnt_adr), &adr_sz)) == -1)
		throw (connection::accept_error());
	std::cout << "connecting: serv - " << serv_sock << ", clnt - " << clnt_sock << "\n";
	std::cout << "clnt info: IP - " << inet_ntoa(clnt_adr.sin_addr) << ", Port - " << ntohs(clnt_adr.sin_port) << std::endl;
	FD_SET(clnt_sock, &read_fds);
	fcntl(clnt_sock, F_SETFL, O_NONBLOCK); //각 클라이언트 fd를 논블로킹으로 설정
	fd_arr.push_back(fd_info(clnt_sock, info_of_fd(serv_sock).server_info));
}

void					connection::disconnect_client(int clnt_sock)
{
	std::vector<fd_info>::iterator it;

	if (is_server_socket(clnt_sock))
		return ;
	std::cout << "remain fd: ";
	for (it = fd_arr.begin(); it != fd_arr.end(); ++it)
		std::cout << it->fd << " ";
	std::cout << std::endl;
	std::cout << "disconnecting: clnt - " << clnt_sock << std::endl << std::endl;
	for (it = fd_arr.begin() + server_size; it != fd_arr.end(); ++it)
	{
		if (it->fd == clnt_sock)
		{
			fd_arr.erase(it);
			return ;
		}
	}
}

void					connection::get_client_msg(int clnt_sock, request &rq)
{
	int		str_len;
	char	buf[BUF_SIZE + 1];

	if ((str_len = recv(clnt_sock, buf, BUF_SIZE, 0)) < 0)
		throw (connection::recv_error());
	buf[str_len] = '\0';
	fd_info &clnt_info = info_of_fd(clnt_sock);
	concatenate_client_msg(clnt_info, buf); //해당 클라이언트에 문자열 이어서 저장
	if (is_input_completed(clnt_info, rq)) //입력값을 다 받았는지 체크해 입력이 끝났다고 판단되면 read_fds에서 fd 삭제(이때 내부에서 파싱과 같은 처리도 같이..)
		FD_CLR(clnt_sock, &read_fds);
}

void					connection::concatenate_client_msg(fd_info &clnt_info, std::string to_append)
{
	//요청 메세지의 각 라인이 '\r\n'으로 끝난다고 함. 하지만 견고한 애플리케이션이라면 '\n'만 있는 경우도? 처리할 수 있어야 한다는 걸 보고 '\r'은 저장하기 전 삭제
	to_append.erase(remove(to_append.begin(), to_append.end(), '\r'), to_append.end());
	clnt_info.msg.append(to_append);
}

bool					connection::is_input_completed(fd_info &clnt_info, request &rq)
{
	request::iterator	it;
	//clnt_info에 status 추가 - 디폴트 값 : RQ_LINE_NOT_PARSED, rq라인 파싱 이후 HEADER_NOT_PARSED, 헤더를 파싱한 이후에 본문 처리 여부에 따라 NO_BODY, TRANSFER_ENCODING, CONTENT_LENGTH 중 하나로 저장됨.
	if (clnt_info.status == RQ_LINE_NOT_PARSED && clnt_info.msg.find("\n") != std::string::npos)
	{
		it = parse_rq_line(clnt_info, rq);
		if (rq.is_invalid(it))
			return (completed_input(rq, it, "", -1));
	}
	if (clnt_info.status == HEADER_NOT_PARSED && clnt_info.msg.find("\n\n") != std::string::npos) //헤더 부분이 완전히 들어와 파싱 처리를 해줄 상태
	{
		it = parse_client_header_by_line(clnt_info, rq); //헤더 내용까지만 파싱 (\n\n 오기 전까지만)
		is_body_exist(clnt_info, rq, it); //파싱된 헤더 내용을 기준으로 status값을 업데이트
		if (clnt_info.status == NO_BODY)
			return (completed_input(rq, it, "", -1)); //추가할 본문이 없다면 빈 문자열만 삽입하고 종료를 의미
		if (clnt_info.msg.empty())
			return (false);
	}
	if (clnt_info.status == TRANSFER_ENCODING) //플래그 값이 transfer-encoding일 때 다른 함수를 호출해서 입력 종료 여부 반환
		return (is_transfer_encoding_completed(clnt_info, rq));
	else if (clnt_info.status == CONTENT_LENGTH)
		return (is_content_length_completed(clnt_info, rq));
	return (false);
}

bool					connection::completed_input(request &rq, request::iterator &it, std::string body, int err_no)
{
	if (err_no != -1)
		rq.set_error(it, err_no);
	rq.insert(it, body);
	return (true);
}

request::iterator		connection::parse_rq_line(fd_info &clnt_info, request &rq)
{
	size_t		next = clnt_info.msg.find("\n");
	std::string	res = clnt_info.msg.substr(0, next);
	clnt_info.msg = clnt_info.msg.substr(next);
	request::iterator	it = rq.insert_rq_line(clnt_info.fd, clnt_info.server_info, res);
	clnt_info.status = HEADER_NOT_PARSED;
	return (it);
}

request::iterator		connection::parse_client_header_by_line(fd_info &clnt_info, request &rq)
{
	size_t	last = 0;
	size_t	next = 0;

	while (clnt_info.msg[last] == '\n')
		last++;
	while ((next = clnt_info.msg.find("\n", last)) != std::string::npos)
	{
		clnt_info.split_msg.push_back(clnt_info.msg.substr(last, next - last));
		last = next + 1;
		if (clnt_info.msg[last] == '\n') //헤더와 본문 사이에 개행을 하나 더 둠. -> 개행 두 개 연속으로 오면 헤더 파싱은 끝난 상태
		{
			last++;
			break ;
		}
	}
	clnt_info.msg = clnt_info.msg.substr(last); //\n\n 앞의 내용들은 파싱해서 저장해줬으니 그 뒷내용(body)들만 msg에 다시 저장
	request::iterator it = rq.find_clnt_in_tmp(clnt_info.fd);
	rq.insert_header(it, clnt_info.split_msg);
	return (it);
}

void					connection::is_body_exist(fd_info &clnt_info, request &rq, request::iterator &it)
{
	if (rq.is_invalid(it) || !rq.which_method(it, "POST")) //telnet 이용 시 본문이 필요하지 않는 메서드는 아예 입력 종료 -> POST가 아닐 때는 더 이상 입력x
	{
		clnt_info.status = NO_BODY;
		return ;
	}
	bool	te_flag = rq.is_existing_header(it, "Transfer-Encoding");
	bool	cl_flag = rq.is_existing_header(it, "Content-Length");
	if (te_flag)
		clnt_info.status = TRANSFER_ENCODING;
	else if (cl_flag)
		clnt_info.status = CONTENT_LENGTH;
	else
		clnt_info.status = NO_BODY;
}

bool					connection::is_transfer_encoding_completed(fd_info &clnt_info, request &rq)
{
	request::iterator it = rq.find_clnt_in_tmp(clnt_info.fd);
	size_t		last, next, tmp;
	int			length;

	if (!rq.corresponding_header_value(it, "Transfer-Encoding").compare("chunked"))
	{
		while ((next = clnt_info.msg.find("\n")) != std::string::npos)
		{
			try
			{
				length = convert_to_num(clnt_info.msg.substr(0, next), 16); //내부에서 변환이 잘못 됐거나 음수면 에러 throw
				last = next + 1;
				tmp = last;
				int	clnt_length = 0;
				while (true)
				{
					if ((next = clnt_info.msg.find("\n", tmp)) == std::string::npos)
						return (false);
					std::string	tmp_str = clnt_info.msg.substr(last, next - last);
					clnt_length = body_length(tmp_str);
					if (!length)
						return (completed_input(rq, it, clnt_info.tmp, -1));
					if (length == clnt_length)
					{
						if (!clnt_info.tmp.empty())
							clnt_info.tmp.append("\n");
						clnt_info.tmp.append(tmp_str);
						clnt_info.msg = clnt_info.msg.substr(next + 1);
						break ;
					}
					else if (length < clnt_length)
						throw (request::incorrect_body_length_error());
					tmp = next + 1;
				}
			}
			catch(const std::exception& e)
			{
				return (completed_input(rq, it, "", 400));
			}
		}
	}
	else
		return (completed_input(rq, it, "", 501));
	return (false);
}

bool					connection::is_content_length_completed(fd_info &clnt_info, request &rq)
{
	request::iterator	it = rq.find_clnt_in_tmp(clnt_info.fd);
	int					length;

	try
	{
		length = convert_to_num(rq.corresponding_header_value(it, "Content-Length"), 10);
		if (clnt_info.msg[clnt_info.msg.length() - 1] != '\n')
			return (false);
		int		clnt_length = body_length(clnt_info.msg.substr(0, clnt_info.msg.length() - 1)); //마지막 \n은 제외하고 길이 계산
		if (clnt_length < length)
			return (false);
		else if (clnt_length > length)
			throw (request::incorrect_body_length_error());
	}
	catch (const std::exception& e)
	{
		return (completed_input(rq, it, "", 400));
	}
	return (completed_input(rq, it, clnt_info.msg.substr(0, clnt_info.msg.length() - 1), -1));
}

int						connection::convert_to_num(std::string str_length, int radix)
{
	if (str_length.empty())
		throw (request::incorrect_body_length_error());
	char	*end;
	int		length = static_cast<int>(strtol(str_length.c_str(), &end, radix));
	if (*end || length < 0)
		throw (request::incorrect_body_length_error());
	return (length);
}

int						connection::body_length(std::string msg)
{
	size_t	newline_num = 0;
	size_t	last = 0;
	size_t	next = 0;
	while ((next = msg.find("\n", last)) != std::string::npos)
	{
		newline_num++;
		last = next + 1;
	}
	return (msg.length() + newline_num); //msg의 실제 길이 + \r 개수
}

void					connection::print_client_msg(int clnt_sock)
{
	std::vector<std::string>	msg = info_of_fd(clnt_sock).split_msg;
	std::vector<std::string>::iterator it;

	std::cout << "[print in client_msg - connection class]" << std::endl;
	for (it = msg.begin(); it != msg.end(); ++it)
		std::cout << "one line" << std::endl << *it << std::endl;
	std::cout << "<end>" << std::endl << std::endl;
}
