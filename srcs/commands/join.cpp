# include "Server.hpp"

void Server::handleJoinCmd(Client& client, std::istringstream& args)
{
    std::string channel_name, key, err_response, temp_str, channel_modes;
    std::vector<std::string> channel_list, key_list;
    std::vector<std::string>::iterator it_channel, it_key;
    size_t temp_ind;

    args >> channel_name >> key;

    if (channel_name.empty())
    {
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), JOIN);
        client.appendSendBuffer(err_response);
        return ;
    }

    if (channel_name == "0")
    {
        this->handlePartCmd(client);
        return ;
    }

    channel_list = ft_split(channel_name, ',');
    if (!key.empty())
        key_list = ft_split(key, ',');
    if (key_list.size() != channel_list.size())
    {
        for (size_t ind = key_list.size(); ind < channel_list.size(); ind++)
            key_list.push_back("");
    }

    it_channel = channel_list.begin();
    while (it_channel != channel_list.end())
    {
        if (isValidChannelName(*it_channel))
        {
            it_channel++;
            continue ;
        }
        err_response = ERR_BADCHANMASK(client.getNick(), *it_channel);
        client.appendSendBuffer(err_response);
        temp_ind = it_channel - channel_list.begin();
        it_key = key_list.begin() + temp_ind;
        channel_list.erase(it_channel);
        it_channel = channel_list.begin() + temp_ind;
        key_list.erase(it_key);
    }

    if (channel_list.empty())
        return ;

    for (size_t ind = 0; ind < channel_list.size(); ind++)
    {
        if (client.isAlreadyJoinedChannel(channel_list[ind]))
            continue ;

        if (client.getJoinedChannelQuantity() >= MAXJOINEDCHANNELS)
        {
            err_response = ERR_TOOMANYCHANNELS(client.getNick(), channel_list[ind]);
            client.appendSendBuffer(err_response);
            continue ;
        }

        if (!isChannelExist(channel_list[ind]))
        {
            Channel new_channel;
            new_channel.setName(channel_list[ind]);
            new_channel.addOperator(client.getFD());
            new_channel.checkJustCreated();
            new_channel.setCreationTime();
            this->_availableChannels[channel_list[ind]] = new_channel;
        }
        else
           channel_list[ind] = getChannelName(channel_list[ind]);

        Channel& channel = this->_availableChannels[channel_list[ind]];

        channel_modes = channel.getChannelModes();
        if (channel_modes.find('i', 0) != std::string::npos && !channel.isUserInvited(client.getFD()))
        {
            err_response = ERR_INVITEONLYCHAN(client.getNick(), channel.getName());
            client.appendSendBuffer(err_response);
            continue ;
        }

        if (channel_modes.find('l', 0) != std::string::npos && !channel.canBeJoined())
        {
            err_response = ERR_CHANNELISFULL(client.getNick(), channel.getName());
            client.appendSendBuffer(err_response);
            continue ;
        }

        if (channel_modes.find('k', 0) != std::string::npos && !channel.isKeyCorrect(key_list[ind]))
        {
            err_response = ERR_BADCHANNELKEY(client.getNick(), channel.getName());
            client.appendSendBuffer(err_response);
            continue ;
        }
        
        channel.addMember(client.getFD(), &client);
        client.addChannel(channel.getName(), &channel);
        channel.sendJoinMessages(client);
    }
    
    return ;
}

bool Server::isValidChannelName(const std::string& chan_name) const
{
    if (chan_name[0] != '#')        
        return (false);

    if (chan_name.length() == 1)
        return (false);

    for (size_t ind = 1; ind < chan_name.size(); ind++)
    {
        if (isForbiddenForChannelName(chan_name[ind]) || chan_name[ind] == 0)
            return (false);
    }    
    return (true);
}

std::string Server::getChannelName(std::string &channel_name)
{
    std::string upper_name, tmp_name;

    upper_name = toUpperString(channel_name);
    for (std::map<std::string, Channel>::const_iterator it_map = this->_availableChannels.begin();
        it_map != this->_availableChannels.end(); it_map++)
    {
        tmp_name = toUpperString(it_map->first);
        if (upper_name == tmp_name)
            return (it_map->first);
    }
    return ("");
}

bool Server::isChannelExist(std::string &channel_name)
{
    std::string upper_name, tmp_name;

    upper_name = toUpperString(channel_name);

    for (std::map<std::string, Channel>::const_iterator it_map = this->_availableChannels.begin();
        it_map != this->_availableChannels.end(); it_map++)
    {
        tmp_name = toUpperString(it_map->first);
        if (upper_name == tmp_name)
            return (true);
    }
    return (false);
}