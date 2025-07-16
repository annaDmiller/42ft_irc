#pragma once

# define SOCKMAXCONN 10 //-> maximum number of connections to socket at a time
# define TERMIN "\r\n" //-> line termination in IRC (based on RFC 2812)
# define SERVERNAME "FT_IRC"
# define VERSION "ver.1.0"
# define CREATEDDATE "" //NEED: to add created date value
# define USERMODES "-"
# define CHANNELMODES "iklot"
# define HOST "irc.local"
# define MAXJOINEDCHANNELS 10
# define MAXLINELENGTH (512 - 2)

//Command names
# define USER "USER" //DONE - ALMOST
# define PASS "PASS" //DONE
# define NICK "NICK" //DONE - ALMOST
# define JOIN "JOIN" //DONE - ALMOST
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
# define ERR_NEEDMOREPARAMS(nick, cmd) (std::string(":") + HOST + " 461 " + nick + " " + cmd + " :Not enough parameters" + TERMIN)
# define ERR_ALREADYREGISTERED(nick) (std::string(":") + HOST + " 462 " + nick + " :Unauthorized command (already registered)" + TERMIN)
# define ERR_NONICKNAMEGIVEN(nick) (std::string(":") + HOST + " 431 " + nick + " :No nickname given" + TERMIN)
# define ERR_NICKNAMEINUSE(nick, new_nick) (std::string(":") + HOST + " 433 " + nick + " " + new_nick + " :Nickname is already in use" + TERMIN)
# define ERR_ERRONEUSNICKNAME(nick, new_nick) (std::string(":") + HOST + " 432 " + nick + " " + new_nick + " :Erroneous nickname" + TERMIN)
# define ERR_PASSWDMISMATCH(nick) (std::string(":") + HOST + " 464 " + nick + " :Password incorrect" + TERMIN)
# define ERR_NOTREGISTERED(nick) (std::string(":") + HOST + " 451 " + nick + " :You have not registered" + TERMIN)
# define ERR_INVITEONLYCHAN(nick, channel_name) (std::string(":") + HOST + " 473 " + nick + " " + channel_name + " :Cannot join channel (+i)" + TERMIN)
# define ERR_CHANNELISFULL(nick, channel_name) (std::string(":") + HOST + " 471 " + nick + " " + channel_name + " :Cannot join channel (+l)" + TERMIN)
# define ERR_BADCHANNELKEY(nick, channel_name) (std::string(":") + HOST + " 475 " + nick + " " + channel_name + " :Cannot join channel (+k)" + TERMIN)
# define ERR_BADCHANMASK(nick, channel_name) (std::string(":") + HOST + " 476 " + nick + " " + channel_name + " :Bad Channel Mask" + TERMIN)
# define ERR_TOOMANYCHANNELS(nick, channel_name) (std::string(":") + HOST + " 405 " + nick + " " + channel_name + " :You have joined too many channels" + TERMIN)
# define ERR_NORECIPIENT 411
# define ERR_NOTEXTTOSEND 412
# define ERR_NOTOPLEVEL 413
# define ERR_WILDTOPLEVEL 414
# define ERR_NOSUCHNICK 401
# define ERR_CANNOTSENDTOCHAN 404

//Macros: Replies to the client
# define RPL_WELCOME(nick, user, host) (std::string(":") + HOST + " 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + TERMIN)
# define RPL_YOURHOST(nick, servername, version) (std::string(":") + HOST + " 002 " + nick + " :Your host is " + servername + ", running version " + version + TERMIN)
# define RPL_CREATED(nick, date) (std::string(":") + HOST + " 003 " + nick + " :This server was created " + date + TERMIN)
# define RPL_MYINFO(nick, servername, version, user_modes, channel_modes) (std::string(":") + HOST + " 004 " + nick + " " + servername + " " + version + " " + user_modes + " " + channel_modes + TERMIN)
# define RPL_TOPIC(nick, channel_name, topic) (std::string(":") + HOST + " 332 " + nick + " " + channel_name + " :" + topic + TERMIN)
# define RPL_NAMREPLY(nick, channel_name) (std::string(":") + HOST + " 353 " + nick + " " + channel_name + " :")
# define RPL_ENDOFNAMES(nick, channel_name) (std::string(":") + HOST + " 366 " + nick + " " + channel_name + " :End of NAMES list" + TERMIN)