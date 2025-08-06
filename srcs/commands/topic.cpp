#include "Server.hpp"

void Server::handleTopic(Client& client, std::istringstream& args)
{
    std::string channel_name, topic, err_message, message, channel_modes;

    args >> channel_name;
    if (args.peek() == ' ')
        args.get();
    if (args.peek() == ':')
        std::getline(args, topic);
    else
        args >> topic;
    
    if (!topic.empty() && topic[0] == ':')
        topic = topic.substr(1);

    if (channel_name.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), TOPIC);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (this->_availableChannels.find(channel_name) == this->_availableChannels.end())
    {
        err_message = ERR_NOSUCHCHANNEL(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (!client.isAlreadyJoinedChannel(channel_name))
    {
        err_message = ERR_NOTONCHANNEL(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    Channel& channel = this->_availableChannels[channel_name];
    if (topic.empty())
    {
        channel.printTopic(client);
        return ;
    }

    channel_modes = channel.getChannelModes();
    if (channel_modes.find('t', 0) != std::string::npos && !channel.isOperator(client.getFD()))
    {
        err_message = ERR_CHANOPRIVSNEEDED(client.getNick(), channel.getName());
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    channel.setTopic(topic);
    channel.sendMessageToAll(client, *this, channel_name, topic, -1, TOPIC);
    return ;
}
