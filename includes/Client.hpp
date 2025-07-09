#pragma once

# include <string>

class Client
{
    private:
        int _fd; //-> client FD
        std::string _ipAddr; //-> client IP address

        Client(const Client& other);
        Client& operator=(const Client& other);

    public:
        Client();
        ~Client();

        int getFD() const;
        std::string getIPAddr() const;

        void setFD(int fd);
        void setIPAddr(std::string addr);
};