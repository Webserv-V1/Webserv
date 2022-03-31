#ifndef EXEC_REQUEST_HPP
# define EXEC_REQUEST_HPP
# include "connection.hpp"
# include "request.hpp"
# include "parsing.hpp"

void    exec_request(config &cf, fd_set &write_fds, request *rq, std::string &request_msg);

#endif
