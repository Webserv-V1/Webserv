#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <fcntl.h>
# include <cstring>
# include <string>
# include <vector>
# include <exception>
# include "parsing.hpp"
# include "request.hpp"
# include "error.hpp"
# include "../default_conf.hpp"

//읽고 쓸 때 체크해야 하는 fd 배열의 집합 - select에서 사용하나 서버 소켓에 대해 해당 read_fd를 활성화해줘야 하기 때문에 connection에서 선언
class IO_fd_set
{
public:
	fd_set	read_fds;
	fd_set	cpy_read_fds;
	fd_set	write_fds;
	fd_set	cpy_write_fds;

	IO_fd_set(void)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		FD_ZERO(&cpy_read_fds);
		FD_ZERO(&cpy_write_fds);
	}
};

class fd_info
{
public:
	int							fd; //현재 파일디스크립터 (서버 fd라도 본인 info 보유)
	server						*server_info; //해당 fd와 연결된 서버 클래스 포인터
	std::string					tmp; //transfer-encoding 때 파싱한 문자열을 저장할 임시 문자열
	std::string					msg; //이 fd가 클라이언트라면 입력받은 문자열을 받아서 나중에 \n 기준으로 split_msg에 추가
	std::vector<std::string>	split_msg;
	int							status;
	int							cn_num;

	fd_info(int f, server *s) : fd(f), server_info(s), tmp(""), msg(""), split_msg(), status(RQ_LINE_NOT_PARSED), cn_num(0) {}
};

class connection
{
private:
	int							server_size; //fd_arr에서 서버, 클라이언트 구별 위함
	std::vector<fd_info>		fd_arr; //서버와 클라이언트 fd 모두 여기에 저장 - 나중에 입출력 검사 위해
	fd_set						&read_fds;

public:
	typedef std::vector<fd_info>::iterator	iterator;

	connection(config &cf, fd_set &rfds);
	~connection(void);
	iterator					fd_arr_begin(void);
	iterator					fd_arr_end(void);
	bool						is_server_socket(int fd);
	int							max_fd(void);
	fd_info						&info_of_fd(int fd);
	void						connect_client(int serv_sock);
	void						disconnect_client(int clnt_sock);
	void						check_connection(int clnt_sock, std::string rp);
	void						get_client_msg(int clnt_sock, request &rq);
	void						clear_client_msg(int clnt_sock);
	void						concatenate_client_msg(fd_info &clnt_info, std::string to_append);
	bool						is_before_body_input(fd_info &clnt_info);
	bool						is_input_completed(fd_info &clnt_info, request &rq);
	bool						completed_input(request &rq, request::iterator &it, std::string body, int err_no);
	request::iterator			parse_rq_line(fd_info &clnt_info, request &rq);
	request::iterator			parse_client_header_by_line(fd_info &clnt_info, request &rq);
	void						is_body_exist(fd_info &clnt_info, request &rq, request::iterator &it);
	bool						is_transfer_encoding_completed(fd_info &clnt_info, request &rq);
	bool						is_content_length_completed(fd_info &clnt_info, request &rq);
	int							convert_to_num(std::string str_length, int radix);
	std::string					trim_last_cr(std::string msg);
	std::string					trim_last_crlf(std::string msg, int required_length);
	void						print_client_msg(int clnt_sock);
};
#endif
