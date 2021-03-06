#include "../include/request.hpp"

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

size_t					request::size(void) const
{
	return (rq.size());
}

bool					request::is_invalid(request::iterator &it)
{
	return ((it->first).is_invalid);
}

int						request::get_errno(request::iterator &it)
{
	return ((it->first).err_no);
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
		for (it2 = (it->second).begin(); it2 != (it->second).end(); ++it2)
			std::cout << "\tkey: " << it2->first << ", value: " << it2->second << std::endl;
	}
}

request::iterator		request::insert_rq_line(int clnt_fd, server *server_info, std::string rq_line)
{
	request::first_type		first(clnt_fd, server_info);
	size_t					next, last = 0;
	for (int i = 0; i < 3; i++)
	{
		if ((next = rq_line.find(" ", last)) == std::string::npos && i != 2)
			break ;
		if (!i)
			first.method = rq_line.substr(last, next - last);
		else if (i == 1)
			first.url = rq_line.substr(last, next - last);
		else
			first.version = rq_line.substr(last);
		last = next + 1;
	}
	tmp_rq.push_back(std::make_pair(first, second_type()));
	request::iterator	it = --tmp_rq.end();
	if (check_info_invalid(first))
		set_error(it, 400);
	return (it);
}

request::iterator		request::insert_header(iterator &it, std::vector<std::string> msg_header)
{
	size_t		next;
	std::string	key, value;
	for (int i = 0; i < (int)msg_header.size(); i++)
	{
		next = msg_header[i].find(":"); //??? ????????? ":" ???????????? ??????
		key = msg_header[i].substr(0, next);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower); //key?????? ???????????? ?????? ??????
		std::string	tmp = msg_header[i].substr(next + 1); //?????? ?????? ??????
		tmp = tmp.erase(tmp.find_last_not_of(" ") + 1);
		value = tmp.erase(0, tmp.find_first_not_of(" "));
		std::pair<second_type::iterator, bool>	res = (it->second).insert(std::make_pair(key, value));
		if (!res.second)
		{
			set_error(it, 400);
			break ;
		}
	}
	if (check_header_invalid(it->second))
		set_error(it, 400);
	return (--tmp_rq.end());
}

request::iterator		request::insert(request::iterator &it, std::string msg_body)
{
	(it->second).insert(std::make_pair("", msg_body));
	rq.insert(rq.end(), *it);
	tmp_rq.erase(it);
	return (--rq.end());
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
		//????????? key??? ???????????? ?????? ??? ?????? 
		if ((it->first).empty())
			return (true);
		for (int i = 0; (it->first)[i]; i++)
		{
			if (isspace((it->first)[i]))
				return (true);
		}
	}
	//???????????? Host ????????? ??? ????????? ???.
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
	std::transform(header.begin(), header.end(), header.begin(), ::tolower); //???????????? ?????? ??????
	request::second_type::iterator it2 = (it->second).find(header);
	if (it2 == (it->second).end())
		return (false);
	return (true);
}

std::string				request::corresponding_header_value(request::iterator &it, std::string header)
{
	std::transform(header.begin(), header.end(), header.begin(), ::tolower); //???????????? ?????? ??????
	request::second_type::iterator it2 = (it->second).find(header);
	if (it2 == (it->second).end())
		return ("");
	return (it2->second);
}

std::string				request::get_body(request::iterator &it)
{
	return (it->second[""]);
}

bool					request::set_error(request::iterator &it, int err_no)
{
	if ((it->first).is_invalid)
		return (false);
	(it->first).is_invalid = true;
	(it->first).err_no = err_no;
	return (true);
}
