CC_SERVER = c++

CC_FLAGS_SERVER = -std=c++98 -g3

NAME_SERVER = server

CC_CLIENT = cc

NAME_CLIENT = client

RM = rm

RM_FLAGS = -f

all :	$(NAME_SERVER) \
		$(NAME_CLIENT)

$(NAME_SERVER):
		$(CC_SERVER) $(CC_FLAGS_SERVER) test_irc_server.cpp -o $(NAME_SERVER)

$(NAME_CLIENT):
		$(CC_CLIENT) test_irc_client.c -o $(NAME_CLIENT)

clean:
		$(RM) $(RM_FLAGS) *.o

fclean: clean
		$(RM) $(RM_FLAGS) $(NAME_CLIENT) $(NAME_SERVER)

re: fclean all

.PHONY: all, clean, fclean, re