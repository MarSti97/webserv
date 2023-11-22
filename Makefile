SRC = Main.cpp ParseConfig.cpp ValidateConfig.cpp Content.cpp Servers.cpp Serv.cpp Cgi.cpp Download.cpp\
Directory.cpp Tools.cpp Disposition.cpp Request.cpp ResponseCheck.cpp ResponseTools.cpp Response.cpp\
ServTools.cpp FilterServers.cpp FilterRequest.cpp DeleteMethod.cpp
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
	@./$(NAME) example.config 2> errors.txt

valgrind : all
	@valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) Config/example.config

clean : 
	@rm -rf $(OBJ) $(BIN)
	@rm -rf ./guarder-html/cgi-bin/uploads

fclean : clean
	@rm -rf $(NAME)


re : fclean all