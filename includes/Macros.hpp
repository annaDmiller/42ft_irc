#pragma once

# define SOCKMAXCONN 10 //-> maximum number of connections to socket at a time
# define TERMIN "\r\n" //-> line termination in IRC (based on RFC 2812)
# define SERVERNAME "FT_IRC"
# define VERSION "ver.1.0"
# define CREATEDDATE ""
# define USERMODES "-"
# define CHANNELMODES "iklot"

//Command names
# define USER "USER"
# define PASS "PASS"
# define NICK "NICK"

//Macros: Numeric replies for errors
# define ERR_NEEDMOREPARAMS(nick, cmd) (": 461 " + nick + " " + cmd + " :Not enough parameters" + TERMIN)
# define ERR_ALREADYREGISTERED(nick) (": 462 " + nick + " :Unauthorized command (already registered)" + TERMIN)
# define ERR_NONICKNAMEGIVEN(nick) (": 431 " + nick + " :No nickname given" + TERMIN)
# define ERR_NICKNAMEINUSE(nick, new_nick) (": 433 " + nick + " " + new_nick + " :Nickname is already in use" + TERMIN)
# define ERR_ERRONEUSNICKNAME(nick, new_nick) (": 432 " + nick + " " + new_nick + " :Erroneous nickname" + TERMIN)
# define ERR_PASSWDMISMATCH(nick) (": 464 " + nick + " :Password incorrect" + TERMIN)
# define ERR_NOTREGISTERED(nick) (": 451 " + nick + " :You have not registered" + TERMIN)

//Macros: Replies to the client
# define RPL_WELCOME(nick, user, host) (": 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + TERMIN)
# define RPL_YOURHOST(nick, servername, version) (": 002 " + nick + " :Your host is " + servername + ", running version " + version + TERMIN)
# define RPL_CREATED(nick, date) (": 003 " + nick + " :This server was created " + date + TERMIN)
# define RPL_MYINFO(nick, servername, version, user_modes, channel_modes) (": 004 " + nick + " " + servername + " " + version + " " + user_modes + " " + channel_modes + TERMIN)