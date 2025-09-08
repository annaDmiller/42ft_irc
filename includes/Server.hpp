#pragma once

# include <iostream>
# include <sstream> //-> to use std::istringstream to extract words from string
# include <csignal> 
# include <cstring>
# include <string>
# include <vector>
# include <map>
# include <algorithm>
# include <cctype>
# include <limits>
# include <climits>

# include <unistd.h> //-> for using C-type close() function for socket fds
//C++-type of closing can't be used as it requires non-raw FD

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <fcntl.h>
# include <poll.h>

# include "Client.hpp"
# include "Macros.hpp"
# include "Channel.hpp"

class Client;
class Channel;

class Server
{
    private:
        int _port;
        int _sockfd;
        std::string _password;
        std::map<int, Client> _clients;
        std::vector<struct pollfd> _pollfds;
        static bool _signalReceived;
        std::map<std::string, Channel> _availableChannels;

        Server(const Server& other);
        Server& operator=(const Server&other);

        void createServSocket(char* port_num);

        void acceptNewClient();
        void receiveNewData(int& clientFD);

        void handleCommands(Client& client, std::string& raw_cmd);
        void handleInitCommands(Client& client, std::string& cmd, std::istringstream& args);
        void handleNicknameCmd(Client& client, std::istringstream& args);
        void handleUsernameCmd(Client& client, std::istringstream& args);
        void handlePasswordCmd(Client& client, std::istringstream& args);
        void handleCapCmd(Client& client, std::istringstream& args);
        void handleJoinCmd(Client& client, std::istringstream& args);
        void handlePrivmsgCmd(Client& client, std::istringstream& args);
        void handleQuitCmd(Client& client, std::istringstream& args);
        void handlePingCmd(Client& client, std::istringstream& args);
        void handleNamesCmd(Client& client, std::istringstream& args);
        void handlePartCmd(Client& client, std::istringstream& args);
        void handlePartCmd(Client& client);
        void handleNoticeCmd(Client& client, std::istringstream& args);
        void handleModeCmd(Client& client, std::istringstream& args);
        void handleTopicCmd(Client& client, std::istringstream& args);
        void handleInviteCmd(Client& client, std::istringstream& args);
        void handleOperCmd(Client& client, std::istringstream& args);
        void handleKickCmd(Client& client, std::istringstream& args);

        std::string toUpperString(std::string str);
        std::string getChannelName(std::string &channel_name);
        bool isValidNickname(const std::string& nick) const;
        bool checkDupNicknamesOnServer(std::string& nick);
        int findUserbyNickname(const std::string& nick) const;
        void sendReply(int clientFD);

        bool isValidChannelName(const std::string& chan_name) const;
        bool isChannelExist(std::string &channel_name);
        bool isChannelTarget(const std::string& target) const;

        std::string changeChannelModes(Client& client, Channel& channel,
                std::vector<std::string>& params);
        bool isValidMode(char mode);
        std::string composeModeMessage(std::map<std::string, std::string> &modes_add, 
                                        std::map<std::string, std::string> &modes_remove) const;
        void setMessageMode(Channel& channel, bool isAdding, std::string mode_change, std::string add_value, 
                                std::string remove_value, std::string mode_start,
                                std::map<std::string, std::string> &modes_add, 
                                std::map<std::string, std::string> &modes_remove);

        void closeFDs();
        void clearClient(const int& client_fd);

        typedef void (Server::*FuncType)(Client&, std::istringstream&);
        static const std::map<std::string, FuncType>& getMapCmdFunc();

    public:
        Server();
        ~Server();

        void initServer(char* port_num, char* password);
        void runServer();
        void sendMessageToUser(Client& sender, const int& target_fd, 
                const std::string& target_name, const std::string& message,
                const std::string& cmd);
        void deleteChannel(const std::string& channel_name);
        void initSignal();

        static void signalHandler(int signum); 
};

bool isSpecial(char car);
std::vector<std::string> ft_split(std::string str, char delim);
bool isForbiddenForChannelName(char car);
std::string ft_itos(int number);