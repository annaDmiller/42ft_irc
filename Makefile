CC = c++

CC_FLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -I./includes

NAME = ircserv

SOURCES =	./srcs/main.cpp \
			./srcs/Channel.cpp \
			./srcs/Client.cpp \
			./srcs/Server.cpp \
			./srcs/utils.cpp \
			./srcs/commands/initialCMDs.cpp \
			./srcs/commands/join.cpp \
			./srcs/commands/privmsg.cpp \
			./srcs/commands/quit.cpp

OBJECTS = $(SOURCES:.cpp=.o)

RM = rm

RM_FLAGS = -f

all : $(NAME)

$(NAME): $(OBJECTS)
		$(CC) $(CC_FLAGS) -o $(NAME) $(OBJECTS)

%.o:%.cpp
		$(CC) $(CC_FLAGS) -c $< -o $@

clean:
		$(RM) $(RM_FLAGS) $(OBJECTS)

fclean: clean
		$(RM) $(RM_FLAGS) $(NAME)

re: fclean all

.PHONY: all, clean, fclean, re