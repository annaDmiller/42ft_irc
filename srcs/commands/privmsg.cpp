#include "Server.hpp"

void Server::handlePrivmsgCmd(Client& client, std::istringstream& args)
{
    std::string receivers, message, err_message;
    std::vector<std::string> rec_list;
    int fd_target;
    
    args >> receivers;
    if (args.peek() == ' ')
        args.get();
    if (args.peek() == ':')
        std::getline(args, message);
    else
        args >> message;

    if (receivers.empty())
    {
        err_message = ERR_NORECIPIENT(client.getNick(), PRIVMSG);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (message.empty() || message.find_first_not_of(" \t\n\v\f\r") == std::string::npos)
    {
        err_message = ERR_NOTEXTTOSEND(client.getNick());
        client.appendSendBuffer(err_message);
        return ;
    }

    if (message[0] == ' ')
        message = message.substr(1);
    if (message[0] == ':')
        message = message.substr(1);

    rec_list = ft_split(receivers, ',');

    for (size_t ind = 0; ind < rec_list.size(); ind++)
    {
        if (isChannelTarget(rec_list[ind]))
        {
            if (!isChannelExist(rec_list[ind]))
            {
                err_message = ERR_NOSUCHNICK(client.getNick(), rec_list[ind]);
                client.appendSendBuffer(err_message);
                continue ;
            }

            Channel& channel = this->_availableChannels[rec_list[ind]];
            if (!channel.userIsMember(client.getFD()))
            {
                err_message = ERR_CANNOTSENDTOCHAN(client.getNick(), channel.getName());
                client.appendSendBuffer(err_message);
                continue ;
            }
            
            channel.sendMessageToAll(client, *this, rec_list[ind], message, client.getFD(), PRIVMSG);
        }
        else
        {
            if ((fd_target = this->findUserbyNickname(rec_list[ind])) == -1)
            {
                err_message = ERR_NOSUCHNICK(client.getNick(), rec_list[ind]);
                client.appendSendBuffer(err_message);
                continue ;
            }
            
            sendMessageToUser(client, fd_target, rec_list[ind], message, PRIVMSG);
        }
    }
    return ;
}

bool Server::isChannelTarget(const std::string& target) const
{
    if (target.empty())
        return (false);

    char c = target[0];
    if (c == '#' || c == '&' || c == '+')
        return (true);
    return (false);
}