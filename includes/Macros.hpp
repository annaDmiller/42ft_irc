#pragma once

# define SOCKMAXCONN 10 //-> maximum number of connections to socket at a time
# define TERMIN "\r\n" //-> line termination in IRC (based on RFC 2812)

//Numeric replies for errors
# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTERED 462
# define ERR_NONICKNAMEGIVEN 431
# define ERR_NICKNAMEINUSE 433
# define ERR_UNAVAILRESOURCE 437
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKCOLLISION 436
# define ERR_RESTRICTED 484