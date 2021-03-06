#include "../include/connection.hpp"

connection::connection(config &cf) : server_size(0), fd_max(0), fd_arr(), fdset()
{
	int serv_sock;
	int reuse = 1;
	struct sockaddr_in	serv_adr;

	for (int i = 0; i < (int)cf.v_s.size(); i++)
	{
		//1. 서버 소켓 하나 생성
		if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			throw (socket_error());
		server_size++;
		fd_arr.push_back(fd_info(serv_sock, &cf.v_s[i]));
		//2. socket에 IP, Port 번호 할당
		memset(&serv_adr, 0, sizeof(serv_adr));
		serv_adr.sin_family = AF_INET;
		std::cout << "IP: " << cf.v_s[i].v_listen[1] << ", Port: " << cf.v_s[i].v_listen[0] << std::endl;
		serv_adr.sin_addr.s_addr = inet_addr(cf.v_s[i].v_listen[1].c_str());
		int port = convert_to_num(cf.v_s[i].v_listen[0], 10);
		serv_adr.sin_port = htons(port);
		if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) //바로 주소 재사용가능하도록 설정
			throw (setsockopt_error());
		if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == -1) //바로 포트 재사용가능하도록 설정
			throw (setsockopt_error());
		if (bind(serv_sock, reinterpret_cast<struct sockaddr*>(&serv_adr), sizeof(serv_adr)) == -1)
			throw (bind_error());
		//3. server socket에 클라이언트 접속 요청 대기할 수 있도록
		if (listen(serv_sock, 2048) == -1)
			throw (listen_error());
		FD_SET(serv_sock, &(fdset.read_fds)); //read_fds에서 서버 fd 활성화
		fd_max = cal_max_fd();
		//소켓 프로그래밍에서 4.클라이언트 접속 요청 수락하는 부분, 5.클라이언트와 연결된 소켓으로 데이터 송수신하는 부분은 다른 메서드에서
	}
}

connection::~connection(void)
{
	for (iterator it = fd_arr.begin(); it != fd_arr.end(); ++it)
		close(it->fd);
	fd_arr.clear();
}

connection::iterator	connection::fd_arr_begin(void)
{
	return (fd_arr.begin());
}

connection::iterator	connection::client_begin(void)
{
	iterator it = fd_arr.begin();
	std::advance(it, server_size);
	return (it);
}

connection::iterator	connection::fd_arr_end(void)
{
	return (fd_arr.end());
}

bool					connection::is_server_socket(int fd)
{
	//서버 소켓에 입력이 들어오는 건 클라리언트 접속 요청이기 때문에 현재 fd가 서버 소켓인지 체크해주는 메서드
	iterator it = fd_arr.begin();
	for (int i = 0; i < server_size; i++)
	{
		if (it->fd == fd)
			return (true);
		++it;
	}
	return (false);
}

int						connection::cal_max_fd(void)
{
	int	max = 0;
	for (iterator it = fd_arr.begin(); it != fd_arr.end(); ++it)
	{
		if (max < it->fd)
			max = it->fd;
	}
	return (max);
}

int						connection::max_fd(void)
{
	return (fd_max);
}

fd_info					&connection::info_of_fd(int fd)
{
	iterator it;
	for (it = fd_arr.begin(); it != fd_arr.end(); ++it)
	{
		if (it->fd == fd)
			break ;
	}
	return (*it);
}

void					connection::update_arr(std::vector<int> &to_move, std::vector<int> &to_connect, std::vector<int> &to_disconnect)
{
	for (int i = 0; i < (int)to_move.size(); i++)
		move_client(to_move[i]);
	for (int i = 0; i < (int)to_connect.size(); i++)
		connect_client(to_connect[i]);
	for (int i = 0; i < (int)to_disconnect.size(); i++)
	{
		disconnect_client(to_disconnect[i]);
		FD_CLR(to_disconnect[i], &(fdset.read_fds));
	}
}

void					connection::connect_client(int serv_sock)
{
	if (fd_max >= MAX_FD)
	{
		iterator it = --fd_arr.end();
		disconnect_client(it->fd);
	}

	int					clnt_sock;
	struct sockaddr_in	clnt_adr;
	socklen_t			adr_sz = sizeof(clnt_adr);

	//4.클라이언트 접속 요청 수락하는 부분
	if ((clnt_sock = accept(serv_sock, reinterpret_cast<struct sockaddr*>(&clnt_adr), &adr_sz)) == -1)
		throw (accept_error());
	//std::cout << "connecting: serv - " << serv_sock << ", clnt - " << clnt_sock << "\n";
	std::cout << "clnt info: IP - " << inet_ntoa(clnt_adr.sin_addr) << ", Port - " << ntohs(clnt_adr.sin_port) << std::endl;
	FD_SET(clnt_sock, &(fdset.read_fds));
	fcntl(clnt_sock, F_SETFL, O_NONBLOCK); //각 클라이언트 fd를 논블로킹으로 설정
	iterator it = client_begin();
	fd_arr.insert(it, fd_info(clnt_sock, info_of_fd(serv_sock).server_info));
	fd_max = cal_max_fd();
}

void					connection::disconnect_client(int clnt_sock)
{
	if (is_server_socket(clnt_sock))
		return ;
	iterator it = fd_arr.begin();
	std::advance(it, server_size);

	while (it != fd_arr.end())
	{
		if (it->fd == clnt_sock)
		{
			close(it->fd);
			fd_arr.erase(it);
			fd_max = cal_max_fd();
			return ;
		}
		++it;
	}
}

bool					connection::check_connection(int clnt_sock, std::string rp)
{
	if (is_server_socket(clnt_sock))
		return (true);
	size_t last = rp.find("Connection: ");
	last += ((std::string)"Connection: ").length();
	size_t next = rp.find("\r\n", last);
	std::string connection = rp.substr(last, next - last);
	//std::cout << "NOW connection: " << connection << std::endl;
	if (!connection.compare("keep-alive"))
		return (true);
	return (false);
}

bool					connection::get_client_msg(int clnt_sock, request &rq)
{
	int		str_len;
	char	buf[BUF_SIZE + 1];

	if ((str_len = recv(clnt_sock, buf, BUF_SIZE, 0)) <= 0)
		return (false);
	buf[str_len] = '\0';
	fd_info &clnt_info = info_of_fd(clnt_sock);
	concatenate_client_msg(clnt_info, std::string(buf, str_len)); //해당 클라이언트에 문자열 이어서 저장
	if (is_input_completed(clnt_info, rq)) //입력값을 다 받았는지 체크해 입력이 끝났다고 판단되면 read_fds에서 fd 삭제(이때 내부에서 파싱과 같은 처리도 같이..)
		FD_CLR(clnt_sock, &(fdset.read_fds));
	return (true);
}

void					connection::clear_client_msg(int clnt_sock)
{
	fd_info &clnt_info = info_of_fd(clnt_sock);
	clnt_info.tmp.clear();
	clnt_info.msg.clear();
	clnt_info.split_msg.clear();
	clnt_info.status = RQ_LINE_NOT_PARSED;
}

void				connection::move_client(int clnt_sock)
{
	iterator it;
	for (it = fd_arr.begin(); it != fd_arr.end(); ++it)
	{
		if (it->fd == clnt_sock)
			break ;
	}
	fd_arr.insert(client_begin(), *it);
	fd_arr.erase(it);
}

void					connection::concatenate_client_msg(fd_info &clnt_info, std::string to_append)
{
	//요청 메세지의 각 라인이 '\r\n'으로 끝난다고 함. 하지만 견고한 애플리케이션이라면 '\n'만 있는 경우도? 처리할 수 있어야 한다는 걸 보고 '\r'은 저장하기 전 삭제
	if (is_before_body_input(clnt_info))
		to_append.erase(remove(to_append.begin(), to_append.end(), '\r'), to_append.end());
	clnt_info.msg.append(to_append);
}

bool					connection::is_before_body_input(fd_info &clnt_info)
{
	if (clnt_info.status == RQ_LINE_NOT_PARSED || clnt_info.status == HEADER_NOT_PARSED)
		return (true);
	return (false);
}

bool					connection::is_input_completed(fd_info &clnt_info, request &rq)
{
	request::iterator	it;
	//clnt_info에 status 추가 - 디폴트 값 : RQ_LINE_NOT_PARSED, rq라인 파싱 이후 HEADER_NOT_PARSED, 헤더를 파싱한 이후에 본문 처리 여부에 따라 NO_BODY, TRANSFER_ENCODING, CONTENT_LENGTH 중 하나로 저장됨.
	if (clnt_info.status == RQ_LINE_NOT_PARSED && clnt_info.msg.find("\n") != std::string::npos)
	{
		if (!trim_first_lf(clnt_info))
			return (false);
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
	else if (clnt_info.status == MULTIPART)
		return (is_multipart_completed(clnt_info, rq));
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
	{
		std::string	type = rq.corresponding_header_value(it, "Content-Type");
		if (type.find("multipart/form-data") != std::string::npos)
			clnt_info.status = MULTIPART;
		else
			clnt_info.status = CONTENT_LENGTH;
	}
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
				length = convert_to_num(trim_last_cr(clnt_info.msg.substr(0, next)), 16); //내부에서 변환이 잘못 됐거나 음수면 에러 throw
				last = next + 1;
				tmp = last;
				int	clnt_length = 0;
				while (true)
				{
					if ((next = clnt_info.msg.find("\n", tmp)) == std::string::npos)
						return (false);
					std::string	tmp_str = trim_last_crlf(clnt_info.msg.substr(last, next - last + 1), length);
					clnt_length = tmp_str.length();
					if (!length)
						return (completed_input(rq, it, clnt_info.tmp, -1));
					if (length == clnt_length)
					{
						clnt_info.tmp.append(tmp_str);
						clnt_info.msg = clnt_info.msg.substr(next + 1);
						break ;
					}
					else if (length < clnt_length)
						throw (incorrect_body_length_error());
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
	std::string			body;

	try
	{
		length = convert_to_num(rq.corresponding_header_value(it, "Content-Length"), 10);
		body = trim_last_crlf(clnt_info.msg, length);
		int clnt_length = body.length();

		if (clnt_length < length)
			return (false);
		else if (clnt_length > length)
			throw (incorrect_body_length_error());
	}
	catch (const std::exception& e)
	{
		return (completed_input(rq, it, "", 400));
	}
	return (completed_input(rq, it, clnt_info.msg, -1));
}

bool					connection::is_multipart_completed(fd_info &clnt_info, request &rq)
{
	request::iterator	it = rq.find_clnt_in_tmp(clnt_info.fd);
	std::string		type = rq.corresponding_header_value(it, "Content-Type");
	size_t			idx = type.find("boundary=") + ((std::string)"boundary=").length();
	std::string		boundary = type.substr(idx);
	//std::cout << "boundary: " << boundary << std::endl;
	boundary += "--";
	if (clnt_info.msg.rfind(boundary) != std::string::npos)
		return (completed_input(rq, it, clnt_info.msg, -1));
	return (false);
}

int						connection::convert_to_num(std::string str_length, int radix)
{
	if (str_length.empty())
		throw (incorrect_body_length_error());
	char	*end;
	int		length = static_cast<int>(strtol(str_length.c_str(), &end, radix));
	if (*end || length < 0)
		throw (incorrect_body_length_error());
	return (length);
}

bool					connection::trim_first_lf(fd_info &clnt_info)
{
	//std::cout << "[trim_first_lf]\n";
	int i = 0;
	while (clnt_info.msg[i] == '\n')
		i++;
	clnt_info.msg = clnt_info.msg.substr(i);
	//std::cout << "substr msg: " << clnt_info.msg << "<end>\n";
	if (clnt_info.msg.find("\n") != std::string::npos)
		return (true);
	return (false);
}

std::string				connection::trim_last_cr(std::string msg)
{
	if (msg[msg.length() - 1] == '\r')
		return (msg.substr(0, msg.length() - 1));
	return (msg);
}

std::string				connection::trim_last_crlf(std::string msg, int required_length)
{
	if (msg.length() - required_length > 0 && msg.length() - required_length <= 2)
	{
		std::string	remain = msg.substr(required_length);
		if (!remain.compare("\r") || !remain.compare("\n") || !remain.compare("\r\n"))
			return (msg.substr(0, required_length));
	}
	return (msg);
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
