SRC = basic.cpp open_config.cpp parse_config.cpp request.cpp Servers.cpp cgi.cpp Download.cpp
BIN = bin
OBJ = $(SRC:%.cpp=${BIN}/%.o)
FLAGS = -Wall -Werror -Wextra -Wshadow -g -std=c++98
CC = c++
NAME = webserv
INC = ./includes/webserv.hpp 

COLOUR =\033[0;32m
END =\033[0m

all : $(NAME)

$(NAME) : $(BIN) $(OBJ) $(INC)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "$(COLOUR) SUCCESSFUL COMPILE $(END)"

$(BIN) :
	@mkdir -p $(BIN)

$(BIN)/%.o : %.cpp
	@$(CC) $(FLAGS) -c $< -o $@

run : all
	@./$(NAME) basic.config

valgrind : all
	@valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) basic.config

clean : 
	@rm -rf $(OBJ) $(BIN)

fclean : clean
	@rm -rf $(NAME)

re : fclean all