#ifndef CGI_PREPROCESSING_HPP
# define CGI_PREPROCESSING_HPP

# include <string>
# include <map>
# include <unistd.h>
# include <cstdio>
# include <sstream>
# include "request.hpp"
# include "parsing.hpp"
# include "exec_request.hpp"
# include "error.hpp"
# include "../default_conf.hpp"

class conf_index;

class CGI_preprocessing
{
private:
	std::string							body;
	std::map<std::string, std::string>	env;
public:
	typedef std::map<std::string, std::string>::iterator	env_iterator;

	CGI_preprocessing(request &rq, conf_index &cf_i);

	~CGI_preprocessing();
	void		set_env_and_argv(char ***env_arr, char **argv);
	std::string	exec_CGI(void);
};

#endif
