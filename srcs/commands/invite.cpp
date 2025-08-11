#include "Server.hpp"

void Server::handleInvite(Client& client, std::istringstream& args)
{
    //RPL_INVITING
    std::string channel_name, nick, err_message, message, channel_modes;
    int target_fd;

    args >> nick >> channel_name;

    if (nick.empty() || channel_name.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), INVITE);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    target_fd = this->findUserbyNickname(nick);
    if (target_fd == -1)
    {
        err_message = ERR_NOSUCHNICK(client.getNick(), nick);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (this->_availableChannels.find(channel_name) == this->_availableChannels.end())
    {
        err_message = ERR_NOSUCHCHANNEL(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    Client& target_user = this->_clients[target_fd];
    if (target_user.isAlreadyJoinedChannel(channel_name))
    {
        err_message = ERR_USERONCHANNEL(client.getNick(), nick, channel_name);
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
    channel_modes = channel.getChannelModes();
    if (channel_modes.find('i', 0) != std::string::npos && !channel.isOperator(client.getFD()))
    {
        err_message = ERR_CHANOPRIVSNEEDED(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    message = RPL_INVITING(client.getNick(), channel_name, nick);
    send(client.getFD(), message.c_str(), message.size(), 0);

    message = client.getPrefix() + INVITE + nick + channel_name + TERMIN;
    send(target_fd, message.c_str(), message.size(), 0);
    channel.addUserToInviteList(target_fd);
    
    return ;
}