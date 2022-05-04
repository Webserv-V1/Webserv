CXX				= clang++
RM				= rm -f
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98
NAME			= webserv

SRCS			= webserv.cpp parsing.cpp connection.cpp exec_request.cpp request.cpp cgi_preprocessing.cpp

OBJS_DIR		= objs

OBJS			= $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

$(OBJS_DIR)/%.o:%.cpp
				@mkdir -p $(OBJS_DIR)
				$(CXX) $(CXXFLAGS) -c $< -o $(<:%.cpp=$(OBJS_DIR)/%.o)

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
				$(RM) $(OBJS)
				rm -rf $(OBJS_DIR)

fclean:			clean
				$(RM) $(NAME)

re:				fclean $(NAME)

.PHONY:			all clean fclean re
