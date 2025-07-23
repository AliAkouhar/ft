CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = src/main.cpp src/Server.cpp src/Client.cpp
NAME = output
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean: 
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re