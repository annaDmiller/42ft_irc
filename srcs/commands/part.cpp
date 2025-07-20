#include "Server.hpp"

void Server::handlePart(Client& client, std::istringstream& args)
{
    std::string channels, message, err_message;
    std::vector<std::string> channel_list;

    args >> channels;
    std::getline(args, message);

    if (channels.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), PART);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (message.empty())
        message = client.getNick();
    else
    {
        if (message[0] == ' ')
            message = message.substr(1);
        if (message[0] == ':')
            message = message.substr(1);
    }

    channel_list = ft_split(channels, ',');

    for (size_t ind = 0; ind < channel_list.size(); ind++)
    {
        if (!this->isChannelExist(channel_list[ind]))
        {
            err_message = ERR_NOSUCHCHANNEL(client.getNick(), channel_list[ind]);
            send(client.getFD(), err_message.c_str(), err_message.size(), 0);
            continue ;
        }

        Channel& channel = this->_availableChannels[channel_list[ind]];
        if (!client.isAlreadyJoinedChannel(channel.getName()))
        {
            err_message = ERR_NOTONCHANNEL(client.getNick(), channel.getName());
            send(client.getFD(), err_message.c_str(), err_message.size(), 0);
            continue ;
        }

        channel.sendMessageToAll(client, *this, channel.getName(), message, -1, PART);
        channel.removeMember(client.getFD());
        client.leaveChannel(channel.getName());

        if (channel.isEmpty())
            this->_availableChannels.erase(channel.getName());
    }
}

void Server::handlePart(Client& client)
{
    std::string message;

    message = client.getNick();
    client.sendToAllJoinedChannels(*this, message, PART, false, true);
    client.leaveAllChannels();
    return ;
}