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

void					request::insert(int clnt_fd, std::vector<std::string> msg)
{
	request::first_type		first(clnt_fd);
	request::second_type	second = parse(first, msg);
	rq.push_back(std::make_pair(first, second));
}

void					request::erase(request::iterator it)
{
	rq.erase(it);
}

request::second_type	request::parse(request::first_type &first, std::vector<std::string> msg)
{
	int						i;
	size_t					last  = 0;
	size_t					next = 0;
	request::second_type	res;

	//첫 번째 줄 파싱하고 유효성 검사
	for (i = 0; i < 3; i++)
	{
		next = msg[0].find(" ", last);
		if (!i)
			first.method = msg[0].substr(last, next - last);
		else if (i == 1)
			first.url = msg[0].substr(last, next - last);
		else
			first.version = msg[0].substr(last);
		last = next + 1;
	}
	if (check_info_invalid(first))
		first.is_invalid = true;
	
	i = 1;
	std::string	key;
	std::string	value;

	while (!msg[i].empty()) //헤더와 본문 사이 빈 문자열을 만나기 전까지 헤더 파싱
	{
		next = msg[i].find(": "); //각 헤더를 ": " 기준으로 분리
		key = msg[i].substr(0, next);
		value = msg[i].substr(next + 2);
		res.insert(std::make_pair(key, value));
		i++;
	}
	if (check_header_invalid(res))
		first.is_invalid = true;
	i++;
	res.insert(std::make_pair("", msg[i])); //본문은 key를 빈 문자열로 하여 맵에 저장
	if (first.is_invalid)
		return (second_type());
	return (res);
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

		//헤더 내부 key, value를 구분할 때 :를 기준으로 하는 것 같아 value에 :가 또 있으면 에러 -> 나중에 다시 확인
		if ((it->second).find(": ") != std::string::npos)
			return (true);
	}
	//헤더에서 Host 정보가 꼭 있어야 함.
	if (second.find("Host") == second.end())
		return (true);
	return (false);
}
