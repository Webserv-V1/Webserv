#include "./include/connection.hpp"

connection::connection(fd_set &rfds) : server_size(0), fd_arr(), read_fds(rfds)
{
	int serv_sock;
	struct sockaddr_in	serv_adr;
	//지금은 하나의 서버만 생성한다고 가정해서 작성하지만 나중엔 Config를 받아서 서버를 모두 생성하도록 수정
	//1. 서버 소켓 하나 생성
	if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw (connection::socket_error());
	server_size++;
	fd_arr.push_back(fd_info(serv_sock));
	//2. socket에 IP, Port 번호 할당
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(100); //일단은 포트 번호 100로 설정
	if (bind(serv_sock, reinterpret_cast<struct sockaddr*>(&serv_adr), sizeof(serv_adr)) == -1)
		throw (connection::bind_error());
	//3. server socket에 클라이언트 접속 요청 대기할 수 있도록 - 일단 10개의 수신 대기열
	if (listen(serv_sock, 10) == -1)
		throw (connection::listen_error());
	FD_SET(serv_sock, &read_fds); //read_fds에서 서버 fd 활성화
	//소켓 프로그래밍에서 4.클라이언트 접속 요청 수락하는 부분, 5.클라이언트와 연결된 소켓으로 데이터 송수신하는 부분은 다른 메서드에서
}

connection::~connection(void)
{
	for (int i = 0; i < server_size; i++)
		close(fd_arr[i].fd);
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
		{
			if (i < server_size)
				break ;
		}
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
	//std::cout << INADDR_ANY << std::endl;
	//std::cout << "IP : " << ntohl(clnt_adr.sin_addr.s_addr) << "\nPort : " << ntohs(clnt_adr.sin_port) << std::endl;
	FD_SET(clnt_sock, &read_fds);
	fcntl(clnt_sock, F_SETFL, O_NONBLOCK); //각 클라이언트 fd를 논블로킹으로 설정
	fd_arr.push_back(fd_info(clnt_sock, serv_sock));
	//std::cout << "clnt sock - " << clnt_sock << std::endl; //여기서 출력해보면 브라우저로 한 번 접속해도 새로운 클라이언트가 2개 생김. 
}

void					connection::disconnect_client(int clnt_sock)
{
	std::vector<fd_info>::iterator it;
	for (it = fd_arr.begin(); it != fd_arr.end(); ++it)
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

	//5.클라이언트와 연결된 소켓으로 데이터 송수신하는 부분
	if ((str_len = recv(clnt_sock, buf, BUF_SIZE, 0)) < 0) //다른 사람 코드 봤을 때 recv 한 번으로 해결하던데.. 나중에 다시 확인
		throw (recv_error());
	buf[str_len] = '\0';
	concatenate_client_msg(info_of_fd(clnt_sock), buf); //해당 클라이언트에 문자열 이어서 저장
	if (recv(clnt_sock, buf, BUF_SIZE, MSG_PEEK | MSG_DONTWAIT) < 0) //옵션을 줘서(대기열에서 빠지지 않고, 논블로킹) 다음에 읽을 문자열을 검사할 때 그 문자열이 없다면
	{
		FD_CLR(clnt_sock, &read_fds); //더 이상 입력을 받지 않도록 설정
		parse_client_msg_by_line(info_of_fd(clnt_sock)); //이때까지 저장한 문자열을 '\n'기준으로 파싱
		rq.insert(clnt_sock, info_of_fd(clnt_sock).split_msg);
		//print_client_msg(clnt_sock); //해당 fd에 파싱된 내용 출력
	}
}

void					connection::concatenate_client_msg(fd_info &clnt_info, std::string to_append)
{
	//요청 메세지의 각 라인이 '\r\n'으로 끝난다고 함. 하지만 견고한 애플리케이션이라면 '\n'만 있는 경우도? 처리할 수 있어야 한다는 걸 보고 '\r'은 저장하기 전 삭제
	to_append.erase(remove(to_append.begin(), to_append.end(), '\r'), to_append.end());
	clnt_info.msg.append(to_append);
}

void					connection::parse_client_msg_by_line(fd_info &clnt_info)
{
	size_t	last = 0;
	size_t	next = 0;

	while ((next = clnt_info.msg.find("\n", last)) != std::string::npos)
	{
		clnt_info.split_msg.push_back(clnt_info.msg.substr(last, next - last));
		last = next + 1;
		if (clnt_info.msg[last] == '\n') //헤더와 본문 사이에 개행을 하나 더 둠. -> 개행이 두 번 오고 난 다음에 오는 값들은 한 번에 저장
		{
			clnt_info.split_msg.push_back("");
			last++;
			break ;
		}
	}
	//본문 저장 - 본문이 존재하지 않는다면 ""만 저장
	next = clnt_info.msg.rfind("\n"); //본문의 마지막도 \n로 끝나는 지는 모르겠지만 이것도 나중에 체크
	clnt_info.split_msg.push_back(clnt_info.msg.substr(last, next - last));
}

void					connection::print_client_msg(int clnt_sock)
{
	std::vector<std::string>	msg = info_of_fd(clnt_sock).split_msg;
	std::vector<std::string>::iterator it;

	for (it = msg.begin(); it != msg.end(); ++it)
		std::cout << "one line" << std::endl << *it << std::endl;
	std::cout << "<end>" << std::endl << std::endl;
}
