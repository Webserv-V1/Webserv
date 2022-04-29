#ifndef EXEC_REQUEST_HPP
# define EXEC_REQUEST_HPP
#include <iostream>
#include <exception>
#include <iostream>
#include <string>
#include <cstdio>
#include <algorithm>    // std::replace
#include <sstream>
#include "request.hpp"
#include "response.hpp"
#include "connection.hpp"
#include "parsing.hpp"
#include "cgi_preprocessing.hpp"
#include "error.hpp"
#include "../default_conf.hpp"
#include <sys/select.h> /* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

class CGI_preprocessing;

class conf_index {
	public :
		int server;
		int location;
		bool cgi_flag;
		bool autoindex_flag;
		bool url_folder_flag;

		std::string Connection;
		std::string file_type;
		std::string location_path;
		std::string root_path;
		std::string file_path;
		std::string redirect_path;
		std::string request_url;
		std::string request_url_remaining;
		std::string query_string;
		std::map <std::string, std::string> m_query_string;

	conf_index() {
		server = -1;
		location = -1;
		cgi_flag = false;
		autoindex_flag = false;
		url_folder_flag = false;
		root_path = "";
		location_path = "";
		file_type = "";
		Connection = "keep-alive";
		file_path = "";
		redirect_path = "";
		request_url = "";
		request_url_remaining = "";
		query_string = "";
	}
};

void    exec_request(config &cf, fd_set &write_fds, request *rq, response *rp, std::map<int, std::string> &m_state_code, std::map<std::string, std::string > &m_mt);
void	make_request(int &state_code, std::string &request_msg, conf_index &cf_i, std::map<int, std::string> &m_state_code, config &cf, std::map<std::string, std::string > &m_mt, request *rq);


#endif
