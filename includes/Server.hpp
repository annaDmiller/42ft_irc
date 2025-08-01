#pragma once

# include <iostream>
# include <sstream> //-> to use std::istringstream to extract words from string
# include <cerrno> //-> for using errno global var
# include <cstdlib> //-> for using exit function
# include <csignal> //-> for signal handling
# include <cstring> //-> for using memset
# include <string>
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

class Client;
class Channel;

class Server
{
    private:
        int _port; //-> server port
        int _sockfd; //-> server socket FD
        std::string _password;
        std::map<int, Client> _clients; //-> map of clients, key is fd
        std::vector<struct pollfd> _pollfds; //-> vector of pollfds which will be used for poll() function
        static bool _signalReceived;
        std::map<std::string, Channel> _availableChannels; //-> map of available channels, key is its channel names

        Server(const Server& other);
        Server& operator=(const Server&other);

        void createServSocket(char* port_num);

        void acceptNewClient();
        void receiveNewData(int& clientFD);
        void disconnectClient(const int& client_fd);

        void handleCommand(Client& client, std::string& raw_cmd);
        void handleInitCommands(Client& client, std::string& cmd, std::istringstream& args);
        void handleNickname(Client& client, std::istringstream& args);
        void handleUsername(Client& client, std::istringstream& args);
        void handlePassword(Client& client, std::istringstream& args);
        void handleJoin(Client& client, std::istringstream& args);
        void handlePrivateMessage(Client& client, std::istringstream& args);
        void handleQuit(Client& client, std::istringstream& args);
        void handlePing(Client& client, std::istringstream& args);
        void handleNames(Client& client, std::istringstream& args);
        void handlePart(Client& client, std::istringstream& args);
        void handlePart(Client& client); // -> for PART from all the channels
        void handleNotice(Client& client, std::istringstream& args);
        void handleMode(Client& client, std::istringstream& args);

        bool isValidNickname(const std::string& nick) const;
        bool checkDupNicknamesOnServer(std::string& nick);
        int findUserbyNickname(const std::string& nick) const;

        bool isValidChannelName(const std::string& chan_name) const;
        bool isChannelExist(std::string &channel_name) const;
        bool isChannelTarget(const std::string& target) const;

        size_t modeHandlingChannel(Client& client, Channel& channel,
                std::vector<std::string>& params);
        bool isValidModes(const std::string& modes, char& incorrect_mode);

        void closeFDs(); //-> close ALL fds
        void clearClient(const int& client_fd);

    public:
        Server();
        ~Server();

        void initServer(char* port_num, char* password);
        void runServer();
        void sendMessageToUser(const Client& sender, const int& target_fd, 
                const std::string& target_name, const std::string& message,
                const std::string& cmd) const;

        static void signalHandler(int signum); //-> signal handler for any signals
};

bool isSpecial(char car);
std::vector<std::string> ft_split(std::string str, char delim);
bool isForbiddenForChannelName(char car);