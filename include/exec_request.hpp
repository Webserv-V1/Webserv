#ifndef EXEC_REQUEST_HPP
# define EXEC_REQUEST_HPP
# include "connection.hpp"
# include "request.hpp"

void    exec_request(fd_set &write_fds, request &rq);

#endif
