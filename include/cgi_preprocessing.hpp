#ifndef CGI_PREPROCESSING_HPP
# define CGI_PREPROCESSING_HPP

# include <string>
# include <map>
# include <unistd.h>
# include <cstdio>
# include "request.hpp"
# include "parsing.hpp"
# include "exec_request.hpp"

# define BUF_SIZE 100

class CGI_preprocessing
{
private:
	std::string							body;
	std::map<std::string, std::string>	env;
public:
	typedef std::map<std::string, std::string>::iterator	env_iterator;

	CGI_preprocessing(request &rq, conf_index &cf_i);
	~CGI_preprocessing();
	std::string	exec_CGI(void);

	class	fork_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("fork error!!");
		}
	};
};

#endif