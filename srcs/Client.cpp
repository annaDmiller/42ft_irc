#include "Client.hpp"

Client::Client() : 
        _fd(-1), _hasNickname(false), _hasUsername(false), 
        _isPasswordChecked(false), _isAuth(false), _isOperator(false)
{
    return ;
}

Client::~Client()
{
    return ;
}

Client::Client(const Client& other)
{
    (void)other;
    return ;
}

Client& Client::operator=(const Client& other)
{
    (void)other;
    return (*this);
}

int Client::getFD() const
{
    return (this->_fd);
}

std::string Client::getIPAddr() const
{
    return (this->_ipAddr);
}

std::string Client::getBuffer() const
{
    return (this->_recvBuffer);
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

void Client::appendBuffer(std::string buff)
{
    this->_recvBuffer.append(buff);
    return ;
}

void Client::setNickname(std::string& nick)
{
    this->_nickname = nick;
    this->_hasNickname = true;
    return ;
}

void Client::setUsername(std::string& username)
{
    this->_username = username;
    this->_hasUsername = true;
    return ;
}

void Client::checkPassword()
{
    this->_isPasswordChecked = true;
    return ;
}

void Client::authenticationConfirmed()
{
    this->_isAuth = true;
    return ;
}

void Client::splitBuffer(size_t start, size_t end)
{
    this->_recvBuffer.erase(start, end);
    return ;
}