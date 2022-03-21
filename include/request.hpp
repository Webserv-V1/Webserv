#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <vector>
# include <map>

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
	typedef base_request								first_type; //클라이언트 fd, 첫 번쨰 줄에 오는 정보 파싱한 값, 전체적인 정보가 유효한지 알려주는 플래그 저장
	typedef std::map<std::string, std::string>			second_type; //헤더와 본문 내용 map 형식으로 저장(본문의 key값은 빈 문자열로)

	std::map<first_type, second_type>	rq; //각 요청 메세지를 맵 형태로 저장 - 현재 처리가 필요한 요청들만 저장해뒀다가 처리해준 뒤 삭제
public:
	typedef std::map<first_type, second_type>	value_type;
	typedef value_type::iterator				iterator;

	request(void) : rq() {}
	~request(void) {}
	iterator	rq_begin(void);
	iterator	rq_end(void);
	bool		empty(void) const;
	bool		is_invalid(iterator it);
	void		print(void);
	void		insert(int clnt_fd, std::vector<std::string> msg);
	void		erase(iterator it);
	second_type	parse(first_type &first, std::vector<std::string> msg);
	bool		check_info_invalid(first_type &first);
	bool		check_header_invalid(second_type &second);
};

#endif
