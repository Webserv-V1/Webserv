#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <exception>
# include <sys/select.h>
# include "connection.hpp"
# include "request.hpp"

# define	BUF_SIZE 100

class select_error : public std::exception
{
	virtual const char	*what(void) const throw()
	{
		return ("Unexpected error while executing 'select' function");
	}
};


#endif
