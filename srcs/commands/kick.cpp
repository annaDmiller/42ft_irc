#include "Server.hpp"

void Server::handleKickCmd(Client& client, std::istringstream& args)
{
    std::string channel_name, nick, comment, err_message, target_for_message;
    std::vector<std::string> user_list;
    std::vector<std::string>::iterator it_user;
    int user_fd;

    args >> channel_name >> nick;
    if (args.peek() == ' ')
        args.get();
    if (args.peek() == ':')
        std::getline(args, comment);
    else
        args >> comment;

    if (channel_name.empty() || nick.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), KICK);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (!isValidChannelName(channel_name))
    {
        err_message = ERR_BADCHANMASK(client.getNick(), channel_name);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (this->_availableChannels.find(channel_name) == this->_availableChannels.end())
    {
        err_message = ERR_NOSUCHCHANNEL(client.getNick(), channel_name);
        client.appendSendBuffer(err_message);
        return ;
    }


    user_list = ft_split(nick, ',');

    it_user = user_list.begin();
    while (it_user != user_list.end())
    {
        user_fd = this->findUserbyNickname(*it_user);
        if (user_fd == -1)
        {
            err_message = ERR_NOSUCHNICK(client.getNick(), *it_user);
            client.appendSendBuffer(err_message);
            it_user++;
            continue ;
        }
        
        Client& target_user = this->_clients[user_fd];

        if (!target_user.isAlreadyJoinedChannel(channel_name))
        {
            err_message = ERR_USERNOTINCHANNEL(client.getNick(), channel_name, *it_user);
            client.appendSendBuffer(err_message);
            it_user++;
            continue ;
        }

        if (!client.isAlreadyJoinedChannel(channel_name))
        {
            err_message = ERR_NOTONCHANNEL(client.getNick(), channel_name);
            client.appendSendBuffer(err_message);
            it_user++;
            continue ;
        }

        Channel& channel = this->_availableChannels[channel_name];
        if (!channel.isOperator(client.getFD()))
        {
            err_message = ERR_CHANOPRIVSNEEDED(client.getNick(), channel_name);
            client.appendSendBuffer(err_message);
            return ;
        }

        if (user_fd == client.getFD())
            return ;

        if (!comment.empty() && comment[0] == ':')
            comment = comment.substr(1);

        target_for_message = channel_name + " " + *it_user;
        channel.sendMessageToAll(client, *this, target_for_message, comment, -1, KICK);
        target_user.leaveChannel(channel_name);
        channel.removeMember(user_fd, *this);

        it_user++;
    }

    return ;
}