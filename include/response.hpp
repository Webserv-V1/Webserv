#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <sys/select.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
# include <list>
# include <iostream>

class response
{
public:
	typedef std::pair<int, std::string>	value_type;
	typedef std::list<value_type>		value_arr;
	typedef value_arr::iterator			iterator;
private:
	value_arr	rp;
public:
	fd_set		&write_fds;

	response(fd_set &wfds) : rp(), write_fds(wfds) {}
	~response(void) {}
	void	print(void)
	{
		iterator	it;

		for (it = rp.begin(); it != rp.end(); ++it)
		{
			std::cout << "[fd - " << it->first << "]\n";
			std::cout << it->second << "\n";
		}
	}
	void	push_rp(int fd, std::string to_rp)
	{
		rp.push_back(std::make_pair(fd, to_rp));
	}
	iterator	find_clnt(int fd)
	{
		iterator	it;
		for (it = rp.begin(); it != rp.end(); ++it)
		{
			if (it->first == fd)
				break ;
		}
		return (it);
	}
	void	erase_rp(iterator it)
	{
		rp.erase(it);
	}
};

#endif
