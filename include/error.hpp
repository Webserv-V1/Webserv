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

#endif
