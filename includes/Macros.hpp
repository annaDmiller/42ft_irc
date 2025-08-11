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
# define MAXLINELENGTH 512
# define OPER_NAME "ADMIN"
# define OPER_PASSWORD "ADMIN"

//Command names
# define USER "USER" //DONE
# define PASS "PASS" //DONE
# define NICK "NICK" //DONE
# define JOIN "JOIN" //DONE
# define QUIT "QUIT" //DONE
# define PRIVMSG "PRIVMSG" //DONE
# define PING "PING" //DONE (Client -> Server)
# define PONG "PONG" //DONE (Server -> Client)
# define NOTICE "NOTICE" //DONE
# define PART "PART" //DONE
# define NAMES "NAMES" //DONE
# define KICK "KICK" //DONE
# define INVITE "INVITE"
# define TOPIC "TOPIC" //DONE
# define MODE "MODE" //DONE
# define OPER "OPER" //DONE

//Macros: Numeric replies for errors
# define ERR_NEEDMOREPARAMS(nick, cmd) (std::string(":") + HOST + " 461 " + nick + " " + cmd + " :Not enough parameters" + TERMIN)
# define ERR_ALREADYREGISTERED(nick) (std::string(":") + HOST + " 462 " + nick + " :Unauthorized command (already registered)" + TERMIN)
# define ERR_NONICKNAMEGIVEN(nick) (std::string(":") + HOST + " 431 " + nick + " :No nickname given" + TERMIN)
# define ERR_NICKNAMEINUSE(nick, new_nick) (std::string(":") + HOST + " 433 " + nick + " " + new_nick + " :Nickname is already in use" + TERMIN)
# define ERR_ERRONEUSNICKNAME(nick, new_nick) (std::string(":") + HOST + " 432 " + nick + " " + new_nick + " :Erroneous nickname" + TERMIN)
# define ERR_PASSWDMISMATCH(nick) (std::string(":") + HOST + " 464 " + nick + " :Password incorrect" + TERMIN)
# define ERR_NOTREGISTERED(nick) (std::string(":") + HOST + " 451 " + nick + " :You have not registered" + TERMIN)
# define ERR_INVITEONLYCHAN(nick, channel_name) (std::string(":") + HOST + " 473 " + nick + " " + channel_name + " :Cannot join channel (+i) - you must be invited" + TERMIN)
# define ERR_CHANNELISFULL(nick, channel_name) (std::string(":") + HOST + " 471 " + nick + " " + channel_name + " :Cannot join channel (+l) - channel is full, try again later" + TERMIN)
# define ERR_BADCHANNELKEY(nick, channel_name) (std::string(":") + HOST + " 475 " + nick + " " + channel_name + " :Cannot join channel (+k) - bad key" + TERMIN)
# define ERR_BADCHANMASK(nick, channel_name) (std::string(":") + HOST + " 476 " + nick + " " + channel_name + " :Bad Channel Mask" + TERMIN)
# define ERR_TOOMANYCHANNELS(nick, channel_name) (std::string(":") + HOST + " 405 " + nick + " " + channel_name + " :You have joined too many channels" + TERMIN)
# define ERR_NORECIPIENT(nick, cmd) (std::string(":") + HOST + " 411 " + nick + " :No recepient given (" + cmd + ")" + TERMIN)
# define ERR_NOTEXTTOSEND(nick) (std::string(":") + HOST + " 412 " + nick + " :No text to send" + TERMIN)
# define ERR_NOSUCHNICK(nick, target) (std::string(":") + HOST + " 401 " + nick + " " + target + " :No such nick/channel" + TERMIN)
# define ERR_CANNOTSENDTOCHAN(nick, channel_name) (std::string(":") + HOST + " 404 " + nick + " " + channel_name + " :Cannot send to channel" + TERMIN)
# define ERR_UNKNOWNCOMMAND(nick, cmd) (std::string(":") + HOST + " 421 " + nick + " " + cmd + " :Unknown command" + TERMIN)
# define ERR_NOORIGIN(nick, cmd) (std::string(":") + HOST + " 409 " + nick + " " + cmd + " :No origin specified" + TERMIN)
# define ERR_NOTONCHANNEL(nick, channel_name) (std::string(":") + HOST + " 442 " + nick + " " + channel_name + " :You're not on thata channel" + TERMIN)
# define ERR_NOSUCHCHANNEL(nick, channel_name) (std::string(":") + HOST + " 403 " + nick + " " + channel_name + " :No such channel" + TERMIN)
# define ERR_USERNOTINCHANNEL(nick, channel_name, target) (std::string(":") + HOST + " 441 " + nick + " " + channel_name + " " + target + " :No such channel" + TERMIN)
# define ERR_KEYSET(nick, channel_name) (std::string(":") + HOST + " 467 " + nick + " " + channel_name + " :Channel key already set" + TERMIN)
# define ERR_CHANOPRIVSNEEDED(nick, channel_name) (std::string(":") + HOST + " 482 " + nick + " " + channel_name + " :You're not channel operator" + TERMIN)
# define ERR_UNKNOWNMODE(nick, mode_char) (std::string(":") + HOST + " 472 " + nick + " " + mode_char + " :No such channel" + TERMIN)
# define ERR_USERONCHANNEL(nick, target_nick, channel_name) (std::string(":") + HOST + " 443 " + nick + " " + target_nick + " " + channel_name + " :is already on channel" + TERMIN)

//Macros: Replies to the client
# define RPL_WELCOME(nick) (std::string(":") + HOST + " 001 " + nick + " :Welcome to the Internet Relay Network " + nick + TERMIN)
# define RPL_YOURHOST(nick, servername, version) (std::string(":") + HOST + " 002 " + nick + " :Your host is " + servername + ", running version " + version + TERMIN)
# define RPL_CREATED(nick, date) (std::string(":") + HOST + " 003 " + nick + " :This server was created " + date + TERMIN)
# define RPL_MYINFO(nick, servername, version, user_modes, channel_modes) (std::string(":") + HOST + " 004 " + nick + " " + servername + " " + version + " " + user_modes + " " + channel_modes + TERMIN)
# define RPL_TOPIC(nick, channel_name, topic) (std::string(":") + HOST + " 332 " + nick + " " + channel_name + " :" + topic + TERMIN)
# define RPL_TOPICWHOWHEN(nick, channel_name, who, when) (std::string(":") + HOST + " 333 " + nick + " " + channel_name + " " + who + " " + when + TERMIN)
# define RPL_NOTOPIC(nick, channel_name) (std::string(":") + HOST + " 333 " + nick + " " + channel_name + " :No topic is set" + TERMIN)
# define RPL_NAMREPLY(nick, channel_name) (std::string(":") + HOST + " 353 " + nick + " = " + channel_name + " :")
# define RPL_ENDOFNAMES(nick, channel_name) (std::string(":") + HOST + " 366 " + nick + " " + channel_name + " :End of /NAMES list" + TERMIN)
# define RPL_INVITELIST 346 //NEED
# define RPL_ENDOFINVITELIST 347 //NEED
# define RPL_INVITING(nick, channel_name, target_nick) (std::string(":") + HOST + " 341 " + nick + " " + target_nick + " " + channel_name + TERMIN)
# define RPL_UNIQOPIS 325 //NEED
# define RPL_CHANNELMODEIS(nick, channel_name, mode, mode_params) (std::string(":") + HOST + " 324 " + nick + " " + channel_name + " " + mode + " " + mode_params + TERMIN)
# define RPL_YOUREOPER(nick) (std::string(":") + HOST + " 381 " + nick + " :You are now an IRC operator" + TERMIN)

//Macros: Additional messages
# define QUIT_MESS(host, message) ("ERROR :Closing Link: " + host + " (" + message + ")" + TERMIN)
# define DISCONNECTION_MESS ("Connection closed by foreign host.\r\n")
# define DISCONNECTION_SERVER_MESS ("Connection closed by the server.\r\n")