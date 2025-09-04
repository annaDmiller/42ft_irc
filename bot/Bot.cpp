#include "Bot.hpp"

typedef std::string (Bot::*FuncBotType)();
typedef void (Bot::*FuncType)(std::string&, std::istringstream&);

bool Bot::_signalReceived = false;

void Bot::signalHandler(int signum)
{
    (void)signum;
    Bot::_signalReceived = true;
    return ;
}

Bot::Bot(): _port(-1), _fd(-1), _password(""),
			_nickname(BOT_NICK), _username(BOT_USER), _realname(BOT_REAL),
			_recvBuffer(""), _sendBuffer("")
{
    return ;
}

Bot::~Bot()
{
	if (_fd != -1)
		close(_fd);
}

void Bot::initBot(char* port_num, char* password)
{
    if (!port_num || !password)
        return ;
    this->_port = atoi(port_num);
    this->_password = password;
    this->initSignal();
    this->initSocket();
	this->initConnection();
    return ;
}

void Bot::initSocket()
{
	int	botSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (botSocket == -1)
		throw (std::runtime_error("Socket error"));

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->_port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(botSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) <= -1)
	{
		close(botSocket);
		throw (std::runtime_error("Connect error"));
	}
	this->_fd = botSocket;
	return ;
}

void Bot::initConnection()
{
	std::string	botData;

	botData = std::string("PASS ") + this->_password + TERMIN;
	botData += std::string("NICK ") + this->_nickname + TERMIN;
	botData += std::string("USER ") + this->_username + " 0 * :" + this->_realname + TERMIN;
	send(this->_fd, botData.c_str(), botData.length(), MSG_NOSIGNAL);
	return ;
}

void Bot::initSignal()
{
   struct sigaction	sa, sb;

        sa.sa_handler = &Bot::signalHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sb.sa_handler = SIG_IGN;
        sigemptyset(&sb.sa_mask);
        sb.sa_flags = 0;
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        if (sigaction(SIGTERM, &sa, NULL) != 0)
            throw (std::runtime_error("sigaction error"));
        if (sigaction(SIGINT, &sa, NULL) != 0)
            throw (std::runtime_error("sigaction error"));
        if (sigaction(SIGPIPE, &sb, NULL) != 0)
            throw (std::runtime_error("sigaction error"));           
    return ;
}


void Bot::runBot()
{
    while (!Bot::_signalReceived)
    {
        if (Bot::_signalReceived)
            break ;
		receiveNewData();
    }
    close(this->_fd);
    return ;
}

void Bot::receiveNewData()
{
    char buffer[1024];
    ssize_t bytes = 0;
    size_t pos_end = 0;
    std::string raw_cmd, remain_line, message;

    memset(buffer, 0, sizeof(buffer));
 
    bytes = recv(this->_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
		close(this->_fd);
        throw (std::runtime_error("Server disconnected.")); 
	}
    else
    {
        buffer[bytes] = '\0';
        std::cout << "[DEBUG] ";
        std::cout << "Server sent data:" << std::endl;

        std::string str(buffer);
        this->appendRecvBuffer(buffer);
		remain_line = this->getRecvBuffer();
        while ((pos_end = remain_line.find(TERMIN)) != std::string::npos)
        {
            raw_cmd = this->getRecvBuffer().substr(0, pos_end);
            if (raw_cmd.size() > (510))
                raw_cmd = raw_cmd.substr(0, 510);
            this->splitRecvBuffer(0, pos_end + 2);
 			remain_line = this->getRecvBuffer();
	
            std::cout << raw_cmd << std::endl;
            this->handleCommand(raw_cmd);
        }
		this->sendReply();
    }
    return ;
}

void Bot::sendReply()
{
	std::string message;
	size_t pos_end = 0;

    if (this->getSendBuffer().length() > 0)
    {
        message = this->getSendBuffer();
        if ((pos_end = message.find(TERMIN)) != std::string::npos)
        {
            message = message.substr(0, pos_end + 2);
            this->splitSendBuffer(0, pos_end + 2);
            send(this->getFD(), message.c_str(), message.size(), 0);
        }
    }
    return ;
}


void Bot::handleCommand(std::string& raw_cmd)
{
    std::string cmd, clientPrefix, nickname;
	size_t	pos;
    const std::map<std::string, FuncType> allowed_cmds = this->getMapCmdFunc();

    if (raw_cmd.empty()) {
        return ;
    }
	if (raw_cmd[0] == ' ')
		raw_cmd = raw_cmd.substr(1);
	if (raw_cmd[0] == ':')
		raw_cmd = raw_cmd.substr(1);

    std::istringstream line(raw_cmd);
    line >> clientPrefix;

	if ((pos = clientPrefix.find("!")) == std::string::npos)
		return ;
	nickname = clientPrefix.substr(0, pos);

	line >> cmd;
    cmd = toUpperString(cmd);

    std::map<std::string, FuncType>::const_iterator it = allowed_cmds.find(cmd);
    if (it != allowed_cmds.end())
        (this->*it->second)(nickname, line);

    return ;
}

const std::map<std::string, FuncType>& Bot::getMapCmdFunc()
{
    static std::map<std::string, FuncType> func_map;
    if (func_map.empty())
    {
        func_map[INVITE] = &Bot::handleInvite;
        func_map[PRIVMSG] = &Bot::handlePrivateMessage;
    }
    return (func_map);
}

const std::map<std::string, FuncBotType>& Bot::getMapBotCmdFunc()
{
    static std::map<std::string, FuncBotType> func_map;
    if (func_map.empty())
    {
        func_map[HELP] = &Bot::handleHelp;
		func_map[ROLL] = &Bot::handleRoll;
        func_map[TIME] = &Bot::handleTime;
    }
    return (func_map);
}


std::string Bot::toUpperString(std::string str)
{
    std::string new_str(str);

    for (size_t i = 0; i < str.size(); i++)
        new_str[i] = std::toupper(str[i]);
    return (new_str);
}

void Bot::appendRecvBuffer(std::string buff)
{
    this->_recvBuffer.append(buff);
    return ;
}

void Bot::appendSendBuffer(std::string buff)
{
    this->_sendBuffer.append(buff);
    return ;
}

void Bot::splitRecvBuffer(size_t start, size_t end)
{
    this->_recvBuffer.erase(start, end);
    return ;
}

void Bot::splitSendBuffer(size_t start, size_t end)
{
    this->_sendBuffer.erase(start, end);
    return ;
}

int Bot::getFD() const
{
    return (this->_fd);
}

std::string Bot::getNick() const
{
    return (this->_nickname);
}

std::string Bot::getUser() const
{
    return (this->_username);
}

std::string Bot::getReal() const
{
    return (this->_realname);
}

std::string Bot::getRecvBuffer() const
{
    return (this->_recvBuffer);
}

std::string Bot::getSendBuffer() const
{
    return (this->_sendBuffer);
}

void Bot::handleInvite(std::string& nickname, std::istringstream& args)
{
    std::string recipient, channel_name, message;

	(void)nickname;
    args >> recipient >> channel_name;

    if (recipient.empty() || channel_name.empty())
        return ;

    message = "JOIN " + channel_name + TERMIN;
    this->appendSendBuffer(message);
    return ;
}

void Bot::handlePrivateMessage(std::string& nickname, std::istringstream& args)
{
    std::string recipient, recv_msg, send_msg, reply;

    args >> recipient >> recv_msg;

    if (recipient.empty() || recv_msg.empty()) {
        return ;
	}
	if (recv_msg[0] == ' ')
		recv_msg = recv_msg.substr(1);
	if (recv_msg[0] == ':')
		recv_msg = recv_msg.substr(1);

	reply = this->handleBotCommand(recv_msg);
	if (reply.empty())
		return ;

    if (recipient[0] == '#') {
        send_msg = "PRIVMSG " + recipient + " :" + reply + TERMIN;
    }
    else {
        send_msg = "PRIVMSG " + nickname + " :" + reply + TERMIN;
    }
	this->appendSendBuffer(send_msg);
    return ;
}

std::string Bot::handleBotCommand(std::string& raw_cmd)
{
    std::string cmd, clientPrefix, nickname, send_msg, reply = "";
    const std::map<std::string, FuncBotType> allowed_cmds = this->getMapBotCmdFunc();

    if (raw_cmd.empty()) {
        return ("");
    }

    std::map<std::string, FuncBotType>::const_iterator it = allowed_cmds.find(raw_cmd);

    if (it != allowed_cmds.end()) {
        reply = (this->*it->second)();
	}
    return (reply);
}

std::string Bot::handleHelp()
{
	std::string commands;

	commands = "Bot Commands: ";
	commands += "!help (List of commands) | ";
	commands += "!roll (Roll a die) | ";
	commands += "!time: (What time is it?)";
	return (commands);
}

std::string Bot::handleRoll()
{
	std::string message;
	std::stringstream ss;
	size_t	number = std::rand() % 6 + 1;

	ss << number;
	message = "The roll result is: " + ss.str();
	return (message);
}

std::string Bot::handleTime()
{
	char currentTime[50];
	time_t setTime = time(NULL);
	struct tm dateTime = *localtime(&setTime);
	std::string message;

	strftime(currentTime, 50, "%m/%d/%Y - %H:%M:%S", &dateTime);
	message = std::string("Date and Time: ") + currentTime;
	return (message);
}