#pragma once

# include <iostream>
# include <sstream> //-> to use std::istringstream to extract words from string
# include <cerrno> //-> for using errno global var
# include <cstdlib> //-> for using exit function
# include <csignal> //-> for signal handling
# include <cstring> //-> for using memset
# include <vector> //-> for vector using
# include <algorithm>
# include <cctype>

# include <unistd.h> //-> for using C-type close() function for socket fds
//C++-type of closing can't be used as it requires non-raw FD

# include <sys/socket.h> //-> for socket() functions
# include <sys/types.h> //-> for socket() functions
# include <netinet/in.h> //-> for sockaddr_in
# include <arpa/inet.h> //-> for inet_ntoa()
# include <netdb.h> //-> for getaddrinfo() and related structures
# include <fcntl.h> //-> for fcntl() function
# include <poll.h> //-> for poll() function

# include "Client.hpp"
# include "Macros.hpp"

class Server
{
    private:
        int _port; //-> server port
        int _sockfd; //-> server socket FD
        std::string _password;
        std::vector<Client> _clients; //-> vector of clients to store their IPs and FDs
        std::vector<struct pollfd> _pollfds; //-> vector of pollfds which will be used for poll() function
        static bool _signalReceived;

        Server(const Server& other);
        Server& operator=(const Server&other);

        void createServSocket(char* port_num);

        void acceptNewClient();

        void receiveNewData(int& clientFD);

        void handleCommand(Client& client, std::string& raw_cmd);

        void handleInitCommands(Client& client, std::string& raw_cmd);
        void handleNickname(Client& client, std::istringstream& args);
        bool isValidNickname(const std::string& nick);
        bool checkDupNicknamesOnServer(std::string& nick);
        void handleUsername(Client& client, std::istringstream& args);
        void handlePassword(Client& client, std::istringstream& args);
        void sendUnknownCMDReply(Client& client, std::string& cmd);

        void closeFDs(); //-> close ALL fds
        void clearClient(int fd);

        size_t findIndClient(int& fd) const;

    public:
        Server();
        ~Server();

        void initServer(char* port_num, char* password);
        void runServer();

        static void signalHandler(int signum); //-> signal handler for any signals
};

bool isSpecial(char car);