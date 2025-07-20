#include "Server.hpp"

void Server::handleNotice(Client& client, std::istringstream& args)
{
    std::string receivers, message;
    std::vector<std::string> rec_list;
    int client_fd;

    args >> receivers;
    std::getline(args, message);

    if (receivers.empty() || message.empty())
        return ;

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
                continue ;
            
            Channel& channel = this->_availableChannels[rec_list[ind]];
            if (!channel.userIsMember(client.getFD()))
                continue ;

            channel.sendMessageToAll(client, *this, rec_list[ind], message, client.getFD(), NOTICE);
        }
        else
        {
            if ((client_fd = this->findUserbyNickname(rec_list[ind])) == -1)
                continue ;

            sendMessageToUser(client, client_fd, rec_list[ind], message, NOTICE);
        }
    }
    
    return ;
}