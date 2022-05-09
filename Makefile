CXX				= clang++
RM				= rm -f
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98
NAME			= webserv

SRCS			= srcs/webserv.cpp srcs/parsing.cpp srcs/connection.cpp srcs/exec_request.cpp srcs/request.cpp srcs/cgi_preprocessing.cpp
#OBJS_DIR		= objs

OBJ_DIR     = objs
OBJS         	= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

#OBJS			= $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

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
