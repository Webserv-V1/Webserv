#include "./include/request.hpp"
#include "./include/connection.hpp"
#include "./include/exec_request.hpp"
#include <iostream>
#include <sys/select.h> /* According to earlier standards */ 
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h>

//void exec_request(request &rq)
//{
//	for (request::iterator it = rq.rq_begin(); it != rq.rq_end();  it++)
//	{ 
//		std::cout << "method: " << (it->first).method << std::endl;	
//	}
//}

void    exec_request(fd_set &write_fds, request &rq)
{
	while (!rq.empty())
	{
	    std::cout << "executing method(GET, POST, DELETE)" << std::endl;
	    request::iterator it = rq.rq_begin(); //rq 맨 처음에 있는 값에 대해       처리
	    if (!rq.is_invalid(it)) //일단 그 값이 유효할 때만 출력 -> 나중에         유효하지 않으면 해당되는 에러 페이지 호출하도록
	        rq.print();
	    FD_SET((it->first).fd, &write_fds); //해당 fd에 대해 출력할 수            있도록 설정
	    rq.erase(it);
	}
}
