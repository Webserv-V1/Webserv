#include "./include/cgi_preprocessing.hpp"

CGI_preprocessing::CGI_preprocessing(request &rq, conf_index &cf_i)
{
	char path[256];
	env["DOCUMENT_ROOT"] = (std::string)getcwd(path, 256);
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SCRIPT_NAME"] = cf_i.request_url;
	env["SCRIPT_FILENAME"] = (std::string)path + cf_i.request_url;
	env["REDIRECT_STATUS"] = "200";
	env["PHP_SELF"] = cf_i.request_url;
	env["PATH_INFO"] = (std::string)path + cf_i.request_url;
	request::iterator it = rq.rq_begin();
	env["REQUEST_URI"] = it->first.url;
	env["REQUEST_METHOD"] = it->first.method;
	body = rq.get_body(it);
	env["CONTENT_TYPE"] = rq.corresponding_header_value(it, "CONTENT-TYPE");
	if (env["CONTENT_TYPE"].empty())
		env["CONTENT_TYPE"] = "text/html";
	/*std::stringstream stream;
	stream << body.length();
	std::string len_to_string;
	stream >> len_to_string;*/
	env["CONTENT_LENGTH"] = rq.corresponding_header_value(it, "CONTENT-LENGTH");
	//env["CONTENT_LENGTH"] = len_to_string;
	env["QUERY_STRING"] = cf_i.query_string; //나중에 쿼리 스트링으로 수정
	//env["REMOTE_ADDR"] = rq.corresponding_header_value(it, "HOST");
	env["SERVER_NAME"] = "0.0.0.0";
	//env["SERVER_NAME"] = it->first.server_info->v_listen[1];
	env["SERVER_PORT"] = it->first.server_info->v_listen[0];
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "Webserv/1.0";
	env["HTTP_HOST"] = "0.0.0.0" + it->first.server_info->v_listen[0];
	//env["HTTP_HOST"] = it->first.server_info->v_listen[1];
	env["HTTP_USER_AGENT"] = rq.corresponding_header_value(it, "User-Agent");
	env["HTTP_ACCEPT_LANGUAGE"] = rq.corresponding_header_value(it, "Accept-Language");
	env["HTTP_ACCEPT_ENCODING"] = rq.corresponding_header_value(it, "Accept-Encoding");
}

CGI_preprocessing::~CGI_preprocessing()
{
	env.clear();
}

std::string		CGI_preprocessing::exec_CGI(void)
{
	char		**env_arr;
	char		*argv[3];
	pid_t		pid;
	std::string	res = "";
	FILE		*fin = tmpfile();
	FILE		*fout = tmpfile();
	int			fdin = fileno(fin);
	int			fdout = fileno(fout);
	try
	{
		env_arr = new char*[env.size() + 1];
		int i = 0;
		for (env_iterator it = env.begin(); it != env.end(); ++it)
		{
			std::string	tmp = it->first + "=" + it->second;
			env_arr[i] = new char[tmp.length() + 1];
			strcpy(env_arr[i++], tmp.c_str());
		}
		env_arr[i] = 0;
		argv[0] = new char[env["PATH_INFO"].length() + 1];
		argv[1] = new char[env["SCRIPT_FILENAME"].length() + 1];
		strcpy(argv[0], env["PATH_INFO"].c_str());
		strcpy(argv[1], env["SCRIPT_FILENAME"].c_str());
		argv[2] = 0;

		std::cout << "checking env_arr\n";
		for (int i = 0; env_arr[i]; i++)
		{
			std::cout << env_arr[i] << std::endl;
		}

		write(fdin, body.c_str(), body.length());
		if ((pid = fork()) == -1)
			throw (fork_error());
		else if (!pid)
		{
			std::cout << argv[1] << std::endl;
			dup2(fdin, STDIN_FILENO);
			dup2(fdout, STDOUT_FILENO);
			execve(env["PATH_INFO"].c_str(), argv, env_arr);
			write(STDOUT_FILENO, "Status: 500 Internal Server Error\r\n\r\n", 37);
			exit(1);
		}

		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);

		lseek(fdout, 0, SEEK_SET);
		int len;
		char buf[BUF_SIZE + 1];
		while ((len = read(fdout, buf, BUF_SIZE)) >= 0)
		{
			buf[len] = '\0';
			res += std::string(buf, len);
			if (!len)
				break ;
		}
	}
	catch (const std::exception& e)
	{
		res = "Status: 500 Internal Server Error\r\n\r\n";
	}
	for (int i = 0; env_arr[i]; i++)
		delete[] env_arr[i];
	delete[] env_arr;
	delete argv[0];
	fclose(fin);
	fclose(fout);
	return (res);
}
