#include "./include/parsing.hpp"
#include "./include/error.hpp"

void check_invalid(std::string &readLine, std::string com, int i)
{
	if(readLine.compare(i, com.size(), com) != 0)
		throw wrong_configfile_data_1();
}

void check_closebrackket_order(config &cf)
{
	if(cf.s_bracket_open_order.empty())
		throw bracket_order_fail();
	if (cf.s_bracket_open_order.top() == "server")
		cf.server_flag--;
	else if(cf.s_bracket_open_order.top() == "location")
		cf.v_s[cf.server_i].location_flag--;
	else
		throw bracket_order_fail();
	cf.s_bracket_open_order.pop();
}

void input_listen(config &cf, std::string &save_data, const int &input_data_cnt)
{
	if(input_data_cnt != 0)
	{
		throw err_input_listen();
	}
	else
	{
		if(save_data.size() < 7)
		{
			for(int i = 0; i < (int)save_data.size(); i++)
				if(save_data[i] < '0' || save_data[i] > '9')
					throw  err_input_listen();
			if(stoi(save_data) < 0 && stoi(save_data) > 65535)
				throw  err_input_listen();
			cf.v_s[cf.server_i].v_listen[0] = save_data;
		}
		else
		{
			int num_cnt = 0;
			int ip_cnt = 0;
			std::string s_ip = "";
			std::string s_num = "";
			for(int i = 0 ; i < (int)save_data.size(); i++)
			{
				if(save_data[i] >= '0' && save_data[i] <= '9')
				{
					s_num += save_data[i];
					num_cnt++;
					if((int)save_data.size() - 1 == i || save_data[i + 1] == '.' || save_data[i + 1] == ':')
					{
						if(ip_cnt < 4 && ((stoi(s_num) >= 0 && stoi(s_num) <= 255) && ((((int)save_data.size() != i + 1) && save_data[i + 1] == '.') || ((int)save_data.size() == i + 1 && ip_cnt == 3) || (((int)save_data.size() != i + 1) && (save_data[i + 1] == ':' && ip_cnt == 3)))))
						{
							num_cnt = 0;
							s_ip += s_num;
							s_num = "";
							if(ip_cnt == 3)
								cf.v_s[cf.server_i].v_listen[1] = s_ip;
							ip_cnt++;
							s_ip += ".";
							i++;
						}
						else if(ip_cnt == 4 && ((int)save_data.size() == i + 1 && (s_num != "" && (stoi(s_num) >= 0 && stoi(s_num) <= 65535))))
						{
							cf.v_s[cf.server_i].v_listen[0] = s_num;
							ip_cnt++;
							s_num = "";
							num_cnt =0;
						}
					}
				}
				else
					throw err_input_listen();
			}
		}
		cf.v_s[cf.server_i].listen = cf.v_s[cf.server_i].v_listen[1] + ":" + cf.v_s[cf.server_i].v_listen[0];
	}
	return ;
}

void value_check(int input_flag, std::string save_data)
{
	if(input_flag == 4) //allow_methods
	{
		 if((save_data) != "GET" && (save_data) != "POST" && (save_data) != "DELETE")
			 throw config_error_allow_methods();
	}
}
void input_data(config &cf, std::string &save_data, int &input_flag, std::string readLine, int i, int &input_data_cnt)
{
	save_data += readLine[i];
	if(readLine[i + 1] == ' ' || readLine[i + 1] == '	' || readLine[i + 1] == '#' || i + 1 == (int)readLine.size())
	{
		//설명 : input_data 0 은 location 일때,
		//설명 : input_flag 1~2는server클래스 데이터 저장.
		//설명 : input_flag 2 보다 큰 값은 location저장.
		if(input_flag == 1)	// "listen"
			input_listen(cf, save_data, input_data_cnt);
		else if(input_flag == 2)	// "error_page"
		{
			if(input_data_cnt == 0)
				cf.v_s[cf.server_i].v_error_page.push_back(make_pair( cf.v_s[cf.server_i].location_i + 1, std::vector<std::string>(1, save_data)));
			else
				cf.v_s[cf.server_i].v_error_page[cf.v_s[cf.server_i].v_error_page.size() - 1].second.push_back(save_data);
		}
		else if(input_flag > 2)
		{
			value_check(input_flag, save_data);
			if(input_data_cnt == (int)cf.v_s[cf.server_i].v_l[cf.v_s[cf.server_i].location_i].m_location[cf.v_location_invalid_key[input_flag - 3]].size())
				cf.v_s[cf.server_i].v_l[cf.v_s[cf.server_i].location_i].m_location[cf.v_location_invalid_key[input_flag - 3]].push_back(save_data);
			else
				cf.v_s[cf.server_i].v_l[cf.v_s[cf.server_i].location_i].m_location[cf.v_location_invalid_key[input_flag - 3]][input_data_cnt] = (save_data);

		}
		save_data = "";
		input_data_cnt++;
	}
}

void after_server(config &cf, int &i, std::string &readLine)
{
	check_invalid(readLine, "server", i);
	cf.server_i++;
	cf.server_flag++;
	cf.v_s.push_back(server());
	cf.bracket++;
	cf.s_bracket_open_order.push("server");
	i = i + 5;
}

void erase_slash(std::string &location_path)
{
	std::string new_location = "";
	std::string save = "";
	int full_flag = 0;
	for(int lo_s_i = 0; lo_s_i < (int)location_path.size(); lo_s_i++)
	{
		if(location_path[lo_s_i] != '/')
		{
			new_location += location_path[lo_s_i];
			full_flag = 1;
			if((int)location_path.size() == lo_s_i + 1)
				save += "/" + new_location;
		}
		else if (((int)location_path.size() == lo_s_i + 1 || location_path[lo_s_i] == '/') && full_flag == 1)
		{
			save += "/" + new_location;
			new_location = "";
			full_flag = 0;
		}
	}
	if(location_path.size() == 1)
	{
		if(location_path[0] == '/')
			;
		else
		{
			location_path = "/" + location_path;
		}
	}
	else location_path = save;


//	std::string new_location = "";
//	std::string save = "";
//	int full_flag = 0;
//	for(int lo_s_i = 0; lo_s_i <  cf.v_s[i].location_path[j].size(); lo_s_i++)
//	{
//		if(cf.v_s[i].location_path[j][lo_s_i] != '/')
//		{
//			new_location += cf.v_s[i].location_path[j][lo_s_i];
//			full_flag = 1;
//			if(cf.v_s[i].location_path[j].size() == lo_s_i + 1)
//				save += "/" + new_location;
//		}
//		else if ((cf.v_s[i].location_path[j].size() == lo_s_i + 1 || cf.v_s[i].location_path[j][lo_s_i] == '/') && full_flag == 1)
//		{
//			save += "/" + new_location;
//			new_location = "";
//			full_flag = 0;
//		}
//	}
//	if(cf.v_s[i].location_path[j].size() == 1)
//	{
//		if(cf.v_s[i].location_path[j][0] == '/')
//			;
//		else
//		{
//			cf.v_s[i].location_path[j] = "/" + cf.v_s[i].location_path[j];
//		}
//	}
//	else cf.v_s[i].location_path[j] = save;
}

void after_location(config &cf, int &i, std::string &readLine)
{
	cf.v_s[cf.server_i].location_i++;
	cf.v_s[cf.server_i].location_flag++;
	cf.bracket++;
	cf.s_bracket_open_order.push("location");
	cf.v_s[cf.server_i].v_l.push_back(location());
	std::string location_path = "";
	int flag = 0;
	for(i = i + 1; i < (int)readLine.size(); i++)
	{
		if(readLine[i] == ' ' || readLine[i] == '	' || readLine[i] == '#' || (int)readLine.size() - 1 == i)
		{
			if(flag == 1)
			{
				if((int)readLine.size() - 1 == i)
					location_path += readLine[i];
				erase_slash(location_path);
				cf.v_s[cf.server_i].location_path.push_back(location_path);
				flag = 2;
			}
			else if(flag == 0 && ((int)readLine.size() - 1 == i || readLine[i] == '#'))
			{
				if((int)readLine.size() - 1 == i)
					location_path += readLine[i];
				if(readLine[i] != ' ' || readLine[i] != '	')
				{
					erase_slash(location_path);
					cf.v_s[cf.server_i].location_path.push_back(location_path);
				}
				else
					throw wrong_configfile_data_2();
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
				throw wrong_configfile_data_2();
			location_path += readLine[i];
			flag = 1;
		}
	}
}

int check_right_value_for_sever(config &cf, std::string &readLine, int &i)
{
	int key_i = 0;

	for(key_i = 0; key_i < (int)cf.v_server_invalid_key.size(); key_i++)
		if(readLine.substr(i, cf.v_server_invalid_key[key_i].size()) == cf.v_server_invalid_key[key_i])
			break;
	if(key_i == (int)cf.v_server_invalid_key.size())
		throw wrong_configfile_data_2();
	i = i + (cf.v_server_invalid_key[key_i].size() - 1);
	return key_i;
}


void check_first_char(config &cf, int &input_flag, std::string &readLine, int &i, int &input_data_cnt)
{
	if(cf.server_flag == 0)
		after_server(cf, i, readLine);
	else if(cf.server_flag != 0 && cf.v_s[cf.server_i].location_flag == 0)
	{
		// 설명: server{
		// 설명: 이 상황일떄,
		int key_i = check_right_value_for_sever(cf, readLine, i);
		if(key_i == 0)	//설명 : readLine이 location임.
			after_location(cf, i, readLine);
		else			//설명 : key_i==1,2일때 "listen", "error_page"
			input_flag = key_i;
	}
	else if(cf.server_flag != 0 && cf.v_s[cf.server_i].location_flag != 0)
	{
		// 설명 : server{
		// 설명 : location{
		// 설명 :	이 상황일떄,
		int key_i = 0;
		for(key_i = 0; key_i < (int)cf.v_location_invalid_key.size(); key_i++)
			if(readLine.substr(i, cf.v_location_invalid_key[key_i].size()) == cf.v_location_invalid_key[key_i])
			{
				break;
			}
		if(key_i == (int)cf.v_location_invalid_key.size())
		{
			throw wrong_configfile_data_2();
		}
		i = i + (cf.v_location_invalid_key[key_i].size() - 1);
		input_flag = key_i + 3;
	}
	input_data_cnt = 0;
}

void set_cf(config &cf, std::string readLine)
{
	int input_flag = 0;
	int input_data_cnt = 0;
	std::string save_data = "";

	for(int i = 0; i < (int)readLine.size(); i++)
	{
		if(readLine[i] == '#')
			break ;
		else if(readLine[i] == ' ' || readLine[i] == '	')
			;
		else if(readLine[i] == '}')
			check_closebrackket_order(cf);
		else
		{
			if(input_flag != 0)
			{
				input_data(cf, save_data, input_flag, readLine, i, input_data_cnt);
			}
			else if(cf.bracket == 1 && readLine[i] == '{')
				cf.bracket--;
				//설명: bracket 은 server, location 같은 블록 이름이 온뒤 1로 해줌.
				//설명: 그리고 {가 나오고 나면 0으로 치환.
			else if (cf.bracket == 1)
				throw open_bracket_after_fail();
			else
				check_first_char(cf, input_flag, readLine, i, input_data_cnt);
		}
	}
}

void check_l_root(config &cf, int &i, int &j)
{
	if(cf.v_s[i].v_l[j].m_location.find("root") != cf.v_s[i].v_l[j].m_location.end())
	{
		if(cf.v_s[i].v_l[j].m_location["root"].size() != 1)
			 throw config_error_root();
	}
}

void check_l_error_page(config &cf, int &i, int &j)
{
	if(cf.v_s[i].v_l[j].m_location.find("error_page") != cf.v_s[i].v_l[j].m_location.end())
	{
		if(1 == cf.v_s[i].v_l[j].m_location["error_page"].size())
			throw config_error_error_page();
		for(int z = 0; z < (int)cf.v_s[i].v_l[j].m_location["error_page"].size() - 1 ; z++)
		{
			if("300" > cf.v_s[i].v_l[j].m_location["error_page"][z] || "599" < cf.v_s[i].v_l[j].m_location["error_page"][z])
				throw config_error_error_page();
		}
	}
}

void check_l_client_max_body_size(config &cf, int &i, int &j)
{
	if(cf.v_s[i].v_l[j].m_location.find("client_max_body_size") != cf.v_s[i].v_l[j].m_location.end())
	{
		if(cf.v_s[i].v_l[j].m_location["client_max_body_size"].size() != 1)
			 throw config_error_client_max();
		if(stol(cf.v_s[i].v_l[j].m_location["client_max_body_size"][0]) > 2048) // 1기가 넘어가면 에러
			 throw config_error_client_max();
		for(int zz = 0; zz < (int)cf.v_s[i].v_l[j].m_location["client_max_body_size"][0].size(); zz++)
		{
			if ((int)cf.v_s[i].v_l[j].m_location["client_max_body_size"][0].size() - 1 != zz && !isdigit(cf.v_s[i].v_l[j].m_location["client_max_body_size"][0][zz]))
				throw config_error_client_max();
			if ((int)cf.v_s[i].v_l[j].m_location["client_max_body_size"][0].size() - 1 == zz && !isdigit(cf.v_s[i].v_l[j].m_location["client_max_body_size"][0][zz]) && cf.v_s[i].v_l[j].m_location["client_max_body_size"][0][zz] != 'M')
				throw config_error_client_max();
		}
	}
}
void check_l_return(config &cf, int &i, int &j)
{
	if(cf.v_s[i].v_l[j].m_location.find("return") != cf.v_s[i].v_l[j].m_location.end())
	{
		std::string ttmp[6] = {"301", "308", "302", "303", "307", "308"};
		std::vector<std::string> amd (ttmp, ttmp + 6);
		if(cf.v_s[i].v_l[j].m_location["return"].size() != 2 || find(amd.begin(), amd.end(), cf.v_s[i].v_l[j].m_location["return"][0]) == amd.end() )
			throw config_error_return();
	}
}

void location_check(config &cf, int &i)
{
	for(int j = 0; j < (int)cf.v_s[i].location_path.size(); j++)
	{
		check_l_root(cf, i, j);
		check_l_error_page(cf, i, j);
		check_l_client_max_body_size(cf, i, j);
		check_l_return(cf, i, j);
	}
}

void last_check(config &cf)
{
	if(cf.v_s.size() == 0)
	{
		std::cout << "이거 안탈거아ㅇ냐? " << std::endl;
		throw config_error_server();
	}
	for(int i = 0; i < (int)cf.v_s.size(); i++)
	{
		if(cf.v_s[i].location_path.size() == 0)
			throw config_error_location();
		for(int j = 0; j < (int)cf.v_s[i].v_error_page.size(); j++)
		{
			for(int z = 0; z < (int)cf.v_s[i].v_error_page[j].second.size() - 1; z++)
			{
				if("300" > cf.v_s[i].v_error_page[j].second[z] || "599" < cf.v_s[i].v_error_page[j].second[z])
					throw config_error_error_page();
			}
		}
		location_check(cf, i);
	}
}

void config_parsing(config &cf)
{
	std::string readLine;
	std::ifstream readFile;

	readFile.open(cf.file_name.c_str());
	if (readFile.is_open())
	{
		while (getline(readFile, readLine))
			set_cf(cf, readLine);
		readFile.close();
	}
	else
		throw open_fail();
	last_check(cf);
}

void print_cf_data(config &cf)
{
	std::cout << "========= print confile data ========== " << std::endl;
	for(int i  = 0; i < (int)cf.v_s.size(); i++)
	{
		std::cout << "======================================== v_s [ " << i << " ] ======================================" <<std::endl;

		std::cout << "location_path :";
		for(int j = 0; j < (int)cf.v_s[i].location_path.size(); j++)
			std::cout << " " << cf.v_s[i].location_path[j];
		std::cout << std::endl;
		std::cout << "listen :";
		for(int j = 0; j < (int)cf.v_s[i].v_listen.size(); j++)
		{
			std::cout << " " << cf.v_s[i].v_listen[j];
			std::cout << " " << cf.v_s[i].listen;
		}
		std::cout << std::endl;
		for(int z = 0; z < (int)cf.v_s[i].v_error_page.size(); z++)
		{
			std::cout << " error_page[" << z << "] : location_i : " << cf.v_s[i].v_error_page[z].first << "   path : " ;
			for(int zz = 0; zz < (int)cf.v_s[i].v_error_page[z].second.size(); zz++)
			{
				std::cout << cf.v_s[i].v_error_page[z].second[zz] << " ";
			}
			std::cout << std::endl;
		}
		int zz = 0;
		for(int z = 0; z < (int)cf.v_s[i].v_l.size(); z++)
		{
			std::cout << "=========== v_s [ " << i << " ].map [ " << z << "  ===========" <<std::endl;
			for(std::map< std::string, std::vector<std::string> >::iterator iter = cf.v_s[i].v_l[z].m_location.begin(); iter!= cf.v_s[i].v_l[z].m_location.end(); iter++)
			{

				std::cout << iter->first << " :";
				for(int z = 0; z < (int)iter->second.size(); z++)
				{
					std::cout << " " << iter->second[z];
				}
				std::cout << std::endl;
				zz++;

			}
		}
		std::cout << std::endl;
	}

}

//int main(void)
//{
//	try{
//		config cf("webserv.conf");
//
//		config_parsing(cf);
//		print_cf_data(cf);
//	}
//	catch (std::exception & e)
//	{
//	    std::cerr << e.what() << std::endl;
//	}
//	catch (...)
//	{
//		std::cout << "error 인데, 여긴 타면 안되지..." << std::endl;
//	}
//}
