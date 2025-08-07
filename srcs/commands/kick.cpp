#include "Server.hpp"

void Server::handleKick(Client& client, std::istringstream& args)
{
    std::string channel_name, nick, comment, err_message, target_for_message;
    int user_fd;

    args >> channel_name >> nick;
    if (args.peek() == ':')
        std::getline(args, comment);
    else
        args >> comment;

    if (channel_name.empty() || nick.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), KICK);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (!isValidChannelName(channel_name))
    {
        err_message = ERR_BADCHANMASK(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (this->_availableChannels.find(channel_name) == this->_availableChannels.end())
    {
        err_message = ERR_NOSUCHCHANNEL(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    user_fd = this->findUserbyNickname(nick);
    if (user_fd == -1)
    {
        err_message = ERR_NOSUCHNICK(client.getNick(), nick);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }
    
    Client& target_user = this->_clients[user_fd];

    if (!target_user.isAlreadyJoinedChannel(channel_name))
    {
        err_message = ERR_USERNOTINCHANNEL(client.getNick(), channel_name, nick);
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
    if (!channel.isOperator(client.getFD()))
    {
        err_message = ERR_CHANOPRIVSNEEDED(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (!comment.empty() && comment[0] == ':')
        comment = comment.substr(1);

    target_for_message = channel_name + " " + nick;
    channel.sendMessageToAll(client, *this, target_for_message, comment, -1, KICK);
    target_user.leaveChannel(channel_name);
    channel.removeMember(user_fd, *this);
    
    return ;
}