CC = c++

CC_FLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -I./includes
CC_BOT_FLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -I./bot/includes

NAME = ircserv
BOT = ircbot

SERV_DIR = ./srcs/
BOT_DIR = ./bot/

SOURCES =	./srcs/main.cpp \
			./srcs/Channel.cpp \
			./srcs/Client.cpp \
			./srcs/Server.cpp \
			./srcs/utils.cpp \
			./srcs/commands/initialCMDs.cpp \
			./srcs/commands/join.cpp \
			./srcs/commands/privmsg.cpp \
			./srcs/commands/quit.cpp \
			./srcs/commands/names.cpp \
			./srcs/commands/part.cpp \
			./srcs/commands/pingPong.cpp \
			./srcs/commands/notice.cpp \
			./srcs/commands/nick.cpp \
			./srcs/commands/mode.cpp \
			./srcs/commands/topic.cpp \
			./srcs/commands/kick.cpp \
			./srcs/commands/invite.cpp \
			./srcs/commands/oper.cpp

BOT_SOURCES =	$(BOT_DIR)main.cpp \
				$(BOT_DIR)Bot.cpp

OBJECTS = $(SOURCES:.cpp=.o)
BOT_OBJECTS = $(BOT_SOURCES:.cpp=.o)

RM = rm

RM_FLAGS = -f

all : $(NAME) $(BOT)

$(NAME): $(OBJECTS)
		$(CC) $(CC_FLAGS) -o $(NAME) $(OBJECTS)

$(BOT): $(BOT_OBJECTS)
		$(CC) $(CC_BOT_FLAGS) -o $(BOT) $(BOT_OBJECTS)

$(SERV_DIR)%.o:$(SERV_DIR)%.cpp includes/Channel.hpp includes/Client.hpp includes/Macros.hpp includes/Server.hpp
		$(CC) $(CC_FLAGS) -c $< -o $@

$(BOT_DIR)%.o:$(BOT_DIR)%.cpp $(BOT_DIR)/includes/Bot.hpp
		$(CC) $(CC_BOT_FLAGS) -c $< -o $@

clean:
		$(RM) $(RM_FLAGS) $(OBJECTS)
		$(RM) $(RM_FLAGS) $(BOT_OBJECTS)

fclean: clean
		$(RM) $(RM_FLAGS) $(NAME)
		$(RM) $(RM_FLAGS) $(BOT)

re: fclean all

.PHONY: all, clean, fclean, re