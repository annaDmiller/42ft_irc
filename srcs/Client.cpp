#include "Client.hpp"

Client::Client() : 
        _fd(-1), _nickname("*"), _hasNickname(false),
        _isAuth(false), _isPasswordChecked(false),
        _hasUsername(false), _isOperator(false)
{
    return ;
}

Client::~Client()
{
    return ;
}

Client::Client(const Client& other)
{
    this->_fd = other._fd;
    this->_ipAddr = other._ipAddr;
    this->_recvBuffer = other._recvBuffer;
    this->_nickname = other._nickname;
    this->_username = other._username;
    this->_realname = other._realname;
    this->_isPasswordChecked = other._isPasswordChecked;
    this->_isAuth = other._isAuth;
    this->_hasNickname = other._hasNickname;
    this->_hasUsername = other._hasUsername;
    this->_isOperator = other._isOperator;
    this->_joinedChannels = other._joinedChannels;
    return ;
}

Client& Client::operator=(const Client& other)
{
    std::string temp_key;

    if (this != &other)
    {
        this->_fd = other._fd;
        this->_ipAddr = other._ipAddr;
        this->_recvBuffer = other._recvBuffer;
        this->_nickname = other._nickname;
        this->_username = other._username;
        this->_realname = other._realname;
        this->_isPasswordChecked = other._isPasswordChecked;
        this->_isAuth = other._isAuth;
        this->_hasNickname = other._hasNickname;
        this->_hasUsername = other._hasUsername;
        this->_isOperator = other._isOperator;
        this->_joinedChannels = other._joinedChannels;
    }
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

bool Client::isPassChecked() const
{
    return (this->_isPasswordChecked);
}

std::string Client::getNick() const
{
    return (this->_nickname);
}

bool Client::isHaveNick() const
{
    return (this->_hasNickname);
}

bool Client::isHaveUsername() const
{
    return (this->_hasUsername);
}

size_t Client::joinedChannelQuantity() const
{
    return (this->_joinedChannels.size());    
}

std::string Client::getPrefix() const
{
    return (this->_nickname + "!" + this->_username + "@" + this->_ipAddr);
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

void Client::setOperator(bool value)
{
    this->_isOperator = value;
    return ;
}

void Client::setRealname(const std::string& realname)
{
    this->_realname = realname;
    return ;
}

void Client::setUsername(const std::string& username)
{
    this->_username = username;
    return ;
}

void Client::checkPassword()
{
    this->_isPasswordChecked = true;
    return ;
}

void Client::addChannel(const std::string& channel_name, Channel* channel)
{
    this->_joinedChannels[channel_name] = channel;
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

bool Client::isAlreadyJoinedChannel(const std::string& channel_name) const
{
    if (this->_joinedChannels.empty())
        return (false);
    
    for (std::map<std::string, Channel *>::const_iterator it = this->_joinedChannels.begin();
        it != this->_joinedChannels.end(); it++)
    {
        if (channel_name == it->first)
            return (true);
    }
    return (false);
}

void Client::sendToAllJoinedChannels(const Server& server, const std::string& message,
        const std::string& cmd, bool isOnce, bool includeChannelName) const
{
    std::set<int> already_sent_fds;

    for (std::map<std::string, Channel*>::const_iterator it = this->_joinedChannels.begin();
            it != this->_joinedChannels.end(); it++)
        if (isOnce)
            it->second->sendMessageToAll(*this, server, "", message, already_sent_fds, cmd);
        else
        {
            if (includeChannelName)
                it->second->sendMessageToAll(*this, server, it->first, message, -1, cmd);
            else
                it->second->sendMessageToAll(*this, server, "", message, -1, cmd);

        }
    
    return ;
}

void Client::leaveAllChannels(Server& server)
{
    if (this->_joinedChannels.empty())
        return ;
        
    for (std::map<std::string, Channel*>::iterator it = this->_joinedChannels.begin();
            it != this->_joinedChannels.end(); it++)
        it->second->removeMember(this->_fd, server);
    
    this->_joinedChannels.clear();
    return ;
}

void Client::leaveChannel(const std::string& channel_name)
{
    this->_joinedChannels.erase(channel_name);
    return ;
}