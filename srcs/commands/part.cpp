#include "Server.hpp"

void Server::handlePartCmd(Client& client, std::istringstream& args)
{
    std::string channels, message, err_message;
    std::vector<std::string> channel_list;
    std::string channel_name;

    args >> channels;
    if (args.peek() == ' ')
        args.get();
    if (args.peek() == ':')
        std::getline(args, message);
    else
        args >> message;

    if (channels.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), PART);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (!message.empty())
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
            client.appendSendBuffer(err_message);
            continue ;
        }

        Channel& channel = this->_availableChannels[channel_list[ind]];
        channel_name = channel.getName();
        if (!client.isAlreadyJoinedChannel(channel.getName()))
        {
            err_message = ERR_NOTONCHANNEL(client.getNick(), channel.getName());
            client.appendSendBuffer(err_message);
            continue ;
        }

        channel.sendMessageToAll(client, *this, channel.getName(), message, -1, PART);
        channel.removeMember(client.getFD(), *this);

        client.leaveChannel(channel_name);
    }
}

void Server::handlePartCmd(Client& client)
{
    client.sendToAllJoinedChannels(*this, "", PART, false, true);
    client.leaveAllChannels(*this);
    return ;
}