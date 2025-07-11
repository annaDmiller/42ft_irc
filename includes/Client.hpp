#pragma once

# include <string>

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

        Client(const Client& other);
        Client& operator=(const Client& other);

    public:
        Client();
        ~Client();

        int getFD() const;
        std::string getIPAddr() const;
        std::string getBuffer() const;
        bool isRegistered() const;
        std::string getNick() const;
        bool isHaveNick() const;
        std::string getUsername() const;
        bool isHaveUsername() const;

        void setFD(int fd);
        void setIPAddr(std::string addr);
        void appendBuffer(std::string buff);
        void setNickname(const std::string& nick);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void checkPassword();

        void splitBuffer(size_t start, size_t end);

        bool tryAuthenticate();
};