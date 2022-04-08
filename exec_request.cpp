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
#include "./include/error.hpp"
void make_map_mime_types (std::map<std::string, std::string > &m_mt, std::string readLine)
{
	int flag = 0;
	std::string key;
	for(int i = 0; i < readLine.size(); i++)
	{
		if(isalpha(readLine[i]))
		{
			int j;
			std::string s_tmp = "";
			for(j = i; j < readLine.size(); j++)
			{
				if(readLine[j] == ' ' || readLine[j] == '	' || readLine[j] == ';')
					break;
				s_tmp += readLine[j];
			}
			if(flag == 0)
			{
				key = s_tmp;
				flag++;
			}
			else
				m_mt[s_tmp] = key;
			i = j;
		}
	}
}

void set(std::map<int, std::string> &m_state_code, std::map<std::string, std::string > &m_mt )
{
	m_state_code[100] = "Continue";
	m_state_code[101] = "Switching Protocols";
	m_state_code[200] = "OK";
	m_state_code[201] = "Created";
	m_state_code[202] = "Accepted";
	m_state_code[203] = "Non-Authoritative Information";
	m_state_code[204] = "No Content";
	m_state_code[205] = "Reset Content";
	m_state_code[206] = "Partial Content";
	m_state_code[300] = "Multiple Choices";
	m_state_code[301] = "Moved Permanently";
	m_state_code[302] = "Found";
	m_state_code[303] = "See Other";
	m_state_code[304] = "Not Modified";
	m_state_code[305] = "Use Proxy";
	m_state_code[307] = "Temporary Redirect";
	m_state_code[400] = "Bad Request";
	m_state_code[401] = "Unauthorized";
	m_state_code[402] = "Payment Required";
	m_state_code[403] = "Forbidden";
	m_state_code[404] = "Not Found";
	m_state_code[405] = "Method Not Allowed";
	m_state_code[406] = "Not Acceptable";
	m_state_code[407] = "Proxy Authentication Required";
	m_state_code[408] = "Request Timeout";
	m_state_code[409] = "Conflict";
	m_state_code[410] = "Gone";
	m_state_code[411] = "Length Required";
	m_state_code[412] = "Precondition Failed";
	m_state_code[413] = "Payload Too Large";
	m_state_code[414] = "URI Too Long";
	m_state_code[415] = "Unsupported Media Type";
	m_state_code[416] = "Range Not Satisfiable";
	m_state_code[417] = "Expectation Failed";
	m_state_code[426] = "Upgrade Required";
	m_state_code[500] = "Internal Server Error";
	m_state_code[501] = "Not Implemented";
	m_state_code[502] = "Bad Gateway";
	m_state_code[503] = "Service Unavailable";
	m_state_code[504] = "Gateway Timeout";
	m_state_code[505] = "HTTP Version Not Supported";

	std::string readLine ="";
	std::ifstream readFile;

	readFile.open("./mime.types.default");
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
			make_map_mime_types (m_mt, readLine);
		readFile.close();
	}
	else{
		throw open_fail();
	}
}
void exec_method(config &cf, request::iterator rq_iter)
{
	if(rq_iter->first.version != "HTTP/1.1")
	{
		throw 400;
	}
	if(rq_iter->first.method == "GET")
	{
		//std::cout << "GET" << std::endl;
	}
	else if(rq_iter->first.method == "POST")
	{
		//std::cout << "POST" << std::endl;
	}
	else if(rq_iter->first.method == "DELETE")
	{
		//std::cout << "DELETE" << std::endl;
	}
	else
	{
		throw 400;
	}
}

class conf_index {
	public : 
		int server;
		int location;

		std::string Connection;
		std::string file_type;
		std::string location_path;
		std::string root_path;
		std::string file_path;

	conf_index() {
		server = -1;
		location = -1;

		root_path = "";
		location_path = "";
		file_type = "text/html";
		Connection = "keep-alive";
		file_path = "";
	}
};

void find_cf_location_i(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
	if(0 == cf.v_s[cf_i.server].location_path.size())
		throw config_error_location();
	for(int i = 0; i < cf.v_s[cf_i.server].location_path.size(); i++)
	{
		if(cf.v_s[cf_i.server].location_path[i] == fd_data.first.url)
		{
			cf_i.location = i;
			cf_i.location_path = cf.v_s[cf_i.server].location_path[i];
			break;
		}
		if(i + 1 == cf.v_s[cf_i.server].location_path.size())
		{
			//std::cout << "에러처리찾기 :  error 404 loaction  못찾음 : " << fd_data.first.url<< std::endl;
			// /favicon.ico 이것도 처리해줘야함...
			throw 404;
		}
	}
}

void find_cf_server_i(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
	// conf 파일에 server가 없을떄
	if(cf.v_s.size() == 0)
		throw config_error_server();
	for(int i = 0; i < cf.v_s.size(); i++)
	{
		if(cf.v_s[i].listen == fd_data.second["host"])
		{
			cf_i.server = i;
			break;
		}
		if(i + 1 == cf.v_s.size())
		{
			std::cout << "에러처리찾기 : error server가 없으므로 에러 뛰워야함." << std::endl;
			throw 404;
		}
	}
}
void when_host_is_localhost(request::value_type &fd_data)
{
	if(fd_data.second["host"].substr(0, 9) == "localhost")
	{
		if(fd_data.second["host"][9] == ':')
			fd_data.second["host"] = (std::string)"127.0.0.1" + ":" + fd_data.second["host"].substr(10);
		else
			fd_data.second["host"] = (std::string)"127.0.0.1" + ":80";
	}
}


void confirmed_root_path(config &cf, conf_index &cf_i)
{
	if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("root") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
	{
		if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"].size() != 1)
			throw config_error_root();
		else
			cf_i.root_path = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"][0];
	}
	else
	{
		//설명 : root 없을떄 기본값.
		cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"].push_back("./html_file");
		cf_i.root_path = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"][0];
	}
}


void exec_header(config &cf, request::value_type &fd_data, conf_index &cf_i, std::map<std::string, std::string> &m_mt)
{
	if(fd_data.second.find("host") !=  fd_data.second.end())
	{
		when_host_is_localhost(fd_data);
		find_cf_server_i(cf, fd_data, cf_i);
	}
	else throw 400;
	if(fd_data.second.find("connection") !=  fd_data.second.end())
	{
		//keep-alive면 그냥 접속 지금처럼 지속해주면됨.
		if(fd_data.second["connection"] != "keep-alive" && fd_data.second["connection"] != "close")
			throw 400;
		if(fd_data.second["connection"] == "close")
			cf_i.Connection = "close";
			//std::cout << "에러처리찾기 : keep-alive아닐때 처리. close같은 옵션도 있던데 해야햐ㅏㄹ까?" << std::endl;
	}
	find_cf_location_i(cf, fd_data, cf_i);
	confirmed_root_path(cf, cf_i);

	if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("index") == cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
		cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].push_back("index.html");
	for(int cnt = 0; cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].size(); cnt++)
	{
		cf_i.file_path = cf_i.root_path + cf_i.location_path + "/" +  cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt];
		if(access(cf_i.file_path.c_str(),F_OK) == 0)
		{
			//std::cout << "파일이 존재 합니다." << std::endl;
			for(int str_i = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt].size() - 1; str_i > 0; str_i--)
			{
				if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt][str_i] == '.')
				{
					if(m_mt.find(cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt].substr(str_i + 1)) != m_mt.end())
					{
						cf_i.file_type = m_mt[cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt].substr(str_i + 1)];
						break;
					}
					else
						throw 400;
				}
			}
			break;
		}
		if (cnt + 1 == cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].size())
			throw 404;
	}
}

void exec_body()
{
}

class response_data{

};

void    exec_request(config &cf, fd_set &write_fds, request *rq, std::string &request_msg)
{
	request_msg = "";
	conf_index	cf_i;
//	int ii = 0;
//	for (request::iterator it = rq->rq_begin(); it < rq->rq_end(); it++)
//	{
//		ii++;
//		std::cout << "================================ ii = " << ii << std::endl;
//	}
//
	while (!rq->empty())
	{
	    std::cout << "executing method(GET, POST, DELETE)" << std::endl;
		std::map<int, std::string> m_state_code;
		std::map<std::string, std::string > m_mt;
		set(m_state_code, m_mt);
		
		//=====================입력값 확인.=====================
	    request::iterator it = rq->rq_begin(); //rq 맨 처음에 있는 값에 대해       처리
	    if (!rq->is_invalid(it)) //일단 그 값이 유효할 때만 출력 -> 나중에         유효하지 않으면 해당되는 에러 페이지 호출하도록
	        rq->print();
		try{
			exec_method(cf, it);
			exec_header(cf, *it, cf_i, m_mt);
			exec_body();
			throw 200;
		}
		catch (int error_code)
		{
			//HTTP/1.1 200 OK
			//std::cout << "============== error_code :  " << error_code << std::endl;
			request_msg = (std::string)"HTTP/1.1 " + std::to_string(error_code) + " " + m_state_code[error_code] + "\r\n";

			std::ifstream readFile;
			if(200 <= error_code && 400 > error_code)
			{
				request_msg += "Connection: " + cf_i.Connection + "\r\n";
				readFile.open(cf_i.file_path);
			}
			else
			{
				request_msg += "Connection: keep-alive\r\n";
				readFile.open((std::string)"./html_file/" + std::to_string(error_code) + ".html");
			}

			std::string readLine ="";
			std::string file_data = "";
			if (readFile.is_open())
			{
				while (getline(readFile, readLine))
					file_data += readLine + "\n";
				readFile.close();
			}
			else
			{
				std::cout << "구린내가 난다.! " << std::endl;
				throw open_fail();
			}
			if(200 <= error_code && 400 > error_code)
			{
				request_msg += "Content-Length: " + std::to_string(file_data.size()) + "\r\n";
				request_msg += "Content-type: " + cf_i.file_type + "\r\n";
			}
			else
			{
				request_msg += "Content-Length: " + std::to_string(file_data.size()) + "\r\n";
				request_msg += "Content-type: text/html\r\n";
			}

			request_msg += "\r\n";
			request_msg += file_data;
			std::cout << request_msg << std::endl;
			std::cout << std::endl;
			std::cout << std::endl;
		//	std::cout << " =====  rkwmdk!!!!!! ======\n" << cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"][0] + cf.v_s[cf_i.server].location_path[cf_i.location] + cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][0] << "\n" << std::endl;
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
			throw e;
		}
		catch (...)
		{
			std::cout << "error 인데 여기는 타면안됨" << std::endl;
			throw "에러 ... 으로 처리..";
		}
	    FD_SET((it->first).fd, &write_fds); //해당 fd에 대해 출력할 수 있도록 설정
		rq->erase(it);
	}
}
