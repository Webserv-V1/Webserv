#include <iostream>
#include <exception>
#include <iostream>
#include <string>
#include <algorithm>    // std::replace
#include <sstream>
#include "./include/request.hpp"
#include "./include/connection.hpp"
#include "./include/exec_request.hpp"
#include "./include/parsing.hpp"
#include "./include/error.hpp"
#include "./default_conf.hpp"
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
#include "./include/exec_request.hpp"

void make_map_mime_types (std::map<std::string, std::string > &m_mt, std::string readLine)
{
	int flag = 0;
	std::string key;
	for(int i = 0; i < (int)readLine.size(); i++)
	{
		if(isalpha(readLine[i]))
		{
			int j;
			std::string s_tmp = "";
			for(j = i; j < (int)readLine.size(); j++)
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

std::string make_GMT_time()
{
	time_t rawtime;
	struct tm* gm_timeinfo;

	time(&rawtime);
	gm_timeinfo = gmtime(&rawtime);
	std::string tmp (asctime(gm_timeinfo));
	std::istringstream iss(tmp);
	std::string buffer;
	std::vector<std::string> v_time_tmp;
    while (getline(iss, buffer, ' '))
        v_time_tmp.push_back(buffer);
	return (v_time_tmp[0] + ", " + v_time_tmp[2] + " " + v_time_tmp[1] + " " + v_time_tmp[4].substr(0, 4) + " " + v_time_tmp[3] + + " " +"GMT");
}

void exec_method(config &cf, request::iterator rq_iter, conf_index &cf_i)
{
	int flag = 0;

	if(rq_iter->first.version != "HTTP/1.1")
		throw 400; //확인 : 요청헤더 잘못들어옴. 
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
			int flag = 0;
			for(int str_i = cf_i.file_path.size() - 1; str_i > 0; str_i--)
				if(cf_i.file_path[str_i] == '.')
					if(cf_i.file_path.substr(str_i + 1) == "php")
					{
						flag = 1;
						break;
					}
			if(flag == 0)
				throw 405;  //확인 : post는 동적파일만 허용한다. 해당 웹서버에서 동적파일은 현재 php뿐임.
		}
		else if(rq_iter->first.method == "DELETE")
		{
		}
		else
		{
			throw 400;
		}
	}
	else
	{
		throw 403; //에메함 : conf에 접근 권한 없는 요청이라 403 뛰움. 클라이언트는 콘텐츠에 접근할 권리를 가지고 있지 않습니다.
	}
}


void	parsing_url(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
	if(fd_data.first.url.size() == 0)
		throw 400; //확인 : url이 아예없으면 잘못된 요청!
	for(int i = 0; i < fd_data.first.url.size(); i++)
	{
		if(fd_data.first.url[i] == '?')
		{
			cf_i.request_url = fd_data.first.url.substr(0, i);
			cf_i.qurry_string = fd_data.first.url.substr(i + 1);
			break;
		}
		if(i + 1 == fd_data.first.url.size())
		{
			cf_i.request_url = fd_data.first.url;
		}
	}
// 잔해... qurrystring 짤랐던거..
//	std::cout << "url :test  ==== " <<  fd_data.first.url << std::endl;
//	std::string tmp = "";
//	std::string key_tmp = "";
//	int save_flag = 0;
//	std::cout <<" 11 : " << cf_i.request_url << std::endl;
//	std::cout <<" 22 : " << cf_i.qurry_string << std::endl;
//	for(int i = 0; i < fd_data.first.url.size(); i++)
//	{
//		tmp +=  fd_data.first.url[i];
//		if((i + 1 == fd_data.first.url.size() && save_flag == 0) || (fd_data.first.url[i + 1] == '?' && save_flag == 0))
//		{
//			cf_i.request_url = tmp;
//			save_flag = 1;
//			tmp = "";
//			i = i + 1;
//		}
//		else if(save_flag == 1 && (i + 1 == fd_data.first.url.size() || fd_data.first.url[i + 1] == '=') )
//		{
//			save_flag = 2;
//			key_tmp = tmp;
//			tmp = "";
//			i = i + 1;
//		}
//		else if(save_flag == 2  && (i + 1 == fd_data.first.url.size() || (i + 2 < fd_data.first.url.size() && (fd_data.first.url[i + 1] == '&'  && fd_data.first.url[i + 2] == '&'))))
//		{
//			save_flag = 1;
//			cf_i.m_qurry_string[key_tmp] = tmp;
//			key_tmp = "";
//			tmp = "";
//			i = i + 2;
//		}
//	}

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
	for(int i = 0; i < (int)tmp1.size(); i++)
	{
		if(tmp1[i] != tmp2[i])
			return 0;
	}
	return 1;
}

void find_cf_location_i(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
	int same_value = 0;

	for(int i = 0; i < (int)cf.v_s[cf_i.server].location_path.size(); i++)
	parsing_url(cf, fd_data,cf_i);
	for(int i = 0; i < cf.v_s[cf_i.server].location_path.size(); i++)
	{
		if(my_compare_string(cf.v_s[cf_i.server].location_path[i], cf_i.request_url) == 1)
		{
			if(cf.v_s[cf_i.server].location_path[i].size() == 1 || (cf_i.request_url.size() == cf.v_s[cf_i.server].location_path[i].size()) || cf_i.request_url[cf.v_s[cf_i.server].location_path[i].size()] == '/')
			{
				if(same_value < (int)cf.v_s[cf_i.server].location_path[i].size())
				{
					same_value = cf.v_s[cf_i.server].location_path[i].size();
					cf_i.location = i;
					cf_i.location_path = cf.v_s[cf_i.server].location_path[i];
				}
			}
		}
		if(i + 1 == (int)cf.v_s[cf_i.server].location_path.size())
		{
			if (same_value != 0)
				break;
			throw 404;  //설명 : url로 보낸 파일 못찾을 때 404 에러
		}
	}
	if(cf_i.location_path.size() < cf_i.request_url.size())
		cf_i.request_url_remaining = cf_i.request_url.substr(cf_i.location_path.size());
}

void find_cf_server_i(config &cf, request::value_type &fd_data, conf_index &cf_i)
{
	for(int i = 0; i < (int)cf.v_s.size(); i++)
	{
		if(cf.v_s[i].listen == fd_data.second["host"])
		{
			cf_i.server = i;
			break;
		}
		if(i + 1 == (int)cf.v_s.size())
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
		cf_i.root_path = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"][0];
	else
	{
		cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"].push_back(DEFAULT_HTML_PATH);
		cf_i.root_path = cf.v_s[cf_i.server].v_l[cf_i.location].m_location["root"][0];
	}
	std::string path = cf_i.root_path + "/" + cf_i.request_url_remaining;
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
		throw 404; // 확인 : root + url 남은 부분의 경로를 찾을수 없으면 파일을 못찾음. 404에러
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

void find_file_type(std::string &file_name, conf_index &cf_i, std::map<std::string, std::string> &m_mt, bool &cgi_flag)
{
	for(int str_i = file_name.size() - 1; str_i > 0; str_i--)
	{
		if(file_name[str_i] == '/')
			break ;
		else if(file_name[str_i] == '.')
		{
			if(file_name.substr(str_i + 1) == "php")
			{
				cgi_flag = 1;
				std::cout << file_name <<  " : CCCCCCCCCC GGGGGGGGGG  IIIIIIII" << std::endl;
			}
			else if(m_mt.find(file_name.substr(str_i + 1)) != m_mt.end())
			{
				cf_i.file_type = m_mt[file_name.substr(str_i + 1)];
			}
			break;
//					else
//						throw 400;
		}
	}
}

void confirmed_file_path_and_file_type(config &cf, conf_index &cf_i, std::map<std::string, std::string> &m_mt)
{
	if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("index") == cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
		cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].push_back("index.html");

	for(int cnt = 0; cnt < (int)cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].size(); cnt++)
	{
		cf_i.file_path = cf_i.root_path + cf_i.request_url_remaining + "/" +  cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt];
		std::ifstream readFile( cf_i.file_path.c_str());
		if (readFile.is_open())
		{
			if(413 == check_filesize(readFile, cf_i, cf))
			{
				readFile.close();
				throw 413; //확인 : 요청한 파일을 열어보고 사이즈가 더 크면 413 에러
			}
			//find_file_type(cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"][cnt], cf_i, m_mt);
			readFile.close(); // 맞겠지? 
			break;		
		}
		if (cnt + 1 == (int)cf.v_s[cf_i.server].v_l[cf_i.location].m_location["index"].size())
		{
			if(cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("auto_index") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end() && cf.v_s[cf_i.server].v_l[cf_i.location].m_location["auto_index"][0] == "on")
				cf_i.autoindex_flag = 1;
			else 
				throw 404; //확인 : 원하는 경로에 파일 없으니까 404에러!
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
	else throw 400; //확인 : host 값없으면 잘못된 헤더
	if(fd_data.second.find("connection") !=  fd_data.second.end())
	{
		//keep-alive면 접속유지, close의 경우 한번응답해주고 끊음.
		if(fd_data.second["connection"] != "keep-alive" && fd_data.second["connection"] != "close")
			throw 400; //확인 : host 값없으면 잘못된 헤더
		if(fd_data.second["connection"] == "close")
			cf_i.Connection = "close";
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

void make_folder_list(std::string &path_list, std::string path)
{
	DIR * dir = NULL;
	struct dirent * entry;
	struct stat buf;

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
		else ;
			//std::cout << "???  이거 파일도 아니고, 폴더도 아닌게 있어?   " << std::endl;
		path_list += "<a href=\"";
		path_list += tmp_d_name;
		path_list += "\">" + tmp_d_name + "</a>";
		for(int zz = tmp_d_name.size(); zz <= 40; zz++)
			path_list += " ";
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
	//std::cout << std::endl;

}

void input_autoindex_data(conf_index &cf_i, std::string &file_data, std::string &path_list)
{
	std::string find_str1 = "$1";
	std::string find_str2 = "$2";
	file_data.replace(file_data.find(find_str1), find_str1.size(), cf_i.root_path + cf_i.request_url_remaining);
	file_data.replace(file_data.find(find_str1), find_str1.size(), cf_i.root_path + cf_i.request_url_remaining);
	file_data.replace(file_data.find(find_str2), find_str2.size(), path_list);
}

void confirm_file(int &state_code, conf_index &cf_i, std::map<std::string, std::string > &m_mt, std::string &c_open_file)
{
	if(200 <= state_code && 400 > state_code)
	{
		std::string sum_string (DEFAULT_HTML_PATH);
		if(cf_i.autoindex_flag == 1)
			c_open_file = sum_string + "/autoindex.html";
		else
			c_open_file = cf_i.file_path;
	}
	else if(state_code == -900) //error_page
	{
		c_open_file = cf_i.file_path;
		//걱정마! exec_error_page 여기서 열리는지 먼저 확인햇으니까!!!
		cf_i.Connection = "keep-alive";
		//error_page 일때 항상 keep-alive
	}
	else
	{
		c_open_file = (std::string)DEFAULT_HTML_PATH + "/" + std::to_string(state_code) + ".html";
		cf_i.Connection = "keep-alive";
	}
}
void open_file_data(std::string &file_data, std::string &c_open_file)
{
	std::ifstream readFile;

	readFile.open(c_open_file);
	std::string readLine ="";
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
			file_data += readLine + "\n";
		readFile.close();
	}
	else
	{
		std::cout << "앞서 다 확인했지만, 혹시!??" <<std::endl;
		throw open_fail();
	}
}

void exec_autoindex(int &state_code, conf_index &cf_i, std::string &file_data)
{
	if(200 <= state_code && 400 > state_code)
	{
		if(cf_i.autoindex_flag == 1) // cf_i.root_path + cf_i.request_url_remaining 이 폴더 일때만 탐.. 
		{
			std::string path_list = "";
			make_folder_list(path_list, cf_i.root_path + cf_i.request_url_remaining);
			input_autoindex_data(cf_i, file_data, path_list);
		}
	}
}

void make_request_msg(int &state_code, std::string &request_msg, conf_index &cf_i, std::map<std::string, std::string > &m_mt)
{
	bool cgi_flag = 0;
	std::string c_open_file ="";
	std::string file_data = "";

	confirm_file(state_code, cf_i, m_mt, c_open_file);
	find_file_type(c_open_file, cf_i, m_mt, cgi_flag);
		//1. 여기서 파일 확장자 파악하고, .php일때 cgi실행, 
		//2. cgi에서 쓰는 헤더 종류 데이터 종류(Content-type), (content-lenth) 위치 정보(Location-type), 상태 정보(Status-type)
		//3. 만약 cgi_flag == 1이면 cgi를 여기서 실행시켜주자. 
		//4. 상태코드에 따른 첫번째줄 다시 작성하자.
	
	if(cgi_flag == 1)
	{
		// ###################################
		// 여기 cgi  프로그램 실행
		// ###################################
		
		//check_first_line();
	}
	else
	{
		open_file_data(file_data, c_open_file);
		exec_autoindex(state_code, cf_i, file_data);

		request_msg += "Content-Length: " + std::to_string(file_data.size()) + "\r\n";
		if(cf_i.file_type != "")
			request_msg += "Content-type: " + cf_i.file_type + "\r\n";
		else 
			request_msg += (std::string)"Content-type: " + "application/octet-stream" + "\r\n";
	}

	request_msg += "Connection: " + cf_i.Connection + "\r\n";
	// 주의! : CGI 실행되고 connection 다시한번 확인하기!!
	request_msg += (std::string)"Date: " + make_GMT_time() + "\r\n";
	request_msg += "\r\n";
	if(cgi_flag == 1)
		; // 주의! : 여기에는 cgi프로그램 출력값과 request_msg합쳐야할듯.
	else 		
		request_msg += file_data;
}

void exec_error_page(conf_index &cf_i, config &cf, int &error_code, std::vector<std::string> &v_error_page, std::string &request_msg, std::map<int, std::string> &m_state_code)
{
	std::ifstream readFile;

	for(int i = 0; i < (int)v_error_page.size() - 1; i++)
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

void error_page(int &state_code, conf_index &cf_i, config &cf, std::string &request_msg, std::map<int, std::string> &m_state_code)
		
{
	if (cf_i.root_path == "")
		cf_i.root_path = DEFAULT_HTML_PATH;
	if (cf_i.server != -1 && cf_i.location != -1 && cf.v_s[cf_i.server].v_l[cf_i.location].m_location.find("error_page") != cf.v_s[cf_i.server].v_l[cf_i.location].m_location.end())
	{
		exec_error_page(cf_i, cf, state_code, cf.v_s[cf_i.server].v_l[cf_i.location].m_location["error_page"], request_msg, m_state_code);	
	}
	else if (cf_i.server != -1)
	{
		exec_error_page(cf_i, cf, state_code, cf.v_s[cf_i.server].v_error_page[0].second, request_msg, m_state_code);	
	}

}
void make_request_redirect(std::string &request_msg, conf_index &cf_i)
{
	//request_msg += (std::string)"Content-Type: text/html" + "\r\n"; //리다이렉션은 필요없을듯..
	request_msg += (std::string)"Connection: keep-alive" + "\r\n";
	request_msg += (std::string)"Date: " + make_GMT_time() + "\r\n";
	request_msg += (std::string)"Location: " + cf_i.redirect_path + "\r\n" + "\r\n";
}

void make_request(int &state_code, std::string &request_msg, conf_index &cf_i, std::map<int, std::string> &m_state_code, config &cf, std::map<std::string, std::string > &m_mt)
{
	request_msg = (std::string)"HTTP/1.1 " + std::to_string(state_code) + " " + m_state_code[state_code] + "\r\n";
	if(cf_i.redirect_path != "")
		make_request_redirect(request_msg, cf_i);
	else
	{
		if(cf_i.autoindex_flag != 1)
			error_page(state_code, cf_i, cf, request_msg, m_state_code);
		make_request_msg(state_code, request_msg, cf_i, m_mt);
	}
}

void    exec_request(config &cf, fd_set &write_fds, request *rq, std::string &request_msg, std::map<int, std::string> &m_state_code, std::map<std::string, std::string > &m_mt)
{
	while (!rq->empty())
	{	
		conf_index	cf_i;
	    request::iterator it = rq->rq_begin(); //rq 맨 처음에 있는 값에 대해처리

		request_msg = "";
	    std::cout << "executing method(GET, POST, DELETE)" << std::endl;
		//=====================입력값 확인.=====================

	  if (!rq->is_invalid(it))
	        rq->print();
		try{
      		exec_header(cf, *it, cf_i, m_mt);
			exec_method(cf, it, cf_i);
			exec_body();
			throw 200; //확인 : GET 일때 정상 응답!
		}
		catch (int state_code)
		{
			make_request(state_code, request_msg, cf_i, m_state_code, cf, m_mt);
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
