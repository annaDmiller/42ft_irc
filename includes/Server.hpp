#pragma once

# include <iostream>
# include <sstream> //-> to use std::istringstream to extract words from string
# include <cerrno> //-> for using errno global var
# include <cstdlib> //-> for using exit function
# include <csignal> //-> for signal handling
# include <cstring> //-> for using memset
# include <vector> //-> for vector using
# include <map> //-> for map using
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
# include "Channel.hpp"

class Server
{
    private:
        int _port; //-> server port
        int _sockfd; //-> server socket FD
        std::string _password;
        std::map<int, Client> _clients; //-> map of clients, key is fd
        std::vector<struct pollfd> _pollfds; //-> vector of pollfds which will be used for poll() function
        static bool _signalReceived;
        std::string host; //-> used for macros messages
        std::string server_name; //-> used for macros messages
        std::map<std::string, Channel> _availableChannels; //-> map of available channels, key is its name

        Server(const Server& other);
        Server& operator=(const Server&other);

        void createServSocket(char* port_num);

        void acceptNewClient();

        void receiveNewData(int& clientFD);

        void handleCommand(Client& client, std::string& raw_cmd);

        void handleInitCommands(Client& client, std::string& cmd, std::istringstream& args);
        void handleNickname(Client& client, std::istringstream& args);
        bool isValidNickname(const std::string& nick);
        bool checkDupNicknamesOnServer(std::string& nick);
        void handleUsername(Client& client, std::istringstream& args);
        void handlePassword(Client& client, std::istringstream& args);
        void handleJoin(Client& client, std::istringstream& args);
        void sendUnknownCMDReply(Client& client, std::string& cmd);

        void closeFDs(); //-> close ALL fds
        void clearClient(int fd);

    public:
        Server();
        ~Server();

        void initServer(char* port_num, char* password);
        void runServer();

        static void signalHandler(int signum); //-> signal handler for any signals
};

bool isSpecial(char car);