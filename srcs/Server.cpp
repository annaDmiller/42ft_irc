#include "Server.hpp"

typedef void (Server::*FuncType)(Client&, std::istringstream&);

bool Server::_signalReceived = false;

void Server::signalHandler(int signum)
{
    (void)signum;
    Server::_signalReceived = true;
    return ;
}

void Server::initSignal()
{
   struct sigaction	sa, sb;

        sa.sa_handler = &Server::signalHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sb.sa_handler = SIG_IGN;
        sigemptyset(&sb.sa_mask);
        sb.sa_flags = 0;
        if (sigaction(SIGTERM, &sa, NULL) != 0)
            throw (std::runtime_error("sigaction error"));
        if (sigaction(SIGINT, &sa, NULL) != 0)
            throw (std::runtime_error("sigaction error"));
        if (sigaction(SIGPIPE, &sb, NULL) != 0)
            throw (std::runtime_error("sigaction error"));           
    return ;
}

Server::Server() : _port(0), _sockfd(-1), _password("")
{
    return ;
}

Server::~Server()
{
    return ;
}

Server::Server(const Server& other)
{
    (void)other;
    return ;
}

Server& Server::operator=(const Server& other)
{
    (void)other;
    return (*this);
}

void Server::initServer(char* port_num, char* password)
{
    if (!port_num || !password)
        return ;
    this->_port = atoi(port_num);
    this->_password = password;
    this->initSignal();
    this->createServSocket(port_num);

    return ;
}

void Server::createServSocket(char* port_num)
{
    struct addrinfo temp, *servinfo, *it;
    int yes = 1;
    struct pollfd new_poll;

    memset(&temp, 0, sizeof(temp));
    temp.ai_family = AF_INET;
    temp.ai_socktype = SOCK_STREAM;
    temp.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port_num, &temp, &servinfo) == -1)
        throw (std::runtime_error("Failed to get network address"));
    
    for (it = servinfo; it != NULL; it = it->ai_next)
    {
        this->_sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (this->_sockfd == -1)
            continue ;

        if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            freeaddrinfo(servinfo);
            throw (std::runtime_error("Failed to set option SO_REUSEADDR to the socket"));
        }

        if (fcntl(this->_sockfd, F_SETFL, O_NONBLOCK) == -1)
        {
            freeaddrinfo(servinfo);
            throw (std::runtime_error("Failed to set option O_NONBLOCK to the socket"));
        }

        if (bind(this->_sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        {
            close(this->_sockfd);
            this->_sockfd = -1;
            continue ;
        }

        break ;
    }

    freeaddrinfo(servinfo); 

    if (it == NULL) 
        throw (std::runtime_error("Failed to bind socket"));
    
    if (listen(this->_sockfd, SOCKMAXCONN) == -1)
    {
        close(this->_sockfd);
        throw (std::runtime_error("Failed to start listening"));
    }

    new_poll.fd = this->_sockfd;
    new_poll.events = POLLIN;
    new_poll.revents = 0;
    this->_pollfds.push_back(new_poll);

    return ;
}

void Server::runServer()
{
    std::string message = ":irc.local :The connection is closed: the server is stopped.\r\n";
    while (1)
    {
        if (poll(&(this->_pollfds[0]), this->_pollfds.size(), -1) == -1 && Server::_signalReceived == false)
            throw (std::runtime_error("Failed to use poll() function"));
        
        if (Server::_signalReceived)
        {
            for (size_t ind = 0; ind < this->_pollfds.size(); ind++)
            {
                if (this->_pollfds[ind].fd != this->_sockfd
                        && this->_pollfds[ind].revents & POLLOUT)
                {
                    Client& client = this->_clients[this->_pollfds[ind].fd];
                    client.appendSendBuffer(message);
                    this->sendReply(client.getFD());
                }
            }
            break ;
        }

        for (size_t ind = 0; ind < this->_pollfds.size(); ind++)
        {
            if (this->_pollfds[ind].revents & (POLLHUP | POLLERR))
            {
                this->clearClient(this->_pollfds[ind].fd);
                ind--;
                continue;
            }

            else if (this->_pollfds[ind].revents & POLLIN)
            {
                if (this->_pollfds[ind].fd == this->_sockfd)
                    this->acceptNewClient();
                else
                    this->receiveNewData(this->_pollfds[ind].fd);
            }

            else if (this->_pollfds[ind].fd != this->_sockfd
                    && this->_pollfds[ind].revents & POLLOUT)
			    sendReply(this->_pollfds[ind].fd);
        }
    }

    this->closeFDs();
    return ;
}

void Server::acceptNewClient()
{
    Client new_client;
    struct sockaddr_in client_addr;
    struct pollfd new_poll;
    socklen_t len_addr = sizeof(client_addr);
	int client_fd = -1, yes = 1;
    std::string init_mess = ":irc.local :NOTICE AUTH :Welcome!\r\n";
    std::string err_message = ":irc.local :Impossible to establish connection (too many connected clients). Try later.\r\n";

    client_fd = accept(this->_sockfd, reinterpret_cast<sockaddr*>(&client_addr), &len_addr);
    if (client_fd == -1)
    {
        std::cerr << "Failed accept() of new client" << std::endl;
        return ;
    }

    if (this->_clients.size() == SOCKMAXCONN)
    {
        send(client_fd, err_message.c_str(), err_message.size(), 0);
        close(client_fd);
        return ;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Failed O_NONBLOCK option for new client" << std::endl;
        close(client_fd);
        return ;
    }

	if (setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1)
	{
		close(client_fd);
		throw (std::runtime_error("Failed to set option SO_KEEPALIVE to the socket"));
	}

    new_poll.fd = client_fd;
    new_poll.events = POLLIN | POLLOUT;
    new_poll.revents = 0;
    this->_pollfds.push_back(new_poll);

    new_client.setFD(client_fd);
    char *ip = inet_ntoa(client_addr.sin_addr);
    new_client.setIPAddr(ip);
    new_client.appendSendBuffer(init_mess);
    this->_clients[client_fd] = new_client;

    std::cout << "[DEBUG] ";
    std::cout << "New client " << new_client.getFD() << " is accepted." << std::endl;
    return ;
}

void Server::receiveNewData(int& clientFD)
{
    char buffer[1024];
    ssize_t bytes = 0;
    size_t pos_end = 0, termin_len = 0;
    std::string raw_cmd, remain_line;

    memset(buffer, 0, sizeof(buffer));

    bytes = recv(clientFD, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        std::cout << "[DEBUG] ";
        std::cout << "Client " << clientFD << " disconnected." << std::endl;
        this->clearClient(clientFD);
    }
    else
    {
        buffer[bytes] = '\0';
        std::cout << "[DEBUG] ";
        std::cout << "Client " << clientFD << " sent data." << std::endl;

        std::string str(buffer);
        std::map<int, Client>::const_iterator it = _clients.find(clientFD);
        if (it == _clients.end())
            return ;

        Client& our_client = this->_clients[clientFD];
        our_client.appendBuffer(buffer);
        remain_line = our_client.getBuffer();
        while ((pos_end = remain_line.find(TERMIN)) != std::string::npos || (pos_end = remain_line.find_first_of(TERMIN)) != std::string::npos)
        {
            if (remain_line.find(TERMIN) != std::string::npos)
                termin_len = 2;
            else if (remain_line.find_first_of(TERMIN) != std::string::npos)
                termin_len = 1;

            raw_cmd = our_client.getBuffer().substr(0, pos_end);
            if (raw_cmd.size() > (512 - termin_len))
                raw_cmd = raw_cmd.substr(0, 512 - termin_len);
            our_client.splitBuffer(0, pos_end + termin_len);
            remain_line = our_client.getBuffer();

            this->handleCommands(our_client, raw_cmd);
        }
    }

    return ;
}

void Server::handleCommands(Client& client, std::string& raw_cmd)
{
    std::istringstream line(raw_cmd);
    std::string cmd, err_message;
    const std::map<std::string, FuncType> allowed_cmds = this->getMapCmdFunc();

    if (raw_cmd.empty())
        return ;

    line >> cmd;

    cmd = toUpperString(cmd);

    std::map<std::string, FuncType>::const_iterator it = allowed_cmds.find(cmd);

    if (!client.isRegistered())
    {
        if (it == allowed_cmds.end())
            return ;
        this->handleInitCommands(client, cmd, line);
        return ;
    }

    if (it != allowed_cmds.end()) {
        (this->*it->second)(client, line);
	}
    else
    {
        err_message = ERR_UNKNOWNCOMMAND(client.getNick(), cmd);
		client.appendSendBuffer(err_message);
    }

    return ;
}

int Server::findUserbyNickname(const std::string& nick) const
{
    for (std::map<int, Client>::const_iterator it = this->_clients.begin();
            it != this->_clients.end(); it++)
    {
        if (it->second.getNick() == nick)
            return (it->first);
    }
    return (-1);
}

void Server::sendMessageToUser(Client& client, const int& target_fd,
        const std::string& target_name, const std::string& message,
        const std::string& cmd)
{
    std::string body, full_message = std::string(":") + client.getPrefix() + " " + cmd + " ";
    if (!target_name.empty())
        full_message += target_name + " ";
    
    const size_t symb_left = MAXLINELENGTH - full_message.length() - 3;
    
    if (!message.empty())
    {
        body = std::string(":");
        if (message.length() > symb_left)
            body += message.substr(0, symb_left);
        else
            body += message;
    }
    full_message += body + TERMIN;

	std::map<int, Client>::iterator it = _clients.find(target_fd);
	if (it == _clients.end()) {
		return ;
    }
	Client& target_client = this->_clients[it->first];
	target_client.appendSendBuffer(full_message);	

    return ;
}

void Server::closeFDs()
{
    for (std::map<int, Client>::iterator it = this->_clients.begin();
        it != this->_clients.end(); it++)
        close(it->first);
    
    if (this->_sockfd != -1)
        close(this->_sockfd);

    return ;
}

void Server::clearClient(const int& client_fd)
{
	int	fdToClose = client_fd;

    if (this->_clients[client_fd].isRegistered())
        this->_clients[client_fd].leaveAllChannels(*this);
       
    this->_clients.erase(client_fd);

    for (std::vector<struct pollfd>::iterator it = this->_pollfds.begin();
            it != this->_pollfds.end(); it++)
    {
        if (it->fd == client_fd)
        {
            this->_pollfds.erase(it);
            break ;
        }
    }
    close(fdToClose);
    return ;
}

void Server::deleteChannel(const std::string& channel_name)
{
    this->_availableChannels.erase(channel_name);
    return ;
}

const std::map<std::string, FuncType>& Server::getMapCmdFunc()
{
    static std::map<std::string, FuncType> func_map;
    if (func_map.empty())
    {
        func_map[USER] = &Server::handleUsernameCmd;
        func_map[PASS] = &Server::handlePasswordCmd;
        func_map[NICK] = &Server::handleNicknameCmd;
        func_map[INVITE] = &Server::handleInviteCmd;
        func_map[JOIN] = &Server::handleJoinCmd;
        func_map[KICK] = &Server::handleKickCmd;
        func_map[MODE] = &Server::handleModeCmd;
        func_map[NAMES] = &Server::handleNamesCmd;
        func_map[NOTICE] = &Server::handleNoticeCmd;
        func_map[OPER] = &Server::handleOperCmd;
        func_map[PART] = &Server::handlePartCmd;
        func_map[PING] = &Server::handlePingCmd;
        func_map[PRIVMSG] = &Server::handlePrivmsgCmd;
        func_map[QUIT] = &Server::handleQuitCmd;
        func_map[TOPIC] = &Server::handleTopicCmd;
        func_map[CAP] = &Server::handleCapCmd;
    }
    return (func_map);
}

void Server::sendReply(int clientFD)
{
	std::string message;
	size_t pos_end = 0;
	std::map<int, Client>::iterator it = _clients.find(clientFD);

	if (it == _clients.end()) {
		return ;
	}

	Client& our_client = this->_clients[clientFD];
    message = our_client.getSendBuffer();

    if (message.empty()) {
            return ;
	}

	if ((pos_end = message.find(TERMIN)) != std::string::npos)
	{
		message = message.substr(0, pos_end + 2);
		our_client.splitSendBuffer(0, pos_end + 2);
		send(clientFD, message.c_str(), message.size(), 0);
	}
    return ;
}