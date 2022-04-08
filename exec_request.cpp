#include <iostream>
#include <sys/select.h> /* According to earlier standards */ 
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <exception>
#include "./include/request.hpp"
#include "./include/connection.hpp"
#include "./include/exec_request.hpp"
#include "./include/parsing.hpp"

void exec_method(config &cf, request::iterator rq_iter)
{
	std::cout << rq_iter->first.method <<  " : h9hihihi " <<std::endl;

	if(rq_iter->first.method == "GET")
	{
		std::cout << "GET" << std::endl;
	}
	else if(rq_iter->first.method == "POST")
	{
		std::cout << "POST" << std::endl;
	}
	else if(rq_iter->first.method == "DELETE")
	{
		std::cout << "DELETE" << std::endl;
	}
	else
	{
		throw 404;
	}
}

void exec_header()
{
}

void exec_body()
{
}

void    exec_request(config &cf, fd_set &write_fds, request &rq)
{
	while (!rq.empty())
	{
	    std::cout << "executing method(GET, POST, DELETE)" << std::endl;
		//=====================입력값 확인.=====================
	    request::iterator it = rq.rq_begin(); //rq 맨 처음에 있는 값에 대해       처리
	    //if (!rq.is_invalid(it)) //일단 그 값이 유효할 때만 출력 -> 나중에         유효하지 않으면 해당되는 에러 페이지 호출하도록
	    rq.print();

		try{
			exec_method(cf, it);
			exec_header();
			exec_body();
		}
		catch (int error_code)
		{
			std::cout << "error_code :  " << error_code << std::endl;
		}
		catch (std::exception &e)
		{
			throw e;
		}
		catch (...)
		{
			std::cout << "error 인데 여기는 타면안됨" << std::endl;
			throw "에러 ... 으로 처리..";
		}
	    FD_SET((it->first).fd, &write_fds); //해당 fd에 대해 출력할 수            있도록 설정
	    rq.erase(it);
	}
}
