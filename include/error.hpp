#ifndef __ERROR_HPP__
#define __ERROR_HPP__

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
  const char* what() const throw() {return "configfile server not have location";}
};

struct config_error_server : std::exception {
  const char* what() const throw() {return "configfile don't have server";}
};
struct config_error_root : std::exception {
  const char* what() const throw() {return "configfile don't have root";}
};

struct config_error_allow_methods : std::exception {
  const char* what() const throw() {return "configfile wrong data for allow_methods";}
};
struct config_error_error_page : std::exception {
  const char* what() const throw() {return "configfile wrong data for error_page";}
};
struct config_error_client_max : std::exception {
  const char* what() const throw() {return "configfile wrong data for client_max_body_size";}
};
struct config_error_return : std::exception {
  const char* what() const throw() {return "configfile wrong data for return\nreturn 번호 : 영구(301, 308), 임시(302, 303, 307, 308)";}
};

struct root_and_location_error : std::exception {
  const char* what() const throw() {return "root랑 location 경로 안열림!";}
};
struct mime_open_error : std::exception {
  const char* what() const throw() {return "mime_file open error!";}
};


#endif
