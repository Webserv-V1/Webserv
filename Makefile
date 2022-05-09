CXX				= clang++
RM				= rm -f
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98
NAME			= webserv
SRCS_DIR		= srcs
SRCS			= srcs/webserv.cpp srcs/parsing.cpp srcs/connection.cpp srcs/exec_request.cpp srcs/request.cpp srcs/cgi_preprocessing.cpp
OBJS_DIR		= objs

OBJS			= $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)
#OBJ         	= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
#OBJS			= $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
				@mkdir -p $(OBJS_DIR)
				$(CXX) $(CXXFLAGS) -c $< -o $(<:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)

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
