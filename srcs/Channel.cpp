# include "Channel.hpp"
# include "Server.hpp"

Channel::Channel()
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
