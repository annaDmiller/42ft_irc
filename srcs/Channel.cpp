# include "Channel.hpp"
# include "Server.hpp"

Channel::Channel() : _membersLimit(0)
{
    return ;
}

Channel::~Channel()
{
    return ;
}

Channel::Channel(const Channel& other)
{
    this->_name = other._name;
    this->_topic = other._topic;
    this->_operators = other._operators;
    this->_members = other._members;
    this->_modes = other._modes;
    this->_membersLimit = other._membersLimit;
    this->_key = other._key;
    return ;
}

Channel& Channel::operator=(const Channel& other)
{
    if (this!= &other)
    {
        this->_name = other._name;
        this->_topic = other._topic;
        this->_operators = other._operators;
        this->_members = other._members;
        this->_modes = other._modes;
        this->_membersLimit = other._membersLimit;
        this->_key = other._key;
    }
    return (*this);
}

std::string Channel::getName() const
{
    return (this->_name);
}

std::string Channel::getTopic() const
{
    return (this->_topic);
}

std::string Channel::getChannelModes() const
{
    return (this->_modes);
}

std::string Channel::getKey() const
{
    return (this->_key);
}

void Channel::setName(const std::string& name)
{
    this->_name = name;
    return ;
}

void Channel::setTopic(const std::string& topic)
{
    this->_topic = topic;
    return ;
}

void Channel::addModes(const std::string& adding_modes)
{
    for (size_t ind = 0; ind < adding_modes.size(); ind++)
    {
        if (this->_modes.find(adding_modes[ind], 0) != std::string::npos)
            continue ;
        this->_modes += adding_modes[ind];
    }
    return ;
}

void Channel::setKey(const std::string& key)
{
    this->_key = key;
    return ;
}

void Channel::addOperator(const int& client_fd)
{
    this->_operators.insert(client_fd);
    return ;
}

void Channel::addMember(const int& client_fd, Client* client)
{
    this->_members[client_fd] = client;
    return ;
}

bool Channel::canBeJoined() const
{
    size_t num_members = _members.size();
    if (num_members + 1 <= this->_membersLimit)
        return (true);
    return (false);
}

bool Channel::isKeyCorrect(const std::string& key) const
{
    if (key.empty())
        return (false);

    if (key == this->_key)
        return (true);
    return (false);
}

void Channel::sendJoinMessages(Client& client) const
{
    std::string message = ":" + client.getPrefix() + " JOIN #" + this->_name + TERMIN;

    this->sendMessageToAdll(message);
    this->sendInitReplies(client);
    return ;
}

bool Channel::isOperator(const int& client_fd) const
{
    if (this->_operators.find(client_fd) != this->_operators.end())
        return (true);
    return (false);
}

void Channel::sendInitReplies(Client& client) const
{
    std::string message, users;
    size_t prefix_length;

    message = RPL_TOPIC(client.getNick(), this->_name, this->_topic);
    send(client.getFD(), message.c_str(), message.size(), 0);

    message = RPL_NAMREPLY(client.getNick(), this->_name);
    prefix_length = message.length();
    for (std::map<int, Client*>::const_iterator it = this->_members.cbegin();
            it != this->_members.cend(); it++)
    {
        if (isOperator(it->first))
            users += "@";
        users += it->second->getNick();

        if (users.size() + message.length() + 1 > MAXLINELENGTH)
        {
            message += TERMIN;
            send(client.getFD(), message.c_str(), message.size(), 0);
            message = RPL_NAMREPLY(client.getNick(), this->_name);
        }
        if (message.length() != prefix_length)
            message += " ";
        message += users;
    }
    if (message.length() != prefix_length)
    {
        message += TERMIN;
        send(client.getFD(), message.c_str(), message.size(), 0);
    }

    message = RPL_ENDOFNAMES(client.getNick(), this->_name);
    send(client.getFD(), message.c_str(), message.size(), 0);
    return ;
}

void Channel::sendMessageToAdll(const std::string& message) const
{
    for (std::map<int, Client*>::const_iterator it = this->_members.cbegin();
            it != this->_members.cend(); it++)
        send(it->first, message.c_str(), message.size(), 0);

    return ;
}