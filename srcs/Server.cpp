#include "Server.hpp"

bool Server::_signalReceived = false;

void Server::signalHandler(int signum)
{
    (void)signum;
    Server::_signalReceived = true;
    return ;
}

Server::Server() : _sockfd(-1), _port(0), _password(NULL)
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
    while (!Server::_signalReceived)
    {
        if (poll(&(this->_pollfds[0]), this->_pollfds.size(), -1) == -1 && Server::_signalReceived == false)
            throw (std::runtime_error("Failed to use poll() function"));
        
        for (size_t ind = 0; ind < this->_pollfds.size(); ind++)
        {
            if (this->_pollfds[ind].revents & POLLIN)
            {
                if (this->_pollfds[ind].fd == this->_sockfd)
                    this->acceptNewClient();
                else
                    this->receiveNewData(this->_pollfds[ind].fd);
            }
        }
    }

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

    client_fd = accept(this->_sockfd, reinterpret_cast<sockaddr*>(&client_addr), &len_addr);
    if (client_fd == -1)
    {
        std::cerr << "Failed accept() of new client" << std::endl;
        return ;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Failed O_NONBLOCK option for new client" << std::endl;
        close(client_fd);
        return ;
    }

    new_poll.fd = client_fd;
    new_poll.events = POLLIN;
    new_poll.revents = 0;
    this->_pollfds.push_back(new_poll);

    new_client.setFD(client_fd);
    new_client.setIPAddr(inet_ntop(AF_INET, &(client_addr.sin6_addr), client_ip, sizeof(client_ip)));
    this->_clients.push_back(new_client);

    return ;
}

void Server::receiveNewData(int clientFD)
{
    char buffer[1024];
    ssize_t bytes;

    memset(buffer, 0, sizeof(buffer));

    bytes = recv(clientFD, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        this->clearClient(clientFD);
        close(clientFD);
    }
    else
    {
        buffer[bytes] = '\0';
        std::cout << "Client <" << clientFD << "> Data: " << buffer;
    }

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