# include "Channel.hpp"
# include "Server.hpp"

Channel::Channel() : _membersLimit(-1)
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

void Channel::addMode(char new_mode)
{
    if (this->_modes.find(new_mode, 0) == std::string::npos)
        this->_modes += new_mode;
    return ;
}

void Channel::removeMode(char mode_to_remove)
{
    size_t ind = this->_modes.find(mode_to_remove, 0);

    if (ind != std::string::npos)
        this->_modes.erase(mode_to_remove);
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

void Channel::removeMember(const int& client_fd)
{
    std::set<int>::iterator oper_it;

    oper_it = this->_operators.find(client_fd);
    if (oper_it != this->_operators.end())
        this->_operators.erase(client_fd);

    this->_members.erase(client_fd);
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

void Channel::sendJoinMessages(const Client& client) const
{
    std::string message = client.getPrefix() + " " + JOIN + " " + this->_name;
    this->sendMessageToAll(message);
    this->sendInitReplies(client);
    return ;
}

bool Channel::userIsMember(const int& client_fd) const
{
    for (std::map<int, Client*>::const_iterator it = this->_members.begin();
            it != this->_members.end(); it++)
    {
        if (it->first == client_fd)
            return (true);
    }
    return (false);
}

bool Channel::isEmpty() const
{
    if (this->_members.empty())
        return (true);
    return (false);
}

bool Channel::isOperator(int client_fd) const
{
    if (std::lower_bound(this->_operators.begin(), this->_operators.end(), client_fd)
        != this->_operators.end())
        return (true);
    return (false);
}

void Channel::sendInitReplies(const Client& client) const
{
    std::string message;

    message = RPL_TOPIC(client.getNick(), this->_name, this->_topic);
    send(client.getFD(), message.c_str(), message.size(), 0);
    this->sendMemberList(client);
    return ;
}

void Channel::sendMemberList(const Client& client) const
{
    std::string message, users;
    size_t prefix_length;

    message = RPL_NAMREPLY(client.getNick(), this->_name);
    prefix_length = message.length();
    for (std::map<int, Client*>::const_iterator it = this->_members.begin();
            it != this->_members.end(); it++)
    {
        if (isOperator(it->first))
            users += "@";
        users += it->second->getNick();

        if (users.size() + message.length() + 1 > MAXLINELENGTH - 2)
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

void Channel::sendMessageToAll(const std::string& message) const
{
    for (std::map<int, Client*>::const_iterator it = this->_members.begin();
            it != this->_members.end(); it++)
        send(it->first, message.c_str(), message.size(), 0);

    return ;
}

void Channel::sendMessageToAll(const Client& client, const Server& server, const std::string& target, 
        const std::string& message, const int& except_fd, const std::string& cmd) const
{
    for (std::map<int, Client*>::const_iterator it = this->_members.begin();
            it != this->_members.end(); it++)
        if (it->first != except_fd)
            server.sendMessageToUser(client, it->first, target, message, cmd);

    return ;
}

void Channel::sendMessageToAll(const Client& client, const Server& server, const std::string& target, 
        const std::string& message, std::set<int>& except_fds, const std::string& cmd) const
{
    for (std::map<int, Client*>::const_iterator it = this->_members.begin();
            it != this->_members.end(); it++)
    {
        if (except_fds.find(it->first) != except_fds.end())
            continue ;
        
        except_fds.insert(it->first);
        server.sendMessageToUser(client, it->first, target, message, cmd);
    }
    
    return;
}

void Channel::handleMemberLimit(const bool& isAdding, int& limit)
{
    //NEED: to check what will be if we indicate negative value
    if (limit <= 0)
        return ;
    
    if (isAdding)
    {
        this->addMode('l');
        this->_membersLimit = limit;
    }
    else
    {
        this->removeMode('l');
        this->_membersLimit = -1;
    }

    return ;
}

void Channel::handleKey(const bool& isAdding, std::string& password, Client& client)
{
    //NEED: to check how it will work - will the message be sent to the client?
    if (password.empty())
        return ;
    
    if (isAdding)
    {
        if (!this->_key.empty())
        {
            //NEED: ERR_KEYSET error
            return ;
        }
        this->addMode('k');
        this->_key = password;
    }
    else
    {
        this->removeMode('k');
        this->_key.clear();
    }

    return  ;
}

void Channel::handleOperators(const bool& isAdding, int& client_fd)
{
    //NEED: what behavior if we try to add non-existing user to operator?
    if (client_fd == -1)
        return ;

    //NEED: to check if there is any message sending in this case
    if (this->_members.find(client_fd) == this->_members.end())
        return ;
    
    if (isAdding)
    {
        if (this->_operators.find(client_fd) == this->_operators.end())
            this->_operators.insert(client_fd);
    }
    else
    {
        if (this->_operators.find(client_fd) != this->_operators.end())
            this->_operators.erase(client_fd);
    }

    return ;
}