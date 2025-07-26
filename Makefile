CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror
SRC = src/main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp \
	src/commd/Join.cpp src/commd/Invite.cpp src/commd/Mode.cpp \
	src/commd/Part.cpp src/commd/Privmsg.cpp src/commd/Nick.cpp src/commd/User.cpp \
	src/commd/Pass.cpp  src/commd/Topic.cpp  \
	src/commd/Kick.cpp src/commd/Quit.cpp

NAME = irc_serv
OBJ_DIR = obj

# Convert src paths to obj paths

OBJ = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	# @echo "Linking objects into executable $(NAME)\n"
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	# @echo "Hey, I removed the flags LOL :> !\n"
	@rm -rf $(OBJ_DIR)

clean: 
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re