#include "Client.hpp"

Client::Client() : 
        _fd(-1), _hasNickname(false), _hasUsername(false), 
        _isPasswordChecked(false), _isAuth(false), _isOperator(false),
        _nickname("*")
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

bool Client::isRegistered() const
{
    if (this->_isAuth)
        return (true);
    return (false);
}

std::string Client::getNick() const
{
    return (this->_nickname);
}

bool Client::isHaveNick() const
{
    return (this->_hasNickname);
}

std::string Client::getUsername() const
{
    return (this->_username);
}

bool Client::isHaveUsername() const
{
    return (this->_hasUsername);
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

void Client::setNickname(const std::string& nick)
{
    this->_nickname = nick;
    this->_hasNickname = true;
    return ;
}

void Client::setUsername(const std::string& username)
{
    this->_username = username;
    this->_hasUsername = true;
    return ;
}

void Client::setRealname(const std::string& realname)
{
    this->_realname = realname;
    return ;
}

void Client::checkPassword()
{
    this->_isPasswordChecked = true;
    return ;
}

void Client::splitBuffer(size_t start, size_t end)
{
    this->_recvBuffer.erase(start, end);
    return ;
}

bool Client::tryAuthenticate()
{
    if (!this->_isAuth && this->_hasNickname && this->_hasUsername && this->_isPasswordChecked)
        this->_isAuth = true;
    
    return (this->_isAuth);
}