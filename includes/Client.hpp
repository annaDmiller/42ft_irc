#pragma once

# include <string>

class Client
{
    private:
        int _fd; //-> client FD
        std::string _ipAddr; //-> client IP address
        std::string _recvBuffer; //-> to store the incoming message from client
        std::string _nickname;
        std::string _username;
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

        void setFD(int fd);
        void setIPAddr(std::string addr);
        void appendBuffer(std::string buff);
        void setNickname(std::string& nick);
        void setUsername(std::string& username);
        void checkPassword();
        void authenticationConfirmed();

        void splitBuffer(size_t start, size_t end);
};