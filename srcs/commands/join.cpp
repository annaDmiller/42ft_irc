# include "Server.hpp"

void Server::handleJoin(Client& client, std::istringstream& args)
{
    std::string channel, key, err_response, temp_str;
    std::vector<std::string> channel_list, key_list;
    args >> channel >> key;
    std::vector<std::string>::iterator it_channel, it_key, it_temp;
    size_t temp_ind;

    if (channel.empty())
    {
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), JOIN);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    if (channel[0] == '0')
    {
        // run PART command
        return ;
    }

    channel_list = ft_split(channel, ',');
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
        // = send ERR_BADCHANMASK
        temp_ind =  it_channel - channel_list.begin();
        it_key = key_list.begin() + temp_ind;
        it_temp = it_channel + 1;
        channel_list.erase(it_channel);
        it_channel = it_temp;
        key_list.erase(it_key);
    }
    if (channel_list.empty())
        return ;

    for (size_t ind = 0; ind < channel_list.size(); ind++)
    {
        channel_list[ind] = channel_list[ind].substr(1);

        if (client.isAlreadyJoinedChannel(channel_list[ind]))
            continue ;
        
        if (!isChannelExist(channel_list[ind]))
        {
            Channel new_channel;
            new_channel.setName(channel_list[ind]);
            this->_availableChannels[channel_list[ind]] = new_channel;
        }
        Channel& channel = this->_availableChannels[channel_list[ind]];

        if (client.joinedChannelQuantity() >= MAXJOINEDCHANNELS)
        {
            // = send ERR_TOOMANYCHANNELS
            continue ;
        }
        
        //NEED: if the channel's key is incorrect - the passed key doesn't equal to Channel's key
        // = send ERR_BADCHANNELKEY

        //NEED: if the channel has mode "+i", the user can join it only by invitation
        // = send ERR_INVITEONLYCHAN

        //NEED: if the channel has mode "+l", check the limit of members. if the channel is out of limit
        // = send ERR_CHANNELISFULL

        //NEED: to join the channel
        //NEED: to add user to members map for the Channel
        //NEED: to add the channel to the joined map for the Client
        //NEED: to check - if the user is only one in the members, he will be its operator

        //NEED: to send the reply to the joined user
        //NEED: to send the message of joining to all the member of that Channel
    }
    
    return ;
}

bool Server::isValidChannelName(const std::string& chan_name)
{
    if (chan_name[0] != '#' && chan_name[0] != '&'
            && chan_name[0] != '+')
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

bool Server::isChannelExist(std::string &channel_name)
{
    for (std::map<std::string, Channel>::iterator it_map = this->_availableChannels.begin();
        it_map != this->_availableChannels.end(); it_map++)
    {
        if (channel_name == it_map->first)
            return (true);
    }
    return (false);
}