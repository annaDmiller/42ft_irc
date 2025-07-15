#pragma once

# include <string>
# include "Channel.hpp"
# include <map>

class Client
{
    private:
        int _fd; //-> client FD
        std::string _ipAddr; //-> client IP address
        std::string _recvBuffer; //-> to store the incoming message from client
        std::string _nickname; //-> set with NICK command
        std::string _username; //-> set with USER command
        std::string _realname; //-> set with USER command
        bool _hasNickname;
        bool _isAuth;
        bool _isPasswordChecked;
        bool _hasUsername;
        bool _isOperator;
        std::map<std::string, Channel*> _joinedChannels;

    public:
        Client();
        ~Client();
        Client& operator=(const Client& other);
        Client(const Client& other);
        
        int getFD() const;
        std::string getIPAddr() const;
        std::string getBuffer() const;
        bool isRegistered() const;
        std::string getNick() const;
        bool isHaveNick() const;
        std::string getUsername() const;
        bool isHaveUsername() const;
        size_t joinedChannelQuantity() const;
        std::string getPrefix() const;

        void setFD(int fd);
        void setIPAddr(std::string addr);
        void appendBuffer(std::string buff);
        void setNickname(const std::string& nick);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void checkPassword();
        void addChannel(const std::string& channel_name, Channel* channel);

        void splitBuffer(size_t start, size_t end);

        bool tryAuthenticate();
        bool isAlreadyJoinedChannel(const std::string& channel_name);
};