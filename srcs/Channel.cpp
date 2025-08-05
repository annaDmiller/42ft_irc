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

void Channel::removeMember(const int& client_fd, Server& server)
{
    std::set<int>::iterator oper_it;

    this->_members.erase(client_fd);

    oper_it = this->_operators.find(client_fd);
    if (oper_it != this->_operators.end())
    {
        this->_operators.erase(client_fd);
        if (this->_operators.empty() && !this->_members.empty())
        {
            std::map<int, Client*>::iterator it = this->_members.begin();
            if (it != this->_members.end())
                this->_operators.insert(it->first);
        }
    }

    if (this->_members.empty())
        server.deleteChannel(this->getName());

    return ;
}

bool Channel::canBeJoined() const
{
    if (this->_membersLimit == -1)
        return (true);
    size_t num_members = _members.size();
    if (num_members + 1 <= static_cast<size_t>(this->_membersLimit))
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

bool Channel::handleMemberLimit(const bool& isAdding, int& limit)
{
    if (isAdding)
    {
        if (limit <= 0)
            return (false);
        this->addMode('l');
        this->_membersLimit = limit;
    }
    else
    {
        if (this->_modes.find('l', 0) == std::string::npos)
            return (false);
        this->removeMode('l');
        this->_membersLimit = -1;
    }

    return (true);
}

bool Channel::handleKey(const bool& isAdding, std::string& password, Client& client)
{
    std::string err_message;

    if (isAdding)
    {
        if (password.empty())
            return (false);
        if (!this->_key.empty())
        {
            err_message = ERR_KEYSET(client.getNick(), this->_name);
            send(client.getFD(), err_message.c_str(), err_message.size(), 0);
            return (false);
        }
        this->addMode('k');
        this->_key = password;
    }
    else
    {
        if (this->_modes.find('k', 0) == std::string::npos)
            return (false);
        this->removeMode('k');
        this->_key.clear();
    }

    return (true);
}

bool Channel::handleOperators(const bool& isAdding, int& target_fd, Client& client, std::string& target)
{
    std::string err_message;

    if (this->_members.find(target_fd) == this->_members.end())
    {
        err_message = ERR_USERNOTINCHANNEL(client.getNick(), this->_name, target);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return (false);
    }
    
    if (isAdding)
    {
        if (this->_operators.find(target_fd) == this->_operators.end())
            this->_operators.insert(target_fd);
    }
    else
    {
        if (this->_operators.find(target_fd) != this->_operators.end())
            this->_operators.erase(target_fd);
    }

    return (true);
}

void Channel::printModes(Client& client) const
{
    std::string modes, mode_params, message;
    size_t ind_k, ind_l;

    modes = std::string("+") + this->_modes;
    ind_k = this->_modes.find('k', 0);
    ind_l = this->_modes.find('l', 0);

    if (ind_k != std::string::npos && ind_l != std::string::npos)
    {
        if (ind_k > ind_l)
            mode_params = ft_itos(this->_membersLimit) + " " + this->_key;
        else 
            mode_params = this->_key + " " + ft_itos(this->_membersLimit);
    }
    else
    {
        if (ind_l != std::string::npos)
            mode_params = ft_itos(this->_membersLimit);
        if (ind_k != std::string::npos)
            mode_params = this->_key;
    }

    message = RPL_CHANNELMODEIS(client.getNick(), this->_name, modes, mode_params);
    send(client.getFD(), message.c_str(), message.size(), 0);
    return ;
}