#include "../include/webserv.hpp"

bool	connect_socket_and_parsing(connection *cn, request *rq, response *rp)
{
	struct timeval	timeout;
	int				fd_num;
	std::vector<int>	to_move;
	std::vector<int>	to_connect;
	std::vector<int>	to_disconnect;

	cn->fdset.cpy_read_fds = cn->fdset.read_fds;
	cn->fdset.cpy_write_fds = cn->fdset.write_fds;
	timeout.tv_sec = 0; //논블로킹으로 서버 작동해야 하기 때문에 타임제한 0초
	timeout.tv_usec = 0;
	if ((fd_num = select((*cn).max_fd() + 1, &(cn->fdset.cpy_read_fds), &(cn->fdset.cpy_write_fds), 0, &timeout)) == -1)
		throw (select_error());
	else if (!fd_num)
		return true;
	for (connection::iterator it = (*cn).fd_arr_begin(); it != (*cn).fd_arr_end(); ++it)
	{
		if (FD_ISSET(it->fd, &(cn->fdset.cpy_read_fds)))
		{
			if ((*cn).is_server_socket(it->fd))
				to_connect.push_back(it->fd);
			else
			{
				if (!(*cn).get_client_msg(it->fd, (*rq)))
					to_disconnect.push_back(it->fd);
				else
					to_move.push_back(it->fd);
			}
		}
		if (FD_ISSET(it->fd, &(cn->fdset.cpy_write_fds)))
		{
			response::iterator rp_it = rp->find_clnt(it->fd);
			int	ret;
			ret = send(it->fd, (rp_it->second).c_str(), strlen((rp_it->second).c_str()), 0);
			FD_CLR(it->fd, &(cn->fdset.write_fds));
			(*cn).clear_client_msg(it->fd);
			if (ret <= 0 || !(*cn).check_connection(it->fd, rp_it->second))
				to_disconnect.push_back(it->fd);
			else
				FD_SET(it->fd, &(cn->fdset.read_fds));
			rp->erase_rp(rp_it);			
		}
	}
	(*cn).update_arr(to_move, to_connect, to_disconnect);
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

	readFile.open(MIME_TYPE_PATH);
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
	connection		cn(cf);
	request			rq;
	std::map<int, std::string> m_state_code;
	std::map<std::string, std::string > m_mt;
	response		rp(cn.fdset.write_fds);

	set(m_state_code, m_mt);

	while (true)
	{
		if (connect_socket_and_parsing(&cn, &rq, &rp))
			continue;
		exec_request(cf, &rq, &rp, m_state_code, m_mt);
		usleep(1000);
	}
}

std::string confirm_conf_path(int argc, char** argv)
{
	if(argc == 1)
		return (WEBSERV_CONF_PATH);
	else if(argc == 2)
	{
		std::string tmp(argv[1]);
		if(tmp.size() <= 5)
			throw (argv_error());
		else
		{
			if(tmp.substr(tmp.size() - 5) != ".conf")
				throw (argv_error());
		}
	}
	else
		throw argv_error();

	return(argv[1]);
}

int		main(int argc, char* argv[])
{
	try
	{
		config cf(confirm_conf_path(argc, argv));
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
