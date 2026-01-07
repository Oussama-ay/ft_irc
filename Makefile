NAME = ft_irc
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = src/main.cpp src/Server.cpp src/Client.cpp src/Parser.cpp src/Channel.cpp \
		src/execute.cpp src/Server_utils.cpp \
		src/commands/Join.cpp src/commands/Pass.cpp src/commands/Nick.cpp \
		src/commands/User.cpp  src/commands/Ping.cpp src/commands/Cap.cpp \
		src/commands/Privmsg.cpp src/commands/Quit.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
