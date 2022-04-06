#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <vector>
# include <map>

# define	TMP_RQ	0
# define	RQ		1

class base_request
{
public:
	int			fd; 
	std::string	method;
	std::string	url;
	std::string	version;
	bool		is_invalid;
	int			err_no;

	base_request(int fd_) : fd(fd_), method(""), url(""), version(""), is_invalid(false), err_no(-1) {}

	bool	operator<(const base_request &base) const
	{
		return (fd < base.fd);
	}
};

class request
{
public:
	typedef base_request						first_type; //클라이언트 fd, 첫 번쨰 줄에 오는 정보 파싱한 값, 전체적인 정보가 유효한지 알려주는 플래그 저장
	typedef std::map<std::string, std::string>	second_type; //헤더와 본문 내용 map 형식으로 저장(본문의 key값은 빈 문자열로)
	typedef std::pair<first_type, second_type>	value_type;
	typedef std::vector<value_type>				value_arr;
	typedef value_arr::iterator					iterator;
private:
	value_arr	tmp_rq;
	value_arr	rq; //각 요청 메세지를 맵 형태로 저장 - 현재 처리가 필요한 요청들만 저장해뒀다가 처리해준 뒤 삭제
public:
	request(void) : tmp_rq(), rq() {}
	~request(void) {}
	iterator	rq_begin(void);
	iterator	rq_end(void);
	bool		empty(void) const;
	bool		is_invalid(iterator it);
	void		print(void);
	void		print_tmp(void);
	iterator	insert_header(int clnt_fd, std::vector<std::string> msg_header);
	iterator	insert(iterator &it, std::string msg_body);
	void		erase(iterator it);
	second_type	parse_header(first_type &first, std::vector<std::string> msg);
	iterator	find_clnt(int clnt_fd);
	iterator	find_clnt_in_tmp(int clnt_fd);
	bool		check_info_invalid(first_type &first);
	bool		check_header_invalid(second_type &second);
	bool		which_method(iterator &it, std::string method);
	bool		is_existing_header(iterator &it, std::string header);
	std::string	corresponding_header_value(iterator &it, std::string header);
	bool		set_error(iterator &it, int err_no);

	class	invalid_header_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("Invalid Header!!");
		}
	};
	class	incorrect_body_length_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("Incorrect Body Length!!");
		}
	};
};

#endif
