#pragma once

# include <string>

class Client
{
    private:
        int _fd; //-> client FD
        std::string _ipAddr; //-> client IP address

    public:
        Client();
        ~Client();

        int getFD() const;

        void setFD(int fd);
        void setIPAddr(std::string addr);
};