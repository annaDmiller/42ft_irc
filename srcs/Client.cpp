#include "Client.hpp"

Client::Client() : _fd(-1), _ipAddr(NULL)
{
    return ;
}

Client::~Client()
{
    return ;
}

int Client::getFD() const
{
    return (this->_fd);
}

std::string Client::getIPAddr() const
{
    return (this->_ipAddr);
}

void Client::setFD(int fd)
{
    this->_fd = fd;
    return ;
}

void Client::setIPAddr(std::string addr)
{
    this->_ipAddr = addr;
    return ;
}