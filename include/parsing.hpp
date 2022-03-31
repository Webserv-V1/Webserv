#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <exception>

#ifndef __PARSING__HPP__
#define __PARSING__HPP__

///struct open_fail : std::exception {
///  const char* what() const throw() {return "config file is not open!";}
///};
///
///struct wrong_configfile_data_1: std::exception {
///  const char* what() const throw() {return "1. confile is data wrong!, key that cannot be inserted";}
///};
///
///struct bracket_order_fail : std::exception {
///  const char* what() const throw() {return "The order of bracket is wrong.";}
///};
///
///struct open_bracket_after_fail : std::exception {
///  const char* what() const throw() {return "open bracket after come wrong data";}
///};
///
///struct wrong_configfile_data_2 : std::exception {
///  const char* what() const throw() {return "2. confile is data wrong!, key that cannot be inserted";}
///};
///
///struct err_input_listen : std::exception {
///  const char* what() const throw() {return "Error Occurred for input liten";}
///};

class location{
public :
	std::map< std::string, std::vector<std::string> >	m_location;
};

class server{
public :
	int			location_i;
	int			location_flag;
	std::string listen;
	std::vector<location>		v_l;
	std::vector<std::string>	v_listen; //[0] 포트 [1 ~ 4] ip
	std::vector< std::pair<int, std::vector<std::string> > > v_error_page;
	std::vector<std::string>	location_path;

	server(void)
	{
		v_listen.push_back("80");
		v_listen.push_back("0.0.0.0");
		listen = "127.0.0.1:80";
		location_i = -1;
		location_flag = 0;
	}
};

class config{
public :
	int					server_i;
	int					server_flag;
	int					bracket;
	std::string			file_name;

	std::vector<server>			v_s;
	std::vector<std::string>	v_server_invalid_key;
	std::vector<std::string>	v_location_invalid_key;
	std::stack<std::string>		s_bracket_open_order;

	config(std::string _file_name){
		server_i = -1;
		server_flag = 0;
		bracket = 0;
		file_name = _file_name;
		std::string s_invalid_key[] = {"location", "listen", "error_page"};
		std::string l_invalid_key[] = { "error_page", "allow_methods", \
				"request_max_body_size", "root", "index", "auto_index", \
				"cgi_extension", "auth_key", "client_body_buffer_size", "upload_path", "return"};

	v_server_invalid_key.insert(v_server_invalid_key.begin(), s_invalid_key, s_invalid_key + sizeof(s_invalid_key) / sizeof(std::string));
	v_location_invalid_key.insert(v_location_invalid_key.begin(), l_invalid_key, l_invalid_key + sizeof(l_invalid_key) / sizeof(std::string));
	}
private :

};

void	check_invalid(std::string &readLine, std::string com, int i);
void	check_closebrackket_order(config &cf);
void	input_listen(config &cf, std::string &save_data, \
		const int &input_data_cnt);
void	input_data(config &cf, std::string &save_data, \
		int &input_flag, std::string readLine, int i, int &input_data_cnt);
void	after_server(config &cf, int &i, std::string &readLine);
void	after_location(config &cf, int &i, std::string &readLine);
int		check_right_value_for_sever(config &cf, \
		std::string &readLine, int &i);
void	check_first_char(config &cf, int &input_flag, \
		std::string &readLine, int &i, int &input_data_cnt);
void	set_cf(config &cf, std::string readLine);
void	config_parsing(config &cf);
void	print_cf_data(config &cf);

#endif
