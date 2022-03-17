#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include "client_msg.hpp"
#include "request.hpp"

#define	BUF_SIZE		100 //어느 정도가 적합하려나

class fd_sets
{
public:
	int			fd_max;
	fd_set		read_fds;
	fd_set		cpy_read_fds;
	fd_set		write_fds;
	fd_set		cpy_write_fds;
	std::vector<client_msg>	input;

	fd_sets(void) : fd_max(0), input(4)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		FD_ZERO(&cpy_read_fds);
		FD_ZERO(&cpy_write_fds);
	}
};

void	error_handling(const char *message)
{
	std::cerr << message << std::endl;
	exit(1);
}

void	handle_input(int serv_sock, fd_sets &sets, int fd, request &rq)
{
	int					clnt_sock;
	struct sockaddr_in	clnt_adr;
	socklen_t			adr_sz;
	int					str_len;
	char				buf[BUF_SIZE + 1];

	if (fd == serv_sock) // 4. 클라이언트 접속 요청을 수락한다. (클라이언트와 연결된 새로운 socket이 생성된다.)
	{
		adr_sz = sizeof(clnt_adr);
		if ((clnt_sock = accept(serv_sock, reinterpret_cast<struct sockaddr*>(&clnt_adr), &adr_sz)) == -1)
			error_handling("accpet() error");
		FD_SET(clnt_sock, &(sets.read_fds));
		fcntl(clnt_sock, F_SETFL, O_NONBLOCK);
		if (sets.fd_max < clnt_sock)
		{
			sets.input.push_back(client_msg());
			sets.fd_max = clnt_sock;
		}
	}
	else // 5. 클라이언트와 연결된 socket을 통해 데이터를 송수신한다.
	{
		//str_len = read(fd, buf, BUF_SIZE);
		/*if ((str_len = recv(fd, buf, BUF_SIZE, MSG_PEEK | MSG_DONTWAIT)) < 0)
		{
			//buf[str_len] = '\0';
			std::cout << "\tno buf" << std::endl;
			FD_CLR(fd, &(sets.read_fds));
			std::cout << "\tafter clearing" << std::endl;
			rq.insert(fd, sets.input[fd].parse_line());
			std::cout << "\tafter parsing line" << std::endl;
			
			//sets.input[fd].print();

			//sets.input[fd].clear();
			//실행
			//FD_SET(fd, &(sets.write_fds));
			//close(fd);
			std::cout << "input finish - " << fd << std::endl;
		}
		else// if ((str_len = recv(fd, buf, BUF_SIZE, 0)) > 0)
		{
			str_len = recv(fd, buf, BUF_SIZE, 0);
			buf[str_len] = '\0';
			sets.input[fd].strjoin(buf);
			//write(1, buf, str_len);
		}*/
		str_len = recv(fd, buf, BUF_SIZE, 0);
		buf[str_len] = '\0';
		sets.input[fd].strjoin(buf);
		if ((str_len = recv(fd, buf, BUF_SIZE, MSG_PEEK | MSG_DONTWAIT)) < 0)
		{
			FD_CLR(fd, &(sets.read_fds));
			rq.insert(fd, sets.input[fd].parse_line());
			sets.input[fd].clear();
		}
	}
}

void	handle_output(fd_sets &sets, int fd, request &rq)
{
	/*request::iterator it = rq.find(fd);
	std::cout << "handling output" << std::endl;
	if (it != rq.end())
	{
		std::cout << "\tnot end" << std::endl;
		//rq.print();
		//rq.erase(it);
		FD_CLR(fd, &(sets.write_fds));
		close(fd);
	}*/
	write(1, "print done!\n", strlen("print done!\n"));
	FD_CLR(fd, &(sets.write_fds));
}

void	exec_method(request &rq, fd_sets &sets)
{
	std::cout << "executing method(GET, POST, DELETE)" << std::endl;
	request::iterator it = rq.begin();
	//GET, POST, DELETE 메서드에 따라 실행
	//
	rq.print();
	for (int i = (it->first).fd; i <= sets.fd_max; i++)
		sets.input[i].print();
	FD_SET((it->first).fd, &(sets.write_fds));
	rq.erase(it);
	//rq.erase(it);
}

int		main(int argc, char **argv)
{
	int					serv_sock;
	struct sockaddr_in	serv_adr;
	fd_sets				sets;
	struct timeval		timeout;
	int					fd_num;
	request				rq;

	if (argc != 2) {
		std::cout << "Usage : " << argv[0] << " <port>" << std::endl;
		exit(1);
	}

	// 1. socket 하나를 생성한다.
	if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket() error");

	// 2. socket에 IP와 Port 번호를 할당한다.
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	if (bind(serv_sock, reinterpret_cast<struct sockaddr*>(&serv_adr), sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	// 3. server socket(listen socket)을 통해 클라이언트의 접속 요청을 대기한다.
	//    10개의 수신 대기열(큐)을 생성한다.
	if (listen(serv_sock, 10) == -1) //몇 개의 대기열을 사용할지도?
		error_handling("listen() error");

	FD_SET(serv_sock, &(sets.read_fds));
	//FD_SET(serv_sock, &(sets.write_fds));
	sets.fd_max = serv_sock;
	while (1)
	{
		sets.cpy_read_fds = sets.read_fds;
		sets.cpy_write_fds = sets.write_fds;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0; //1초로 타임 제한
		if ((fd_num = select(sets.fd_max + 1, &(sets.cpy_read_fds), &(sets.cpy_write_fds), 0, &timeout)) == -1)
			error_handling("select() error");
		else if (!fd_num)
			continue ;
		for (int i = 0; i < sets.fd_max + 1; i++)
		{
			if (FD_ISSET(i, &(sets.cpy_read_fds)))
				handle_input(serv_sock, sets, i, rq);
			if (FD_ISSET(i, &(sets.cpy_write_fds)))
				handle_output(sets, i, rq);
		}
		while (!rq.empty())
			exec_method(rq, sets);
	}
	close(serv_sock);
	return (0);
}
