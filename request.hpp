#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>

class base_request
{
public:
	int			fd;
	std::string	method;
	std::string	url;
	std::string	version;
	bool		is_invalid;

	base_request(int fd_) : fd(fd_), method(""), url(""), version(""), is_invalid(false) {}

	bool	operator<(const base_request &base) const
	{
		return (fd < base.fd);
	}
};

class request
{
private:
	typedef base_request								first_type;
	typedef std::map<std::string, std::string>			second_type;

	std::map<first_type, second_type>	rq;
public:
	typedef std::map<first_type, second_type>			value_type;
	typedef std::map<first_type, second_type>::iterator	iterator;

	request(void) : rq() {}

	bool		empty(void) const
	{
		return (rq.empty());
	}

	void		print(void)
	{
		iterator	it;
		second_type::iterator	it2;
		for (it = rq.begin(); it != rq.end(); ++it)
		{
			std::cout << "fd: " << (it->first).fd << std::endl;
			std::cout << "method: " << (it->first).method << std::endl;
			std::cout << "url: " << (it->first).url << std::endl;
			std::cout << "version: " << (it->first).version << std::endl;
			std::cout << "is_invalid: " << (it->first).is_invalid << std::endl;
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

	iterator	begin(void)
	{
		return (rq.begin());
	}

	iterator	end(void)
	{
		return (rq.end());
	}

	iterator	find(int fd)
	{
		iterator	it;
		for (it = rq.begin(); it != rq.end(); ++it)
		{
			if ((it->first).fd == fd)
				return (it);
		}
		return (rq.end());
	}

	bool		is_invalid(int fd)
	{
		iterator	it = find(fd);
		if (it == rq.end())
			return (false);
		return ((it->first).is_invalid);
	}

	bool		check_info_invalid(first_type &first)
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

	bool		check_header_invalid(second_type &second)
	{
		second_type::iterator	it;

		for (it = second.begin(); it != second.end(); ++it)
		{
			//나중에 key값 체크해야 되는 지 확인 
			if ((it->first).empty())
				return (true);

			//헤더 내부 key, value를 구분할 때 :를 기준으로 하는 것 같아 value에 :가 또 있으면 에러 -> 나중에 다시 확인
			if ((it->second).find(": ") != std::string::npos)
				return (true);
		}
		//헤더에서 Host 정보가 꼭 있어야 함.
		if (second.find("Host") == second.end())
			return (true);
		return (false);
	}

	second_type	parse(first_type &first, std::vector<std::string> input)
	{
		/*int		len = input.length();
		char	tmp[len];
		strcpy(tmp, input.c_str());
		char	*token = " \r\n";
		char	*res = strtok(tmp, token);*/
		int			i;
		size_t		last = 0;
		size_t		next = 0;
		second_type	res;

		for (i = 0; i < 3; i++)
		{
			next = input[0].find(" ", last);
			if (!i)
				first.method = input[0].substr(last, next - last);
			else if (i == 1)
				first.url = input[0].substr(last, next - last);
			else
				first.version = input[0].substr(last);
			last = next + 1;
		}
		if (i != 3 || check_info_invalid(first))
			first.is_invalid = true;
		i = 1;
		std::string	key;
		std::string	value;
		while (!input[i].empty())
		{
			next = input[i].find(": ");
			key = input[i].substr(0, next);
			value = input[i].substr(next + 2);
			res.insert(std::make_pair(key, value));
			i++;
		}
		if (check_header_invalid(res))
			first.is_invalid = true;
		i++;
		res.insert(std::make_pair("", input[i]));
		if (first.is_invalid)
			return (second_type());
		return (res);
	}

	void		insert(int fd, std::vector<std::string> input)
	{
		first_type	first(fd);
		second_type	second = parse(first, input);
		rq.insert(std::make_pair(first, second));
	}

	/*void		erase(int fd)
	{
		iterator	it = find(fd);
		if (it == rq.end())
			return ;
		rq.erase(it);
	}*/

	void		erase(iterator it)
	{
		rq.erase(it);
	}
};