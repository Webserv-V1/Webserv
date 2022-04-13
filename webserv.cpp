#include "./include/webserv.hpp"
#include "./include/exec_request.hpp"
#include "./include/parsing.hpp"
#include "./include/error.hpp"
#include "./default_conf.hpp"
#include <time.h>

bool	connect_socket_and_parsing(IO_fd_set *fds, connection *cn, request *rq, std::string &request_msg)
{
	struct timeval	timeout;
	int				fd_num;

	(*fds).cpy_read_fds = (*fds).read_fds;
	(*fds).cpy_write_fds = (*fds).write_fds;
	timeout.tv_sec = 1; //1초로 타임 제한
	timeout.tv_usec = 0;
	if ((fd_num = select((*cn).max_fd() + 1, &((*fds).cpy_read_fds), &((*fds).cpy_write_fds), 0, &timeout)) == -1)
		throw (select_error());
	else if (!fd_num)
		return true;
	for (connection::iterator it = (*cn).fd_arr_begin(); it != (*cn).fd_arr_end(); ++it)
	{	
		if (FD_ISSET(it->fd, &((*fds).cpy_read_fds)))
		{
			if ((*cn).is_server_socket(it->fd))
			{
				(*cn).connect_client(it->fd);
				break ;
			}
			else
				(*cn).get_client_msg(it->fd, (*rq));
		}
		if (FD_ISSET(it->fd, &((*fds).cpy_write_fds)))
		{
			/*std::string	rp = "print done!\n";
			send(1, rp.c_str(), strlen(rp.c_str()), 0);*/ //여기선 단순히 문자열 출력하는 방법
	//		std::string	rp = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 16\r\nContent-type: text/html\r\n\r\n<h1>TESTING</h1>";
			std::string rp = request_msg;
			send(it->fd, rp.c_str(), strlen(rp.c_str()), 0); //recv에 맞춰서 write도 send로 변경
			FD_CLR(it->fd, &((*fds).write_fds));
			(*cn).disconnect_client(it->fd); //출력까지 하고 나서 cn 내부에 아직 남아있는 해당 fd 정보를 삭제
			break ;
		}
	}
	return false;
}

void	exec_webserv(config &cf)
{
	IO_fd_set		fds;
	connection		cn(fds.read_fds);
	request			rq;
	std::string		request_msg;

	while (true)
	{
		if (connect_socket_and_parsing(&fds, &cn, &rq, request_msg))
			continue;
		exec_request(cf, fds.write_fds, &rq, request_msg);
	}
}

int		main(void)
{
	try
	{
		config cf(WEBSERV_CONF_PATH);
		config_parsing(cf);
//		print_cf_data(cf); //이걸로 cf출력 볼수 있습니다. 
		exec_webserv(cf); //나중에 config 받아서~
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	catch(...)
	{
		std::cout << "error 인데, 여긴 타면 안되지..." << std::endl;
	}
	return (0);
}
