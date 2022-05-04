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
void CGI_preprocessing::set_env_and_argv(char ***env_arr, char **argv)
{
	(*env_arr) = new char*[env.size() + 1];
	int i = 0;
	for (env_iterator it = env.begin(); it != env.end(); ++it)
	{
		std::string	tmp = it->first + "=" + it->second;
		(*env_arr)[i] = new char[tmp.length() + 1];
		strcpy((*env_arr)[i++], tmp.c_str());
	}
	(*env_arr)[i] = 0;
		std::cout << "checking env_arr start\n";
		for (int i = 0; (*env_arr)[i]; i++)
		{
			std::cout << (*env_arr)[i] << std::endl;
		}
		std::cout << "checking env_arr end\n";

//		argv[0] = new char[env["SCRIPT_NAME"].length() + 1];
//		strcpy(argv[0], env["SCRIPT_NAME"].c_str());
//		argv[1] = 0;
	std::string cgibin_path = "/Users/hoyonglee/goinfre/Webserv/cgi-bin/php-cgi";
	std::string binfile_path = env["SCRIPT_NAME"];
	argv[0] = new char[cgibin_path.size() + 1];
	strcpy(argv[0], cgibin_path.c_str());
	argv[1] = new char[binfile_path.size() + 1];
	strcpy(argv[1], binfile_path.c_str());
	argv[2] = 0;

//		std::cout << "cgibin_path : " << cgibin_path << std::endl;
//		std::cout << "argv 1,[0] : " << argv[0] << std::endl;
//		std::cout << "argv 1,[1] : " << argv[1] << std::endl;
}
std::string		CGI_preprocessing::exec_CGI(void)
{
	char		**env_arr;
	char		*argv[3];
	pid_t		pid;
	std::string	res = "";
	FILE		*fin = tmpfile(); //설명 : 임시파일 만듬.
	FILE		*fout = tmpfile(); // 설명 : 임시파일 만듬.
	int			fdin = fileno(fin); // 설명 : 임시파일의 fn을 fdin에 넣음.
	int			fdout = fileno(fout);

	try
	{
		set_env_and_argv(&env_arr, argv);

		write(fdin, body.c_str(), body.length());
		lseek(fdin, 0, SEEK_SET); 
		//설명 : write 쓰고나서 fdin 맨앞으로 커서이동.

		if ((pid = fork()) == -1)
			throw (fork_error());
		else if (!pid) //자식은 여기 탑니다.
		{
			dup2(fdin, STDIN_FILENO);
			dup2(fdout, STDOUT_FILENO);
			execve(argv[0], argv, env_arr);
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
