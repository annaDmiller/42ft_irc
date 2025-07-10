#include "Server.hpp"

bool Server::_signalReceived = false;

void Server::signalHandler(int signum)
{
    (void)signum;
    Server::_signalReceived = true;
    return ;
}

Server::Server() : _sockfd(-1), _port(0)
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
    temp.ai_family = AF_INET6;
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
        
        //We loop through the pollfds to find which fd got event
        for (size_t ind = 0; ind < this->_pollfds.size(); ind++)
        {
            if (this->_pollfds[ind].revents & POLLIN)
            {
                //if its the socket fd, then it means that the new client is trying to connect
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
    struct sockaddr_in6 client_addr;
    struct pollfd new_poll;
    socklen_t len_addr = sizeof(client_addr);
    int client_fd = -1;
    char client_ip[INET6_ADDRSTRLEN];
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
    new_client.setIPAddr(inet_ntop(AF_INET, &(client_addr.sin6_addr), client_ip, sizeof(client_ip)));
    this->_clients.push_back(new_client);

    std::cout << "[DEBUG]";
    std::cout << "New client " << new_client.getFD() << " is accepted." << std::endl;

    //here we send a welcome message to the connected client
    send(client_fd, init_mess.c_str(), init_mess.length(), 0);

    return ;
}

void Server::receiveNewData(int& clientFD)
{
    char buffer[1024];
    ssize_t bytes;
    size_t ind_client, pos_end;
    std::string raw_cmd;

    //we need an empty buffer to store the receiving message
    memset(buffer, 0, sizeof(buffer));

    //receive the message
    bytes = recv(clientFD, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        //if the return value of recv equals or less than 0, it means that the client disconnected
        std::cout << "[DEBUG]";
        std::cout << "Client " << clientFD << " disconnected." << std::endl;

        this->clearClient(clientFD);
        close(clientFD);
    }
    else
    {
        //otherwise, we store the message and add it to the buffer of Client
        buffer[bytes] = '\0';

        std::cout << "[DEBUG]";
        std::cout << "Client " << clientFD << " sent data." << std::endl;

        ind_client = this->findIndClient(clientFD);
        Client& our_client = this->_clients[ind_client];
        our_client.appendBuffer(buffer);

        //here we check if there is a TERMIN in the Client's buffer. If there isn't, then we shall wait for the next portion
        if ((pos_end = our_client.getBuffer().find(TERMIN)) == std::string::npos)
            return ;
        
        //if there is a TERMIN in buffor, we must take a substring, remove it from Client's buffer and process it as a command
        raw_cmd = our_client.getBuffer().substr(0, pos_end);
        our_client.splitBuffer(0, pos_end + 2);

        this->handleCommand(our_client, raw_cmd);
    }

    return ;
}

void Server::handleCommand(Client& client, std::string& raw_cmd)
{
    if (raw_cmd.empty())
        return ;

    this->handleInitCommands(client, raw_cmd);

    return ;
}

void Server::closeFDs()
{
    for (size_t ind = 0; ind < this->_clients.size(); ind++)
        close(this->_clients[ind].getFD());
    
    if (this->_sockfd != -1)
        close(this->_sockfd);

    return ;
}

size_t Server::findIndClient(int& fd) const
{
    for (size_t ind = 0; ind < this->_clients.size(); ind++)
    {
        if (this->_clients[ind].getFD() == fd)
            return (ind);
    }
    throw (std::runtime_error("Impossible to find client with this fd"));
    return (0);
}