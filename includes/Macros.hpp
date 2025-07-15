#pragma once

# define SOCKMAXCONN 10 //-> maximum number of connections to socket at a time
# define TERMIN "\r\n" //-> line termination in IRC (based on RFC 2812)
# define SERVERNAME "FT_IRC"
# define VERSION "ver.1.0"
# define CREATEDDATE ""
# define USERMODES "-"
# define CHANNELMODES "iklot"
# define HOST 1
# define MAXJOINEDCHANNELS 10

//Command names
# define USER "USER"
# define PASS "PASS"
# define NICK "NICK"
# define JOIN "JOIN"
# define QUIT "QUIT"
# define PRIVMSG "PRIVMSG"
# define NOTICE "NOTICE"
# define KICK "KICK"
# define INVITE "INVITE"
# define TOPIC "TOPIC"
# define MODE "MODE"
# define OPER "OPER"
# define PART "PART"

//Macros: Numeric replies for errors
# define ERR_NEEDMOREPARAMS(nick, cmd) (": 461 " + nick + " " + cmd + " :Not enough parameters" + TERMIN)
# define ERR_ALREADYREGISTERED(nick) (": 462 " + nick + " :Unauthorized command (already registered)" + TERMIN)
# define ERR_NONICKNAMEGIVEN(nick) (": 431 " + nick + " :No nickname given" + TERMIN)
# define ERR_NICKNAMEINUSE(nick, new_nick) (": 433 " + nick + " " + new_nick + " :Nickname is already in use" + TERMIN)
# define ERR_ERRONEUSNICKNAME(nick, new_nick) (": 432 " + nick + " " + new_nick + " :Erroneous nickname" + TERMIN)
# define ERR_PASSWDMISMATCH(nick) (": 464 " + nick + " :Password incorrect" + TERMIN)
# define ERR_NOTREGISTERED(nick) (": 451 " + nick + " :You have not registered" + TERMIN)
# define ERR_INVITEONLYCHAN(nick, channel_name) (": 473 " + nick + " " + channel_name + " : Cannot join channel (+i)" + TERMIN)
# define ERR_CHANNELISFULL(nick, channel_name) (": 471 " + nick + " " + channel_name + " :Cannot join channel (+l)" + TERMIN)
# define ERR_BADCHANNELKEY(nick, channel_name) (": 475 " + nick + " " + channel_name + " :Cannot join channel (+k)" + TERMIN)
# define ERR_BADCHANMASK(nick, channel_name) (": 476 " + nick + " " + channel_name + " :Bad Channel Mask" + TERMIN)
# define ERR_TOOMANYCHANNELS(nick, channel_name) (": 405 " + nick + " " + channel_name + " :You have joined too many channels" + TERMIN)

//Macros: Replies to the client
# define RPL_WELCOME(nick, user, host) (": 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + TERMIN)
# define RPL_YOURHOST(nick, servername, version) (": 002 " + nick + " :Your host is " + servername + ", running version " + version + TERMIN)
# define RPL_CREATED(nick, date) (": 003 " + nick + " :This server was created " + date + TERMIN)
# define RPL_MYINFO(nick, servername, version, user_modes, channel_modes) (": 004 " + nick + " " + servername + " " + version + " " + user_modes + " " + channel_modes + TERMIN)
# define RPL_TOPIC 332
# define RPL_NAMREPLY 353