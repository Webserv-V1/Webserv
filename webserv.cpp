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
	timeout.tv_sec = 0; //논블로킹으로 서버 작동해야 하기 때문에 타임제한 0초
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
			{
				if (!(*cn).get_client_msg(it->fd, (*rq)))
					break ;
			}
		}
		if (FD_ISSET(it->fd, &((*fds).cpy_write_fds)))
		{
			std::string rp = request_msg;
			send(it->fd, rp.c_str(), strlen(rp.c_str()), 0); //recv에 맞춰서 write도 send로 변경
			FD_CLR(it->fd, &((*fds).write_fds));
			FD_SET(it->fd, &((*fds).read_fds));
			(*cn).clear_client_msg(it->fd);
			if (!(*cn).check_connection(it->fd, rp))
				break ;
			//(*cn).disconnect_client(it->fd); //출력까지 하고 나서 cn 내부에 아직 남아있는 해당 fd 정보를 삭제
		}
	}
	return false;
}

void make_map_mime_types (std::map<std::string, std::string > &m_mt, std::string readLine)
{
	int flag = 0;
	std::string key;

	for(size_t i = 0; i < readLine.size(); i++)
	{
		if(isalpha(readLine[i]))
		{
			size_t j;
			std::string s_tmp = "";
			for(j = i; j < readLine.size(); j++)
			{
				if(readLine[j] == ' ' || readLine[j] == '	' || readLine[j] == ';')
					break;
				s_tmp += readLine[j];
			}
			if(flag == 0)
			{
				key = s_tmp;
				flag++;
			}
			else
				m_mt[s_tmp] = key;
			i = j;
		}
	}
}

void set_m_state_code(std::map<int, std::string> &m_state_code )
{
	m_state_code[100] = "Continue";
	m_state_code[101] = "Switching Protocols";
	m_state_code[200] = "OK";
	m_state_code[201] = "Created";
	m_state_code[202] = "Accepted";
	m_state_code[203] = "Non-Authoritative Information";
	m_state_code[204] = "No Content";
	m_state_code[205] = "Reset Content";
	m_state_code[206] = "Partial Content";
	m_state_code[300] = "Multiple Choices";
	m_state_code[301] = "Moved Permanently";
	m_state_code[302] = "Found";
	m_state_code[303] = "See Other";
	m_state_code[304] = "Not Modified";
	m_state_code[305] = "Use Proxy";
	m_state_code[307] = "Temporary Redirect";
	m_state_code[400] = "Bad Request";
	m_state_code[401] = "Unauthorized";
	m_state_code[402] = "Payment Required";
	m_state_code[403] = "Forbidden";
	m_state_code[404] = "Not Found";
	m_state_code[405] = "Method Not Allowed";
	m_state_code[406] = "Not Acceptable";
	m_state_code[407] = "Proxy Authentication Required";
	m_state_code[408] = "Request Timeout";
	m_state_code[409] = "Conflict";
	m_state_code[410] = "Gone";
	m_state_code[411] = "Length Required";
	m_state_code[412] = "Precondition Failed";
	m_state_code[413] = "Payload Too Large";
	m_state_code[414] = "URI Too Long";
	m_state_code[415] = "Unsupported Media Type";
	m_state_code[416] = "Range Not Satisfiable";
	m_state_code[417] = "Expectation Failed";
	m_state_code[426] = "Upgrade Required";
	m_state_code[500] = "Internal Server Error";
	m_state_code[501] = "Not Implemented";
	m_state_code[502] = "Bad Gateway";
	m_state_code[503] = "Service Unavailable";
	m_state_code[504] = "Gateway Timeout";
	m_state_code[505] = "HTTP Version Not Supported";
}

void set_m_mt(std::map<std::string, std::string > &m_mt)
{
	std::string readLine ="";
	std::ifstream readFile;

	readFile.open("./mime.types.default");
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
			make_map_mime_types (m_mt, readLine);
		readFile.close();
	}
	else{
		throw mime_open_error();
	}
}

void set(std::map<int, std::string> &m_state_code, std::map<std::string, std::string > &m_mt)
{
	set_m_state_code(m_state_code);
	set_m_mt(m_mt);
}

void	exec_webserv(config &cf)
{
	IO_fd_set		fds;
	connection		cn(cf, fds.read_fds);
	request			rq;
	std::string		request_msg;
	std::map<int, std::string> m_state_code;
	std::map<std::string, std::string > m_mt;

	set(m_state_code, m_mt);
	while (true)
	{
		if (connect_socket_and_parsing(&fds, &cn, &rq, request_msg))
			continue;
		exec_request(cf, fds.write_fds, &rq, request_msg, m_state_code, m_mt);
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
