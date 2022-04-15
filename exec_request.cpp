#include <iostream>
#include <exception>
#include <iostream>
#include <string>
#include <algorithm>    // std::replace
#include "./include/request.hpp"
#include "./include/connection.hpp"
#include "./include/exec_request.hpp"
#include "./include/parsing.hpp"
#include "./include/error.hpp"

#include <sys/select.h> /* According to earlier standards */ 
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <io.h>

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




class conf_index {
	public : 
		int server;
		int location;
		bool autoindex_flag;

		std::string Connection;
		std::string file_type;
		std::string location_path;
		std::string root_path;
		std::string file_path;
		std::string redirect_path;
		std::string request_url;
		std::string request_url_remaining;
		std::map <std::string, std::string> m_qurry_string;

	conf_index() {
		server = -1;
		location = -1;
		autoindex_flag = false;

		root_path = "";
		location_path = "";
		file_type = "text/html";
		Connection = "keep-alive";
		file_path = "";
		redirect_path = "";
		request_url = "";
		request_url_remaining = "";
	}
};

void exec_method(config &cf, request::iterator rq_iter, conf_index &cf_i)
{
	int flag = 0;
	if(rq_iter->first.version != "HTTP/1.1")
		throw 400;
    if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("allow_methods") == cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
		flag = 1;

	if(flag == 1 || find (cf.v_s[cf_i.server].v_l[cf_i.location].m_location["allow_methods"].begin(), cf.v_s[cf_i.server].v_l[cf_i.location].m_location["allow_methods"].end(), rq_iter->first.method) != cf.v_s[cf_i.server].v_l[cf_i.location].m_location["allow_methods"].end())
	{
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
	else
	{
		std::cout << "이거 타는거 같은데? " <<std::endl;
		throw 403;
	}
}


void	parsing_url(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
//	std::cout << "url :test  ==== " <<  fd_data.first.url << std::endl;
	std::string tmp = "";
	std::string key_tmp = "";
	int save_flag = 0;
	for(int i = 0; i < fd_data.first.url.size(); i++)
	{
		tmp +=  fd_data.first.url[i];
		if((i + 1 == fd_data.first.url.size() && save_flag == 0) || (fd_data.first.url[i + 1] == '?' && save_flag == 0))
		{
			cf_i.request_url = tmp;
			save_flag = 1;
			tmp = "";
			i = i + 1;
		}
		else if(save_flag == 1 && (i + 1 == fd_data.first.url.size() || fd_data.first.url[i + 1] == '=') )
		{
			save_flag = 2;
			key_tmp = tmp;
			tmp = "";
			i = i + 1;
		}
		else if(save_flag == 2  && (i + 1 == fd_data.first.url.size() || (i + 2 < fd_data.first.url.size() && (fd_data.first.url[i + 1] == '&'  && fd_data.first.url[i + 2] == '&'))))
		{
			save_flag = 1;
			cf_i.m_qurry_string[key_tmp] = tmp;
			key_tmp = "";
			tmp = "";
			i = i + 2;
		}
	}

// qurrystring
//	std::cout << " ======-====== " << std::endl;
//	std::cout << " ======-====== " << std::endl;
//	std::cout << " cf_i.request_url : "  << cf_i.request_url << std::endl;
//	for(std::map<std::string, std::string>::iterator it = cf_i.m_qurry_string.begin();  it != cf_i.m_qurry_string.end(); it++)
//		std::cout <<"first : "<<  it->first << " second : " << it->second <<std::endl;
//	std::cout << " ======-====== " << std::endl;
//	std::cout << " ======-====== " << std::endl;


}

int my_compare_string(std::string tmp1, std::string tmp2)
{
	//tmp1만큼만 비교,
	if(tmp1.size() > tmp2.size())
		return 0;
	for(int i = 0; i < tmp1.size(); i++)
	{
		if(tmp1[i] != tmp2[i])
			return 0;
	}
	return 1;
}

void find_cf_location_i(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
	int same_value = 0;
	parsing_url(cf, fd_data,cf_i);

	for(int i = 0; i < cf.v_s[cf_i.server].location_path.size(); i++)
	{
//		std::cout <<  cf.v_s[cf_i.server].location_path[i] << "  ::::  " << cf_i.request_url  << " size : " <<cf.v_s[cf_i.server].location_path[i].size() << " compare : " << cf.v_s[cf_i.server].location_path[i].compare(0,  cf.v_s[cf_i.server].location_path[i].size(), cf_i.request_url) << "    :" << cf.v_s[cf_i.server].location_path[i][0] << ":    :"<<cf_i.request_url[0] << ":" <<  std::endl;
		if(my_compare_string(cf.v_s[cf_i.server].location_path[i], cf_i.request_url) == 1)
		{
			if(cf.v_s[cf_i.server].location_path[i].size() == 1 || (cf_i.request_url.size() == cf.v_s[cf_i.server].location_path[i].size()) || cf_i.request_url[cf.v_s[cf_i.server].location_path[i].size()] == '/')
			{
				if(same_value < cf.v_s[cf_i.server].location_path[i].size())
				{
					same_value = cf.v_s[cf_i.server].location_path[i].size();
					cf_i.location = i;
					cf_i.location_path = cf.v_s[cf_i.server].location_path[i];
				}
			}
		}
		if(i + 1 == cf.v_s[cf_i.server].location_path.size())
		{
			if (same_value != 0)
				break;
			//std::cout << "에러처리찾기 :  error 404 loaction  못찾음 : " << fd_data.first.url<< std::endl;
			// /favicon.ico 이것도 처리해줘야함...
			throw 404;
		}
	}
	if(cf_i.location_path.size() < cf_i.request_url.size())
		cf_i.request_url_remaining = cf_i.request_url.substr(cf_i.location_path.size());
}

void find_cf_server_i(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
	for(int i = 0; i < cf.v_s.size(); i++)
	{
		if(cf.v_s[i].listen == fd_data.second["host"])
		{
			cf_i.server = i;
			break;
		}
		if(i + 1 == cf.v_s.size())
		{
			cf_i.server = 0;
			std::cout << "cf.v_s[i].listen : " << cf.v_s[i].listen <<std::endl;
			std::cout << "fd_data.second[\"Host\"]" << fd_data.second["Host"] << std::endl;
			std::cout << "에러처리찾기 : error server가 없으므로 에러 뛰워야함." << std::endl;
			break;
			// 일치하는 Host없으면 첫번째 server가 디폴트 server임.
			//throw 404;
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

std::string confirmed_root_path(config &cf, conf_index &cf_i)
{
	if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("root") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
	{
			cf_i.root_path = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"][0];
	}
	else
	{
		//설명 : root 없을떄 기본값.
		cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"].push_back("./html_file");
		cf_i.root_path = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"][0];
	}
	std::string path = cf_i.root_path + cf_i.request_url_remaining;
	struct stat s;
	if( stat(path.c_str(),&s) == 0 )
	{
		if( s.st_mode & S_IFREG )
		{
			cf_i.file_path = path;
			return "location is file";
		}
		else //( s.st_mode & S_IFDIR )
			return "location is folder";
	}

	else
		throw 404;

//	DIR * dir = NULL;
//	std::string path = cf_i.root_path + cf_i.request_url_remaining;
//	std::cout << "path : " << path << std::endl;
//	std::string path_list = "";
//	if( (dir = opendir(path.c_str())) == NULL)
//	{
//		std::cout << "root, location 경로 잘못됨.!!! " <<  std::endl;
//		throw 404;
//	}
//	closedir(dir);

//	char* filename = "COOL"; // 여기에 파일이나 폴더명 입력

}

int check_filesize(std::ifstream &readFile, conf_index &cf_i, config &cf)
{
	std::streampos begin, end;
	begin = readFile.tellg();
	readFile.seekg (0, std::ios::end);
	end = readFile.tellg();
	readFile.close();
	if(cf_i.location != -1 && cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("client_max_body_size") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
	{
		if(stol(cf.v_s[cf_i.server].v_l[cf_i.location].m_location["client_max_body_size"][0]) * 1048576 < end - begin)
		return 413;
	}		
	//std::cout << "###########size is: " << (end-begin) << " bytes.\n";
	return 0;
}

void confirmed_file_path_and_file_type(config &cf, conf_index &cf_i, std::map<std::string, std::string> &m_mt)
{
	if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("index") == cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
		cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].push_back("index.html");
	//std::cout << "server : " << cf_i.server << "   location : " << cf_i.location << std::endl;
	for(int cnt = 0; cnt < cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].size(); cnt++)
	{
		cf_i.file_path = cf_i.root_path + cf_i.request_url_remaining + "/" +  cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt];
		std::ifstream readFile( cf_i.file_path.c_str() );
		if (readFile.is_open())
		{
			if(413 == check_filesize(readFile, cf_i, cf))
				throw 413;
			//readFile.close()
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
		{
			if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("auto_index") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end() && cf.v_s[cf_i.server].v_l[cf_i.location].m_location["auto_index"][0] == "on")
				cf_i.autoindex_flag = 1;
			else throw 404;
		}
	}
}

void exec_redirection(config &cf, conf_index &cf_i)
{

	if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("return") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
	{
		cf_i.redirect_path = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["return"][1];
		throw stoi(cf.v_s[cf_i.server].v_l[cf_i.location].m_location["return"][0]);
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
	exec_redirection(cf ,cf_i);
	if(confirmed_root_path(cf, cf_i) == "location is folder")
		confirmed_file_path_and_file_type(cf, cf_i, m_mt);
}

void exec_body()
{
}

class response_data{

};

void make_request(int &error_code, std::string &request_msg, conf_index &cf_i)
{
	std::ifstream readFile;

	if(200 <= error_code && 400 > error_code)
	{
		request_msg += "Connection: " + cf_i.Connection + "\r\n";
		if(cf_i.autoindex_flag == 1)
			readFile.open("./html_file/autoindex.html");
		else
			readFile.open(cf_i.file_path);
	}
	else if(error_code == -900) //error_page
	{
		request_msg += "Connection: keep-alive\r\n";
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
		throw open_fail();
	if(200 <= error_code && 400 > error_code)
	{
        if(cf_i.autoindex_flag == 1) // cf_i.root_path + cf_i.request_url_remaining 이 폴더 일때만 탐.. 
        {
			DIR * dir = NULL;
			struct dirent * entry;
			struct stat buf;
			std::string path = cf_i.root_path + cf_i.request_url_remaining;
			std::cout << "path : " << path << std::endl;
			std::string path_list = "";
			if( (dir = opendir(path.c_str())) == NULL)
				throw root_and_location_error();
			while( (entry = readdir(dir)) != NULL)
			{
				std::string s_tmp (entry->d_name);
				std::string tmp_d_name (entry->d_name);
				s_tmp = path + "/" + s_tmp;
				stat(s_tmp.c_str(), &buf);
				if(S_ISREG(buf.st_mode))
				{
					//	printf("FILE    ");
				}
				else if(S_ISDIR(buf.st_mode))
				{
					tmp_d_name += "/";
					//printf("DIR     ");
				}
				else
					std::cout << "???  이거 파일도 아니고, 폴더도 아닌게 있어?   " << std::endl;
				path_list += "<a href=\"";
				path_list += tmp_d_name;
				path_list += "\">" + tmp_d_name + "</a>";
				for(int zz = tmp_d_name.size(); zz <= 40; zz++)
					path_list += " ";
	//			printf("%s      ", entry->d_name);
	//			printf("Last status change: %s", ctime(&buf.st_ctime)); 
	//			printf("Last file access: %s", ctime(&buf.st_atime));

				std::string b_size_tmp;
				if(S_ISREG(buf.st_mode))
					b_size_tmp = std::to_string(buf.st_size);
				else b_size_tmp = "-";
				path_list += b_size_tmp;
				for(int zz = b_size_tmp.size(); zz <= 20; zz++)
					path_list += " " ;
				path_list += std::string(ctime(&buf.st_mtime));
			}
			closedir(dir);
			std::cout << std::endl;
			std::string find_str1 = "$1";
			std::string find_str2 = "$2";
			file_data.replace(file_data.find(find_str1), find_str1.size(), cf_i.root_path + cf_i.request_url_remaining);
			file_data.replace(file_data.find(find_str1), find_str1.size(), cf_i.root_path + cf_i.request_url_remaining);
			file_data.replace(file_data.find(find_str2), find_str2.size(), path_list);
		}
		request_msg += "Content-Length: " + std::to_string(file_data.size()) + "\r\n";
		request_msg += "Content-type: " + cf_i.file_type + "\r\n";
	}
	else
	{
		//error_page는 html파일만 보냄..
		request_msg += "Content-Length: " + std::to_string(file_data.size()) + "\r\n";
		request_msg += "Content-type: text/html\r\n";
	}
	request_msg += "\r\n";
	request_msg += file_data;
}

void exec_error_page(conf_index &cf_i, config &cf, int &error_code, std::vector<std::string> &v_error_page, std::string &request_msg, std::map<int, std::string> &m_state_code)
{
	std::ifstream readFile;

	for(int i = 0; i < v_error_page.size() - 1; i++)
	{
		if(stoi(v_error_page[i]) == error_code)
		{
			readFile.open(cf_i.root_path + cf_i.request_url_remaining + v_error_page[v_error_page.size() - 1]);
			//std::cout << cf_i.root_path + cf_i.request_url_remaining + v_error_page[v_error_page.size() - 1]<< std::endl;
			if (readFile.is_open())
			{
				if(413 == check_filesize(readFile, cf_i, cf))
				{
					error_code = 413;
					request_msg = (std::string)"HTTP/1.1 " + std::to_string(error_code) + " " + m_state_code[error_code] + "\r\n";
				}
				else
				{
					error_code = -900;
					cf_i.file_path = cf_i.root_path + cf_i.request_url_remaining + v_error_page[v_error_page.size() - 1];
				}
				readFile.close();
			}
			else ; //error_page파일 안열리면 그냥 에러코드를 출력하면 됨.
			break;
		}
	}
}



void error_page(int &error_code, conf_index &cf_i, config &cf, std::string &request_msg, std::map<int, std::string> &m_state_code)
		
{
	if (cf_i.root_path == "")
		cf_i.root_path = "./html_file";
	if (cf_i.server != -1 && cf_i.location != -1 && cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("error_page") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
	{
		exec_error_page(cf_i, cf, error_code, cf.v_s[cf_i.server].v_l[cf_i.location].m_location["error_page"], request_msg, m_state_code);	
	}
	else if (cf_i.server != -1)
	{
		exec_error_page(cf_i, cf, error_code, cf.v_s[cf_i.server].v_error_page[0].second, request_msg, m_state_code);	
	}

}
void make_request_redirect(std::string &request_msg, conf_index &cf_i)
{
	request_msg += (std::string)"Content-Type: text/html" + "\r\n";
	request_msg += (std::string)"Connection: keep-alive" + "\r\n";
	request_msg += (std::string)"Location: " + cf_i.redirect_path + "\r\n" + "\r\n";
}

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
      			exec_header(cf, *it, cf_i, m_mt);
			exec_method(cf, it, cf_i);
			exec_body();
			throw 200;
		}
		catch (int error_code)
		{
			//std::cout << "rrbbrbr " << std::endl;
			request_msg = (std::string)"HTTP/1.1 " + std::to_string(error_code) + " " + m_state_code[error_code] + "\r\n";
			if(cf_i.redirect_path != "")
				make_request_redirect(request_msg, cf_i);
			else
			{
				if(cf_i.autoindex_flag != 1)
					error_page(error_code, cf_i, cf, request_msg, m_state_code);
				make_request(error_code, request_msg, cf_i);
			}
			std::cout << request_msg << std::endl;
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
