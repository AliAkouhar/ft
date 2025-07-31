CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror

NAME = irc_serv
OBJ_DIR = obj

SRC = src/main.cpp \
      src/Server.cpp \
      src/Client.cpp \
      src/Channel.cpp \
      src/commd/Join.cpp \
      src/commd/Invite.cpp \
      src/commd/Mode.cpp \
      src/commd/Part.cpp \
      src/commd/Privmsg.cpp \
      src/commd/Nick.cpp \
      src/cmd_utils.cpp \
      src/commd/User.cpp \
      src/commd/Pass.cpp \
      src/commd/Topic.cpp \
      src/commd/Kick.cpp \
      src/commd/Quit.cpp \
      src/commd/mode_utils.cpp

# Convert src paths to obj paths
OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean: 
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
