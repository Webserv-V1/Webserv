#include "./include/request.hpp"

request::iterator		request::rq_begin(void)
{
	return (rq.begin());
}

request::iterator		request::rq_end(void)
{
	return (rq.end());
}

bool					request::empty(void) const
{
	return (rq.empty());
}

bool					request::is_invalid(request::iterator it)
{
	return ((it->first).is_invalid);
}

void					request::print(void)
{
	request::iterator				it;
	request::second_type::iterator	it2;

	for (it = rq.begin(); it != rq.end(); ++it)
	{
		std::cout << "fd: " << (it->first).fd << std::endl;
		std::cout << "method: " << (it->first).method << std::endl;
		std::cout << "url: " << (it->first).url << std::endl;
		std::cout << "version: " << (it->first).version << std::endl;
		std::cout << "is_invalid: " << (it->first).is_invalid << std::endl;
		std::cout << "err_no: " << (it->first).err_no << std::endl;
		std::cout << "<header>" << std::endl;
		if ((it->first).is_invalid)
		{
			std::cerr << "Cannot print because it's an invalid format" << std::endl;
			continue ;
		}
		for (it2 = ++(it->second).begin(); it2 != (it->second).end(); ++it2)
			std::cout << "\tkey: " << it2->first << ", value: " << it2->second << std::endl;
		std::cout << "<body>" << std::endl;
		it2 = (it->second).begin();
		std::cout << it2->second << std::endl;
	}
}

void					request::print_tmp(void)
{
	request::iterator				it;
	request::second_type::iterator	it2;

	for (it = tmp_rq.begin(); it != tmp_rq.end(); ++it)
	{
		std::cout << "fd: " << (it->first).fd << std::endl;
		std::cout << "method: " << (it->first).method << std::endl;
		std::cout << "url: " << (it->first).url << std::endl;
		std::cout << "version: " << (it->first).version << std::endl;
		std::cout << "is_invalid: " << (it->first).is_invalid << std::endl;
		std::cout << "err_no: " << (it->first).err_no << std::endl;
		std::cout << "<header>" << std::endl;
		if ((it->first).is_invalid)
		{
			std::cerr << "Cannot print because it's an invalid format" << std::endl;
			continue ;
		}
		for (it2 = ++(it->second).begin(); it2 != (it->second).end(); ++it2)
			std::cout << "\tkey: " << it2->first << ", value: " << it2->second << std::endl;
	}
}

request::iterator		request::insert_rq_line(int clnt_fd, std::string rq_line)
{
	request::first_type		first(clnt_fd);
	size_t					next, last = 0;
	for (int i = 0; i < 3; i++)
	{
		next = rq_line.find(" ", last);
		if (!i)
			first.method = rq_line.substr(last, next - last);
		else if (i == 1)
			first.url = rq_line.substr(last, next - last);
		else
			first.version = rq_line.substr(last);
		last = next + 1;
	}
	if (check_info_invalid(first))
		first.is_invalid = true;
	tmp_rq.push_back(std::make_pair(first, second_type()));
	return (tmp_rq.end() - 1);
}

request::iterator		request::insert_header(iterator &it, std::vector<std::string> msg_header)
{
	//request::first_type		first(clnt_fd);
	//request::second_type	second = parse_header(first, msg_header);
	//tmp_rq.push_back(std::make_pair(first, second));
	size_t		next;
	std::string	key, value;
	for (int i = 0; !msg_header[i].empty(); i++)
	{
		next = msg_header[i].find(":"); //각 헤더를 ":" 기준으로 분리
		key = msg_header[i].substr(0, next);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower); //key값은 소문자로 모두 변경
		std::string	tmp = msg_header[i].substr(next + 1); //양옆 공백 제거
		tmp = tmp.erase(tmp.find_last_not_of(" ") + 1);
		value = tmp.erase(0, tmp.find_first_not_of(" "));
		(it->second).insert(std::make_pair(key, value));
	}
	if (check_header_invalid(it->second))
		(it->first).is_invalid = true;
	return (tmp_rq.end() - 1);
}

request::iterator		request::insert(request::iterator &it, std::string msg_body)
{
	(it->second).insert(std::make_pair("", msg_body));
	rq.insert(rq.end(), *it);
	tmp_rq.erase(it);
	return (rq.end() - 1);
}

void					request::erase(request::iterator &it)
{
	rq.erase(it);
}

request::iterator		request::find_clnt(int clnt_fd)
{
	request::iterator it;
	for (it = rq.begin(); it != rq.end(); ++it)
	{
		if ((it->first).fd == clnt_fd)
			break ;
	}
	return (it);
}

request::iterator		request::find_clnt_in_tmp(int clnt_fd)
{
	request::iterator it;
	for (it = tmp_rq.begin(); it != tmp_rq.end(); ++it)
	{
		if ((it->first).fd == clnt_fd)
			break ;
	}
	return (it);
}

bool					request::check_info_invalid(request::first_type &first)
{
	if (first.method.empty() || first.url.empty() || first.version.empty())
		return (true);
	if (first.method.compare("GET") && first.method.compare("POST")
		&& first.method.compare("DELETE"))
		return (true);
	if (first.version.compare("HTTP/1.1"))
		return (true);
	return (false);
}

bool					request::check_header_invalid(request::second_type &second)
{
	second_type::iterator	it;

	for (it = second.begin(); it != second.end(); ++it)
	{
		//나중에 key값 체크해야 되는 지 확인 
		if ((it->first).empty())
			return (true);
		for (int i = 0; (it->first)[i]; i++)
		{
			if (isspace((it->first)[i]))
				return (true);
		}
	}
	//헤더에서 Host 정보가 꼭 있어야 함.
	if (second.find("host") == second.end())
		return (true);
	return (false);
}

bool					request::which_method(request::iterator &it, std::string method)
{
	if (!(it->first).method.compare(method))
		return (true);
	return (false);
}

bool					request::is_existing_header(request::iterator &it, std::string header)
{
	std::transform(header.begin(), header.end(), header.begin(), ::tolower); //소문자로 모두 변경
	request::second_type::iterator it2 = (it->second).find(header);
	if (it2 == (it->second).end())
		return (false);
	return (true);
}

std::string				request::corresponding_header_value(request::iterator &it, std::string header)
{
	std::transform(header.begin(), header.end(), header.begin(), ::tolower); //소문자로 모두 변경
	request::second_type::iterator it2 = (it->second).find(header);
	return (it2->second);
}

bool					request::set_error(request::iterator &it, int err_no)
{
	if ((it->first).is_invalid)
		return (false);
	(it->first).is_invalid = true;
	(it->first).err_no = err_no;
	return (true);
}
