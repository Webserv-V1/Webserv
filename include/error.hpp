#ifndef __ERROR_HPP__
#define __ERROR_HPP__

struct argv_error : std::exception {
  const char* what() const throw() {return "You can specify a .conf path or run it without parameters.";}
};

struct open_fail : std::exception {
  const char* what() const throw() {return "config file is not open!";}
};

struct wrong_configfile_data_1: std::exception {
  const char* what() const throw() {return "1. confile is data wrong!, key that cannot be inserted";}
};

struct bracket_order_fail : std::exception {
  const char* what() const throw() {return "The order of bracket is wrong.";}
};

struct open_bracket_after_fail : std::exception {
  const char* what() const throw() {return "open bracket after come wrong data";}
};

struct wrong_configfile_data_2 : std::exception {
  const char* what() const throw() {return "2. confile is data wrong!, key that cannot be inserted";}
};

struct err_input_listen : std::exception {
  const char* what() const throw() {return "Error Occurred for input liten";}
};

struct config_error_location : std::exception {
  const char* what() const throw() {return "configfile wrong data : server not have location";}
};

struct config_error_server : std::exception {
  const char* what() const throw() {return "configfile wrong data : don't have server";}
};
struct config_error_root : std::exception {
  const char* what() const throw() {return "configfile wrong data : don't have root";}
};

struct config_error_allow_methods : std::exception {
  const char* what() const throw() {return "configfile wrong data : allow_methods";}
};
struct config_error_error_page : std::exception {
  const char* what() const throw() {return "configfile wrong data : error_page";}
};
struct config_error_client_max : std::exception {
  const char* what() const throw() {return "configfile wrong data : client_max_body_size";}
};
struct config_error_return : std::exception {
  const char* what() const throw() {return "configfile wrong data : return 번호 : 영구(301, 308), 임시(302, 303, 307, 308)";}
};

struct config_error_same_port : std::exception {
  const char* what() const throw() {return "configfile wrong data : It contains the same port number.";}
};

struct root_and_location_error : std::exception {
  const char* what() const throw() {return "root랑 location 경로 안열림!";}
};
struct mime_open_error : std::exception {
  const char* what() const throw() {return "mime_file open error!";}
};

class	socket_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'socket' function"); }
};
class	setsockopt_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'setsockopt' function"); }
};
class	bind_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'bind' function"); }
};
class	listen_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'listen' function"); }
};
class	accept_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'accept' function"); }
};
class	recv_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'recv' function"); }
};

class	send_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'send' function"); }
};

class select_error : public std::exception
{
	virtual const char	*what(void) const throw() { return ("Unexpected error while executing 'select' function"); }
};

class	invalid_header_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Invalid Header!!"); }
};
class	incorrect_body_length_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("Incorrect Body Length!!"); }
};

struct fork_error : public std::exception
{
  virtual const char	*what(void) const throw() { return ("fork error!!"); }
};

#endif
