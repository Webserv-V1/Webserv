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
# include "request.hpp"

# define	BUF_SIZE 100

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
	int							fd; //현재 파일디스크립터
	int							server_sock; //이 fd가 클라이언트라면 연결된 서버fd를 의미, 서버라면 -1 -> 나중에 연결된 서버 페이지를 보여준다던가 할 때 사용하려고 일단 fd를 넣고 해당 config 내용이 필요하다면 나중에 수정
	std::string					msg; //이 fd가 클라이언트라면 입력받은 문자열을 받아서 나중에 \n 기준으로 split_msg에 추가
	std::vector<std::string>	split_msg;

	fd_info(int f) : fd(f), server_sock(-1), msg(""), split_msg() {}
	fd_info(int f, int s) : fd(f), server_sock(s), msg(""), split_msg() {}
};

class connection
{
private:
	int							server_size; //fd_arr에서 서버, 클라이언트 구별 위함
	std::vector<fd_info>		fd_arr; //서버와 클라이언트 fd 모두 여기에 저장 - 나중에 입출력 검사 위해
	fd_set						&read_fds;

	void						concatenate_client_msg(fd_info &clnt_info, std::string to_append);
	void						parse_client_msg_by_line(fd_info &clnt_info);

public:
	typedef std::vector<fd_info>::iterator	iterator;

	connection(fd_set &rfds);
	~connection(void);
	iterator					fd_arr_begin(void);
	iterator					fd_arr_end(void);
	bool						is_server_socket(int fd);
	int							max_fd(void);
	fd_info						&info_of_fd(int clnt_sock);
	void						connect_client(int serv_sock);
	void						disconnect_client(int clnt_sock);
	void						get_client_msg(int clnt_sock, request &rq);
	void						print_client_msg(int clnt_sock);

	class	socket_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("Unexpected error while executing 'socket' function");
		}
	};
	class	bind_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("Unexpected error while executing 'bind' function");
		}
	};
	class	listen_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("Unexpected error while executing 'listen' function");
		}
	};
	class	accept_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("Unexpected error while executing 'accept' function");
		}
	};
	class	recv_error : public std::exception
	{
		virtual const char	*what(void) const throw()
		{
			return ("Unexpected error while executing 'recv' function");
		}
	};
};
#endif
