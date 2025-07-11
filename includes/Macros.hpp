#pragma once

# define SOCKMAXCONN 10 //-> maximum number of connections to socket at a time
# define TERMIN "\r\n" //-> line termination in IRC (based on RFC 2812)

//Numeric replies for errors
# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTERED 462
# define ERR_NONICKNAMEGIVEN 431
# define ERR_NICKNAMEINUSE 433
# define ERR_ERRONEUSNICKNAME 432
# define ERR_PASSWDMISMATCH 464
# define ERR_NOTREGISTERED 451

//Replies to the client
# define RPL_WELCOME 001
# define RPL_YOURHOST 002
# define RPL_CREATED 003
# define RPL_MYINFO 004