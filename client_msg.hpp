#include <string>
#include <vector>
#include <iostream>

class client_msg
{
private:
	std::vector<std::string>	split_msg;
	std::string					msg;
public:
	client_msg(void) : split_msg(), msg("") {}

	void						print(void)
	{
		std::cout << "[print in client_msg]" << std::endl;
		std::vector<std::string>::iterator it;
		for (it = split_msg.begin(); it != split_msg.end(); ++it)
			std::cout << "one line" << std::endl << *it << std::endl;
		std::cout << "<end>" << std::endl << std::endl;
	}

	void						strjoin(std::string str)
	{
		str.erase(remove(str.begin(), str.end(), '\r'), str.end());
		msg.append(str);
	}

	std::vector<std::string>	&parse_line(void)
	{
		size_t	last = 0;
		size_t	next = 0;
		std::string	tmp;

		while ((next = msg.find("\n", last)) != std::string::npos)
		{
			split_msg.push_back(msg.substr(last, next - last));
			last = next + 1;
			if (msg[last] == '\n')
			{
				split_msg.push_back("");
				last++;
				break ;
			}
		}
		//split_msg.push_back(msg.substr(last - 1));
		next = msg.rfind("\n");
		tmp = msg.substr(last, next - last);
		/*if (!tmp.compare("\n"))
			split_msg.push_back("");
		else*/
		split_msg.push_back(tmp);
		return (split_msg);
	}

	/*void	push_back(bool &is_first, std::string to_push)
	{
		std::cout << "\t\t\tpush back - ";
		if (is_first)
		{
			std::cout << "is first" << std::endl;
			msg.push_back(remain + to_push);
			is_first = false;
			remain = "";
		}
		else
		{
			std::cout << "not first" << std::endl;
			msg.push_back(to_push);
		}
	}

	void	insert(std::string str)
	{
		//find, substr 이용
		bool		is_first = true;
		size_t		start = 0;
		size_t		len = 0;
		size_t		count;
		std::string	de("\r\n");
		std::string	tmp;
		std::cout << "insert called" << std::endl;
		std::cout << "\t" << str << std::endl;
		for (std::size_t i = 0; i < str.length(); i++)
		{
			if (de.find(str[i]) == std::string::npos)
			{
				//std::cout << "\t\tno deli" << std::endl;
				len++;
			}
			else
			{
				std::cout << "\t\telse" << std::endl;
				if (!is_first)
				{
					tmp = str.substr(start, len);
					std::cout << "\t\tpushing " << tmp << std::endl;
					push_back(is_first, tmp);
					start += len;
					len = 0;
				}

				count = 0;
				while (str[i] && de.find(str[i]) != std::string::npos)
				{
					if (str[i] == '\n')
						count++;
					i++;
					start = i;
				}
				while (count > 1)
				{
					push_back(is_first, "");
					count--;
				}
			}
		}
		remain = str.substr(start);
	}*/

	void	clear(void)
	{
		split_msg.clear();
		msg = "";
	}
};