#pragma once

# include <iostream>
# include <sstream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <fcntl.h>
# include <unistd.h>
# include <cstring>
# include <string>
# include <vector>
# include <map>
# include <algorithm>
# include <csignal>
# include <limits>
# include <climits>

# define TERMIN "\r\n"
# define BOT_NICK "bot"
# define BOT_USER "bot"
# define BOT_REAL "bot"

# define INVITE "INVITE"
# define PRIVMSG "PRIVMSG"
# define HELP "!help"
# define ROLL "!roll"
# define TIME "!time"

class Bot
{
	private:
        int _port;
        int _fd;
		std::string _password;
        std::string _nickname;
        std::string _username;
        std::string _realname;
		std::string _recvBuffer;
		std::string _sendBuffer;
		static bool _signalReceived;

        Bot(const Bot& other);
        Bot& operator=(const Bot&other);

		void initSocket();
		void initConnection();

		typedef std::string (Bot::*FuncBotType)();
        typedef void (Bot::*FuncType)(std::string&, std::istringstream&);
        static const std::map<std::string, FuncType>& getMapCmdFunc();
		static const std::map<std::string, FuncBotType>& getMapBotCmdFunc();

	public:
        Bot();
        ~Bot();
		
		int getFD() const;
		std::string getNick() const;
		std::string getUser() const;
		std::string getReal() const;
		std::string getRecvBuffer() const;
		std::string getSendBuffer() const;
		std::string toUpperString(std::string str);

		void initBot(char* port_num, char* password);
		void initSignal();
		void runBot();
		void receiveNewData();
		void appendRecvBuffer(std::string buff);
		void appendSendBuffer(std::string buff);
		void splitRecvBuffer(size_t start, size_t end);
		void splitSendBuffer(size_t start, size_t end);
		void handleCommand(std::string& raw_cmd);
		std::string handleBotCommand(std::string& raw_cmd);
		void handleInviteCmd(std::string& nickname, std::istringstream& args);
		void handlePrivmsgCmd(std::string& nickname, std::istringstream& args);
		void sendReply();

		std::string handleHelp();
		std::string handleRoll();
		std::string handleTime();

		static void signalHandler(int signum);
};