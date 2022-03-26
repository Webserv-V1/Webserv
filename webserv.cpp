#include "./include/webserv.hpp"

void	exec_method(fd_set &write_fds, request &rq)
{
	//GET, POST, DELETE 메서드에 따라 실행
	while (!rq.empty())
	{
		std::cout << "executing method(GET, POST, DELETE)" << std::endl;
		request::iterator it = rq.rq_begin(); //rq 맨 처음에 있는 값에 대해 처리
		if (!rq.is_invalid(it))	//일단 그 값이 유효할 때만 출력 -> 나중에 유효하지 않으면 해당되는 에러 페이지 호출하도록
			rq.print();
		FD_SET((it->first).fd, &write_fds); //해당 fd에 대해 출력할 수 있도록 설정
		rq.erase(it);
	}
}

void	exec_webserv(void)
{
	IO_fd_set		fds;
	connection		cn(fds.read_fds);
	struct timeval	timeout;
	int				fd_num;
	request			rq;

	while (true)
	{
		fds.cpy_read_fds = fds.read_fds;
		fds.cpy_write_fds = fds.write_fds;
		timeout.tv_sec = 1; //1초로 타임 제한
		timeout.tv_usec = 0;
		if ((fd_num = select(cn.max_fd() + 1, &(fds.cpy_read_fds), &(fds.cpy_write_fds), 0, &timeout)) == -1)
			throw (select_error());
		else if (!fd_num)
			continue ;
		std::cout << "처리할 fd가 있는 상태" << std::endl;
		std::cout << "\t현재 존재하는 fd (num - " << fd_num << ")" << std::endl << "\t";
		for (connection::iterator it = cn.fd_arr_begin(); it != cn. fd_arr_end(); ++it)
			std::cout << it->fd << " ";
		std::cout << std::endl;
		for (connection::iterator it = cn.fd_arr_begin(); it != cn.fd_arr_end(); ++it)
		{	
			std::cout << "in for loop - " << it->fd << "\n";
			if (!it->fd)
				return ;
			if (FD_ISSET(it->fd, &(fds.cpy_read_fds)))
			{
				if (cn.is_server_socket(it->fd))
				{
					std::cout << "it's server socket - need connecting" << std::endl;
					cn.connect_client(it->fd);
					break ;
				}
				else
				{
					std::cout << "no server socket - getting msg : " << it->fd << std::endl;
					cn.get_client_msg(it->fd, rq);
				}
			}
			if (FD_ISSET(it->fd, &(fds.cpy_write_fds)))
			{
				std::cout << "출력할 게 있으! " << it->fd << std::endl;
				/*std::string	rp = "print done!\n";
				send(1, rp.c_str(), strlen(rp.c_str()), 0);*/ //여기선 단순히 문자열 출력하는 방법
				std::string	rp = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 16\r\nContent-type: text/html\r\n\r\n<h1>TESTING</h1>";
				send(it->fd, rp.c_str(), strlen(rp.c_str()), 0); //recv에 맞춰서 write도 send로 변경
				FD_CLR(it->fd, &(fds.write_fds));
				cn.disconnect_client(it->fd); //출력까지 하고 나서 cn 내부에 아직 남아있는 해당 fd 정보를 삭제
				break ;
			}
		}
		std::cout << "집합 검사 끝\n";
		exec_method(fds.write_fds, rq);
		std::cout << "메서드 실행 끝\n";
	}
}

int		main(void)
{
	try
	{
		exec_webserv(); //나중에 config 받아서~
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}
