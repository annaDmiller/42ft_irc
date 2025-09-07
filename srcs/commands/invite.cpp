#include "Server.hpp"

void Server::handleInvite(Client& client, std::istringstream& args)
{
    std::string channel_name, nick, err_message, message, channel_modes;
    int target_fd;

    args >> nick >> channel_name;

    if (nick.empty() || channel_name.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), INVITE);
        client.appendSendBuffer(err_message);
        return ;
    }

    target_fd = this->findUserbyNickname(nick);
    if (target_fd == -1)
    {
        err_message = ERR_NOSUCHNICK(client.getNick(), nick);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (this->_availableChannels.find(channel_name) == this->_availableChannels.end())
    {
        err_message = ERR_NOSUCHCHANNEL(client.getNick(), channel_name);
        client.appendSendBuffer(err_message);
        return ;
    }

    Client& target_user = this->_clients[target_fd];
    if (target_user.isAlreadyJoinedChannel(channel_name))
    {
        err_message = ERR_USERONCHANNEL(client.getNick(), nick, channel_name);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (!client.isAlreadyJoinedChannel(channel_name))
    {
        err_message = ERR_NOTONCHANNEL(client.getNick(), channel_name);
        client.appendSendBuffer(err_message);
        return ;
    }

    Channel& channel = this->_availableChannels[channel_name];
    channel_modes = channel.getChannelModes();
    if (channel_modes.find('i', 0) != std::string::npos && !channel.isOperator(client.getFD()))
    {
        err_message = ERR_CHANOPRIVSNEEDED(client.getNick(), channel_name);
        client.appendSendBuffer(err_message);
        return ;
    }

    message = RPL_INVITING(client.getNick(), channel_name, nick);
    client.appendSendBuffer(message);

    message = std::string(":") + client.getPrefix() + " " + INVITE + " " + nick + " " + channel_name + TERMIN;
	std::map<int, Client>::iterator it = _clients.find(target_fd);
	if (it == _clients.end()) {
		return ;
    }
	Client& target_client = this->_clients[it->first];
	target_client.appendSendBuffer(message);

    channel.addUserToInviteList(target_fd);
    
    return ;
}