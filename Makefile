CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address
#CFLAGS = -std=c++98

NAME = webserv
SRCS = webserv.cpp parsing.cpp connection.cpp exec_request.cpp request.cpp cgi_preprocessing.cpp

all : $(NAME)

$(NAME) :
	$(CC) $(CFLAGS) -o $(NAME) $(SRCS)

clean :
	rm -f $(NAME)

fclean : clean

re : fclean all
