#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <exception>


struct open_fail : std::exception {
  const char* what() const throw() {return "config file is not open!";}
};

struct wrong_configfile_data_1: std::exception {
  const char* what() const throw() {return "1. confile is data wrong!, key that cannot be inserted";}
};

struct bracket_order_fail : std::exception {
  const char* what() const throw() {return "The order of bracket is wrong.";}
};

struct open_bracket_after_fail : std::exception {
  const char* what() const throw() {return "open bracket after come wrong data";}
};

struct wrong_configfile_data_2 : std::exception {
  const char* what() const throw() {return "2. confile is data wrong!, key that cannot be inserted";}
};

class location{
public :
	std::map< std::string, std::vector<std::string> >	m_location;
};

class server{
public :
	int location_i;
	int location_flag;

	std::vector<std::string>			location_path;
	std::string					server_name;
	std::string					error_page;
	std::vector<std::string>	v_listen; //[0] 포트 [1 ~ 4] ip
	std::vector<location>		v_l;
	//std::map< std::string, std::vector<std::string> >	m_location;
	
	server(void)
	{
		location_i = -1;
		location_flag = 0;
		server_name = "";
		error_page = "";
	}
};

class config{
public :
	int					server_i;
	int					server_flag;
	int					bracket;
	std::string			file_name;

	std::vector<server>			v_s;
	std::vector<std::string>	v_server_invalid_key;
	std::vector<std::string>	v_location_invalid_key;
	std::stack<std::string>		s_bracket_open_order;

	config(std::string _file_name){
		server_i = -1;
		server_flag = 0;
		bracket = 0;
		file_name = _file_name;
		std::string s_invalid_key[] = {"location", "server_name", "listen", "error_page"};
		std::string l_invalid_key[] = { "error_page", "allow_methods", \
				"request_max_body_size", "root", "index", "auto_index", \
				"cgi_extension", "auth_key", "client_body_buffer_size", "upload_path", "return"};

	v_server_invalid_key.insert(v_server_invalid_key.begin(), s_invalid_key, s_invalid_key + sizeof(s_invalid_key) / sizeof(std::string));
	v_location_invalid_key.insert(v_location_invalid_key.begin(), l_invalid_key, l_invalid_key + sizeof(l_invalid_key) / sizeof(std::string));
	}
private : 

};

void check_invalid(std::string &readLine, std::string com, int i)
{
	if(readLine.compare(i, com.size(), com) != 0)
		throw wrong_configfile_data_1();
}

void set_cf(config &cf, std::string readLine)
{
	std::cout << readLine << std::endl;
	int input_flag = 0; //값 저장해야하는 경우.
	std::string tmp = "";
	for(int i = 0; i < readLine.size(); i++)
	{
		if(readLine[i] == '#')
			break ;
		else if(readLine[i] == ' ' || readLine[i] == '	')
			;
		else if(readLine[i] == '}')
		{
			if(cf.s_bracket_open_order.empty())
				throw bracket_order_fail();
			if (cf.s_bracket_open_order.top() == "server")
				cf.server_flag--;
			else if(cf.s_bracket_open_order.top() == "location")
				cf.v_s[cf.server_i].location_flag--;
			cf.s_bracket_open_order.pop();
		}
		else
		{
			//bracket 은 server, location같은게 오고난뒤 1로 해줌. 그리고 {가 나오고 나면 0으로 치환.
			if(input_flag != 0)
			{
				tmp += readLine[i];
				if(readLine[i + 1] == ' ' || readLine[i + 1] == '#' || i + 1 == readLine.size())
				{
					//input_flag 1~2는server클래스 데이터 저장.
					if(input_flag == 1)
						cf.v_s[cf.server_i].server_name = tmp;
					else if(input_flag == 2)
						cf.v_s[cf.server_i].v_listen.push_back(tmp);
					else if(input_flag == 3)
						cf.v_s[cf.server_i].error_page = tmp;
					else if(input_flag > 3)
						cf.v_s[cf.server_i].v_l[cf.v_s[cf.server_i].location_i].m_location[cf.v_location_invalid_key[input_flag - 3]].push_back(tmp);
					tmp = "";
				}
			}
			else if(cf.bracket == 1 && readLine[i] == '{')
				cf.bracket--;
			else if (cf.bracket == 1)
			{
				std::cout << readLine[i] << "  "<< i << std::endl;
				throw open_bracket_after_fail();
			}
			else
			{
				if(cf.server_flag == 0)
				{
					check_invalid(readLine, "server", i);
					cf.server_i++;
					cf.server_flag++;
					cf.v_s.push_back(server());
					cf.bracket++;
					cf.s_bracket_open_order.push("server");
					i = i + 5;
				}
				else if(cf.server_flag != 0 && cf.v_s[cf.server_i].location_flag == 0)
				{
					// server{
					// 이 상황일떄,
					int key_i = 0;
					for(key_i = 0; key_i < cf.v_server_invalid_key.size(); key_i++)
						if(readLine.substr(i, cf.v_server_invalid_key[key_i].size()) == cf.v_server_invalid_key[key_i])
							break;
					if(key_i == cf.v_server_invalid_key.size())
					{
						throw wrong_configfile_data_2();
					}
					i = i + (cf.v_server_invalid_key[key_i].size() - 1);
					if(key_i == 0) //i==0일때는 readLine이 location임.
					{
						cf.v_s[cf.server_i].location_i++;
						cf.v_s[cf.server_i].location_flag++;
						cf.bracket++;
						cf.s_bracket_open_order.push("location");
						cf.v_s[cf.server_i].v_l.push_back(location());
						std::string location_path = "";
						int flag = 0;
						for(i = i + 1; i <readLine.size(); i++)
						{
							if(readLine[i] == ' ' || readLine[i] == '	' || readLine[i] == '#' || readLine.size() - 1 == i)
							{
								if(flag == 1)
								{
									cf.v_s[cf.server_i].location_path.push_back(location_path);
									flag = 2;
								}
								else if(flag == 0 && (readLine.size() - 1 == i || readLine[i] == '#'))
								{
									if(readLine[i] != ' ' || readLine[i] != '	')
										cf.v_s[cf.server_i].location_path.push_back(location_path);
									else
									{
										std::cout << "dlrjdisi" <<std::endl;
										throw wrong_configfile_data_2();
									}
								}
								if(flag == 2 && readLine[i] == '#')
								{
									i = readLine.size() - 1;
									break;
								}
							}
							else
							{
								if(flag == 2)
								{
									throw wrong_configfile_data_2();
								}
								location_path += readLine[i];
								flag = 1;
							}

//							if(flag == 1 && (readLine[i] == ' ' || readLine[i] == '#' || readLine[i] == '	'))
//								cf.v_s[cf.server_i].location_path = location_path;
//							if(readLine[i] == '#')
//							{
//								cf.v_s[cf.server_i].location_path.push_back(location_path);
//								i = readLine.size() - 1;
//							}
//							else if(readLine[i] == ' ' || readLine[i] == '	' || readLine.size() - 1 == i)
//							{
//								if(flag == 1)
//								{
//									cf.v_s[cf.server_i].location_path.push_back(location_path);
//									flag = 2;
//								}
//							}
//							else if(readLine[i] != ' ')
//							{
//								if (flag == 2)
//								{
//									std::cout << " rjfflskrdy? : " << readLine[i]<< std::endl;
//									throw wrong_configfile_data_2();
//								}
//								location_path += readLine[i];
//								flag = 1;
//							}
						}
					}
					else //key_i==1,2, 3일때 "server_name", "error_page"
					{
						input_flag = key_i; //값 저장해야하는 경우.
					}
				}
				else if(cf.server_flag != 0 && cf.v_s[cf.server_i].location_flag != 0)
				{
					// server{
					//		location{
					//			이 상황일떄,
					int key_i = 0;
					for(key_i = 0; key_i < cf.v_location_invalid_key.size(); key_i++)
						if(readLine.substr(i, cf.v_location_invalid_key[key_i].size()) == cf.v_location_invalid_key[key_i])
						{
							break;
						}
					if(key_i == cf.v_location_invalid_key.size())
					{
						throw wrong_configfile_data_2();
					}
					i = i + (cf.v_location_invalid_key[key_i].size() - 1);
					input_flag = key_i + 3; //값 저장해야하는 경우.
				}
			}
		}
	}
}

void config_parsing(config &cf)
{
	std::string readLine;
	std::ifstream readFile;

	readFile.open(cf.file_name.c_str()); //내부에 실패시 throw함.
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
		{
			set_cf(cf, readLine);
		}
	}
	else
		throw open_fail();
}


int main(void)
{
	try{
		config cf("webserv.conf");

		config_parsing(cf);
		std::cout << std::endl;
		std::cout << " test == test " << std::endl;
		for(int i  = 0; i < cf.v_s.size(); i++)
		{
			std::cout << "======================================== v_s [ " << i << " ] ======================================" <<std::endl;

			std::cout << "location_path :";
			for(int j = 0; j < cf.v_s[i].location_path.size(); j++)
				std::cout << " " << cf.v_s[i].location_path[j];
			std::cout << std::endl;
			std::cout << "location : " << cf.v_s[i].location_path[1] << std::endl;
			std::cout << "server_name : " << cf.v_s[i].server_name << std::endl;
			std::cout << "listen :";
			for(int j = 0; j < cf.v_s[i].v_listen.size(); j++)
				std::cout << " " << cf.v_s[i].v_listen[j];
			std::cout << std::endl;
			std::cout << "error_page : "<< cf.v_s[i].error_page << std::endl;
			int zz = 0;
			for(int z = 0; z < cf.v_s[i].v_l.size(); z++)
			{
				std::cout << "=========== v_s [ " << i << " ].map [ " << z << "  ===========" <<std::endl;
				for(std::map< std::string, std::vector<std::string> >::iterator iter = cf.v_s[i].v_l[z].m_location.begin(); iter!= cf.v_s[i].v_l[z].m_location.end(); iter++)
				{
	
					std::cout << iter->first << " :";
					for(int z = 0; z < iter->second.size(); z++)
					{
						std::cout << " " << iter->second[z];
					}
					std::cout << std::endl;
					zz++;
	
				}
			}
		}
	}
	catch (std::exception & e)
	{
	    std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "error 인데, 여긴 타면 안되지..." << std::endl;
	}
}
