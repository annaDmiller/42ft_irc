#include "Server.hpp"
//NEED: to adapt the code so send() function is used only after check of poll() - create a sending buffer for the client?
//NEED: what is network interface? - it's written in the evaluation sheet
//NEED: to check with nc tool
//NEED: to test with telnet and IRSSI
//NEED: to test when kill nc with half of command sent (how to manage this? And what does it mean?)

typedef void (Server::*FuncType)(Client&, std::istringstream&);

bool Server::_signalReceived = false;

void Server::signalHandler(int signum)
{
    (void)signum;
    Server::_signalReceived = true;
    return ;
}

Server::Server() : _port(0), _sockfd(-1)
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
    this->_port = atoi(port_num);
    this->_password = password;
    
    this->createServSocket(port_num);

    return ;
}

void Server::createServSocket(char* port_num)
{
    struct addrinfo temp, *servinfo, *it;
    int yes = 1;
    struct pollfd new_poll;

    //First of all, we need to get the network address to connect it to socket
    memset(&temp, 0, sizeof(temp));
    temp.ai_family = AF_INET;
    temp.ai_socktype = SOCK_STREAM;
    temp.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port_num, &temp, &servinfo) == -1)
        throw (std::runtime_error("Failed to get network address"));
    
    //Loop through the addresses of servinfo to find the socket that we can bind
    for (it = servinfo; it != NULL; it = it->ai_next)
    {
        //Firstly, we try to create the socket for network address
        this->_sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        //If we can't create socket for this network address, we move to the next address in the struct
        if (this->_sockfd == -1)
            continue ;

        //Now we need to set option of SO_REUSEADDR to reuse the address if it's still being cleaned
        if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            freeaddrinfo(servinfo);
            throw (std::runtime_error("Failed to set option SO_REUSEADDR to the socket"));
        }

        //The next step is to set option of O_NONBLOCK to the socket to prevent its blocking
        if (fcntl(this->_sockfd, F_SETFL, O_NONBLOCK) == -1)
        {
            freeaddrinfo(servinfo);
            throw (std::runtime_error("Failed to set option O_NONBLOCK to the socket"));
        }

        //Next, binding - assotiating the socket with the port for future connection establishment
        if (bind(this->_sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        {
            close(this->_sockfd);
            this->_sockfd = -1;
            continue ;
        }

        //If we reach this point, then we assotiated the socket to the port successfully
        break ;
    }

    freeaddrinfo(servinfo); //-> We don't need this any more

    //Additional check that we binded the socket successfully (if we didn't reach the end of for cycle)
    if (it == NULL) 
        throw (std::runtime_error("Failed to bind socket"));
    
    //The last step - listening to the socket
    if (listen(this->_sockfd, SOCKMAXCONN) == -1)
    {
        close(this->_sockfd);
        throw (std::runtime_error("Failed to start listening"));
    }

    //We also need to put our sockfd to the pollfd struct
    new_poll.fd = this->_sockfd;
    new_poll.events = POLLIN;
    new_poll.revents = 0;
    this->_pollfds.push_back(new_poll);

    return ;
}

void Server::runServer()
{
    //We run eternal loop until we receive any signal pre-defined
    while (!Server::_signalReceived)
    {
        //poll() allows us to wait in background mode for any events for saved fds. After they happen, we continue
        if (poll(&(this->_pollfds[0]), this->_pollfds.size(), -1) == -1 && Server::_signalReceived == false)
            throw (std::runtime_error("Failed to use poll() function"));
        
        if (Server::_signalReceived)
            break ;

        //We loop through the pollfds to find which fd got event
        for (size_t ind = 0; ind < this->_pollfds.size(); ind++)
        {
            if (this->_pollfds[ind].revents & POLLIN)
            {
                //if its the socket fd, then it means that the new client is trying to connect

                std::cout << "this->_pollfds[ind].fd: " << this->_pollfds[ind].fd << std::endl;
                std::cout << "this->_sockfd: " << this->_sockfd << std::endl;

                if (this->_pollfds[ind].fd == this->_sockfd)
                    this->acceptNewClient();
                else
                //otherwise, we receive a new data from already connected client
                    this->receiveNewData(this->_pollfds[ind].fd);
            }
        }
    }

    //in the end, we close all our FDs to avoid leaks
    this->closeFDs();
    return ;
}

void Server::acceptNewClient()
{
    Client new_client;
    struct sockaddr_in client_addr;
    struct pollfd new_poll;
    socklen_t len_addr = sizeof(client_addr);
    int client_fd = -1;
    std::string init_mess = ":irc.local NOTICE AUTH :Welcome!\r\n";

    //we accept the new connection and save the address of client
    client_fd = accept(this->_sockfd, reinterpret_cast<sockaddr*>(&client_addr), &len_addr);
    if (client_fd == -1)
    {
        std::cerr << "Failed accept() of new client" << std::endl;
        return ;
    }

    //we add option of O_NONBLOCK to the new fd of client to avoid blocking
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Failed O_NONBLOCK option for new client" << std::endl;
        close(client_fd);
        return ;
    }

    //then we save the new client fd into pollfds vector
    new_poll.fd = client_fd;
    new_poll.events = POLLIN;
    new_poll.revents = 0;
    this->_pollfds.push_back(new_poll);

    //we set the values of fd and ID-address of the client to the Client var and then add it into vector of Clients
    new_client.setFD(client_fd);
    char *ip = inet_ntoa(client_addr.sin_addr);
    new_client.setIPAddr(ip);
    this->_clients[client_fd] = new_client;

    std::cout << "[DEBUG] ";
    std::cout << "New client " << new_client.getFD() << " is accepted." << std::endl;

    //here we send a welcome message to the connected client
    send(client_fd, init_mess.c_str(), init_mess.length(), 0);

    return ;
}

void Server::receiveNewData(int& clientFD)
{
    std::cout << "receiveNewData" << std::endl;//test
    char buffer[1024];
    ssize_t bytes;
    size_t pos_end;
    std::string raw_cmd;

    //we need an empty buffer to store the receiving message
    memset(buffer, 0, sizeof(buffer));

    //receive the message
    bytes = recv(clientFD, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        //if the return value of recv equals or less than 0, it means that the client disconnected
        std::cout << "[DEBUG] ";
        std::cout << "Client " << clientFD << " disconnected." << std::endl;

        this->clearClient(clientFD);
    }
    else
    {
        std::cout << "buffer1:\n" << buffer << "!" << std::endl;//test

        //otherwise, we store the message and add it to the buffer of Client
        buffer[bytes] = '\0';
        std::cout << "[DEBUG] ";
        std::cout << "Client " << clientFD << " sent data." << std::endl;

        std::string str(buffer);

        Client& our_client = this->_clients[clientFD];
        our_client.appendBuffer(buffer);
        // std::cout << "our_client.getBuffer()1:\n" << our_client.getBuffer() << "!" << std::endl;//test
        // for (size_t i = 0; i < str.size(); i++)
        // {
        //     std::cout << "getBuffer[" << i << "]:" << our_client.getBuffer()[i] << "(" << static_cast<int>(our_client.getBuffer()[i]) << ")!" << std::endl;//test
        // }
        size_t counter = 0;//test
        while (our_client.getBuffer().find(TERMIN) != std::string::npos)//test
        {
            std::cout << "our_client.getBuffer()1:\n" << our_client.getBuffer() << "!" << std::endl;//test
            //here we check if there is a TERMIN in the Client's buffer. If there isn't, then we shall wait for the next portion
            if ((pos_end = our_client.getBuffer().find(TERMIN)) == std::string::npos)
            {
                std::cout << "No TERMIN" << std::endl;//test
                return ;
            }
            //if there is a TERMIN in buffer, we must take a substring, remove it from Client's buffer and process it as a command
            raw_cmd = our_client.getBuffer().substr(0, pos_end);
            our_client.splitBuffer(0, pos_end + 2);// == this->_recvBuffer.erase(start, end);
            std::cout << "-------------------" << std::endl;//test
            std::cout << "our_client.getBuffer(): " << counter << "\n" << our_client.getBuffer() << std::endl;//test
            std::cout << "-------------------" << std::endl;//test

            std::cout << "raw_cmd: " << raw_cmd << "!" << std::endl;//test
            this->handleCommand(our_client, raw_cmd);

            std::cout << "-------------------" << std::endl;//test
            std::cout << "-------------------" << std::endl;//test
            counter++;//test
        }
    }

    return ;
}

void Server::disconnectClient(const int& client_fd)
{
    this->clearClient(client_fd);
    close(client_fd);
    
}

void Server::handleCommand(Client& client, std::string& raw_cmd)
{
    std::cout << "handleCommand" << std::endl;//test
    std::istringstream line(raw_cmd); // it allows to use a string as a stream. Stream send words divided by ' ' (space) symbol
    std::string cmd, err_message;
    const std::map<std::string, FuncType> allowed_cmds = this->getMapCmdFunc();

    if (raw_cmd.empty())
        return ;

    line >> cmd; // it means that we put 1st word from str into command

    //as we have initial commands in uppercase, we need to transform our command
    cmd = toUpperString(cmd);

    //here we launch the command handlers depending on what we receive in CMD (check getMapCmdFunc method)
    std::map<std::string, FuncType>::const_iterator it = allowed_cmds.find(cmd);

    if (!client.isRegistered())
    {
        std::cout << "not isRegistered" << std::endl;//test
        if (it == allowed_cmds.end())
            return ;
        this->handleInitCommands(client, cmd, line);
        return ;
    }

    if (it != allowed_cmds.end())
        (this->*it->second)(client, line);
    else
    {
        err_message = ERR_UNKNOWNCOMMAND(client.getNick(), cmd);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
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

void Server::sendMessageToUser(const Client& client, const int& target_fd,
        const std::string& target_name, const std::string& message,
        const std::string& cmd) const
{
    std::string body, full_message = client.getPrefix() + " " + cmd + " ";
    if (!target_name.empty())
        full_message += target_name;
    
    const size_t symb_left = MAXLINELENGTH - full_message.length() - 3;
    
    if (!message.empty())
    {
        body = std::string(":");
        if (message.length() > symb_left)
            body += message.substr(0, symb_left) + TERMIN;
        else
            body += message;
    }
    
    // full_message += body;
    full_message += body + TERMIN; //test
    send(target_fd, full_message.c_str(), full_message.size(), 0);
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
    //make the client leave all the channels if it is registered
    if (this->_clients[client_fd].isRegistered())
        this->_clients[client_fd].leaveAllChannels(*this);
       
    //remove client from the list of clients of server
    this->_clients.erase(client_fd);

    //remove client from pollfd vector
    for (std::vector<struct pollfd>::iterator it = this->_pollfds.begin();
            it != this->_pollfds.end(); it++)
    {
        if (it->fd == client_fd)
        {
            this->_pollfds.erase(it);
            break ;
        }
    }
    close(client_fd);
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
        func_map[USER] = &Server::handleUsername;
        func_map[PASS] = &Server::handlePassword;
        func_map[NICK] = &Server::handleNickname;
        func_map[INVITE] = &Server::handleInvite;
        func_map[JOIN] = &Server::handleJoin;
        func_map[KICK] = &Server::handleKick;
        func_map[MODE] = &Server::handleMode;
        func_map[NAMES] = &Server::handleNames;
        func_map[NOTICE] = &Server::handleNotice;
        func_map[OPER] = &Server::handleOper;
        func_map[PART] = &Server::handlePart;
        func_map[PING] = &Server::handlePing;
        func_map[PRIVMSG] = &Server::handlePrivateMessage;
        func_map[QUIT] = &Server::handleQuit;
        func_map[TOPIC] = &Server::handleTopic;
        func_map[CAP] = &Server::handleTopic;//test
    }
    return (func_map);
}