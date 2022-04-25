#include "./include/cgi_preprocessing.hpp"

CGI_preprocessing::CGI_preprocessing(request &rq, conf_index &cf_i)
{
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SCRIPT_NAME"] = cf_i.request_url;
	request::iterator it = rq.rq_begin();
	env["REQUEST_URI"] = it->first.url;
	env["REQUEST_METHOD"] = it->first.method;
	body = rq.get_body(it);
	env["CONTENT_TYPE"] = rq.corresponding_header_value(it, "CONTENT-TYPE");
	if (env["CONTENT_TYPE"].empty())
		env["CONTENT_TYPE"] = "text/html";
	env["CONTENT_LENGTH"] = std::to_string(rq.body_length(body));
	env["QUERY_STRING"] = cf_i.query_string; //나중에 쿼리 스트링으로 수정
	env["SERVER_NAME"] = rq.corresponding_header_value(it, "HOST");
	env["SERVER_PORT"] = it->first.server_info->v_listen[0];
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "Webserv/1.0";
}

CGI_preprocessing::~CGI_preprocessing()
{
	env.clear();
}

std::string		CGI_preprocessing::exec_CGI(void)
{
	char		**env_arr;
	pid_t		pid;
	std::string	res = "";
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

		/*std::cout << "checking env_arr\n";
		for (int i = 0; env_arr[i]; i++)
		{
			std::cout << env_arr[i] << std::endl;
		}*/

		FILE	*fin = tmpfile();
		FILE	*fout = tmpfile();
		int		fdin = fileno(fin);
		int		fdout = fileno(fout);
		write(fdin, body.c_str(), body.length());

		if ((pid = fork()) == -1)
		{
			for (int i = 0; env_arr[i]; i++)
				delete[] env_arr[i];
			delete[] env_arr;
			throw (CGI_preprocessing::fork_error());
		}
		else if (!pid)
		{
			dup2(fdin, STDIN_FILENO);
			dup2(fdout, STDOUT_FILENO);
			execve(("./" + env["SCRIPT_NAME"]).c_str(), 0, env_arr);
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
			res += buf;
			if (!len)
				break ;
		}
		
		for (int i = 0; env_arr[i]; i++)
			delete[] env_arr[i];
		delete[] env_arr;
		fclose(fin);
		fclose(fout);
	}
	catch (const std::exception& e)
	{
		res = "Status: 500 Internal Server Error\r\n\r\n";
	}
	return (res);
}