#pragma once

# include <iostream>
# include <cerrno> //-> for using errno global var
# include <cstdlib> //-> for using exit function
# include <csignal> //-> for signal handling
# include <cstring> //-> for using memset
# include <vector> //-> for vector using

# include <unistd.h> //-> for using C-type close() function for socket fds
//C++-type of closing can't be used as it requires non-raw FD

# include <sys/socket.h> //-> for socket() functions
# include <sys/types.h> //-> for socket() functions
# include <netinet/in.h> //-> for sockaddr_in
# include <arpa/inet.h> //-> for inet_ntoa()
# include <netdb.h> //-> for getaddrinfo() and related structures

# include "Client.hpp"

class Server
{
    private:
        int _port; //-> server port
        int _sockfd; //-> server socket FD
        std::vector<Client> clients; //-> vector of clients to store their IPs and FDs

    public:
        Server();
        ~Server();
};