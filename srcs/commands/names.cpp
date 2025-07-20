# include "Server.hpp"

void Server::handleNames(Client& client, std::istringstream& args)
{
    std::string channels;
    std::vector<std::string> channel_list;
    args >> channels;

    if (!channels.empty())
        channel_list = ft_split(channels, ',');
    else
    {
        for (std::map<std::string, Channel>::iterator it = this->_availableChannels.begin();
                it != this->_availableChannels.end(); it++)
            channel_list.push_back(it->first);
    }

    for (size_t ind = 0; ind < channel_list.size(); ind++)
    {
        if (!this->isChannelExist(channel_list[ind]))
            continue ;
        Channel& channel = this->_availableChannels[channel_list[ind]];
        channel.sendMemberList(client);
    }
    return ;
}