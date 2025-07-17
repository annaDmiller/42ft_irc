#include "Server.hpp"

void Server::handlePrivateMessage(Client& client, std::istringstream& args)
{
    std::string receivers, message, err_message;
    std::vector<std::string> rec_list;
    int fd_target;
    
    args >> receivers;
    std::getline(args, message);

    //Firstly, we check whether we have empty arguments for the command
    if (receivers.empty())
    {
        err_message = ERR_NORECIPIENT(client.getNick(), PRIVMSG);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (message.empty())
    {
        err_message = ERR_NOTEXTTOSEND(client.getNick());
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    //The message must start with semicolon char to be processed correctly
    if (message[0] == ':')
        message = message.substr(1);

    //As there can be multiple receivers, create a list of them
    rec_list = ft_split(receivers, ',');

    for (size_t ind = 0; ind < rec_list.size(); ind++)
    {
        //for each receiver, we must check whether it is channel or a user - different behavior
        if (isChannelTarget(rec_list[ind]))
        {
            //handle channel
            if (!isChannelExist(rec_list[ind]))
            {
                err_message = ERR_NOSUCHNICK(client.getNick(), rec_list[ind]);
                send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                continue ;
            }
            
            Channel& channel = this->_availableChannels[rec_list[ind]];
            if (!channel.userIsMember(client.getFD()))
            {
                err_message = ERR_CANNOTSENDTOCHAN(client.getNick(), channel.getName());
                send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                continue ;
            }
            
            //send the message to all members of the channel except for the user itself
            channel.sendMessageToAll(client, *this, rec_list[ind], message, client.getFD());
        }
        else
        {
            //handle user
            if ((fd_target = this->findUserbyNickname(rec_list[ind])) == -1)
            {
                err_message = ERR_NOSUCHNICK(client.getNick(), rec_list[ind]);
                send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                continue ;
            }
            
            //send the message to the target-user
            sendMessageToUser(client, fd_target, rec_list[ind], message);
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