#ifndef EXEC_REQUEST_HPP
# define EXEC_REQUEST_HPP
# include "connection.hpp"
# include "request.hpp"
# include "parsing.hpp"

class conf_index {
	public :
		int server;
		int location;
		bool autoindex_flag;

		std::string Connection;
		std::string file_type;
		std::string location_path;
		std::string root_path;
		std::string file_path;
		std::string redirect_path;
		std::string request_url;
		std::string request_url_remaining;
		std::string qurry_string;
		std::map <std::string, std::string> m_qurry_string;

	conf_index() {
		server = -1;
		location = -1;
		autoindex_flag = false;

		root_path = "";
		location_path = "";
		file_type = "";
		Connection = "keep-alive";
		file_path = "";
		redirect_path = "";
		request_url = "";
		request_url_remaining = "";
		qurry_string = "";
	}
};

void    exec_request(config &cf, fd_set &write_fds, request *rq, std::string &request_msg, std::map<int, std::string> &m_state_code, std::map<std::string, std::string > &m_mt);



#endif
