# include "Server.hpp"

void Server::handleJoin(Client& client, std::istringstream& args)
{
    std::string channel, key, err_response, temp_str, channel_modes;
    std::vector<std::string> channel_list, key_list;
    std::vector<std::string>::iterator it_channel, it_key, it_temp;
    size_t temp_ind;

    //firstly, we separate the words from args stream into channel (channel_name) and key strings
    args >> channel >> key;

    if (channel.empty())
    {
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), JOIN);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    //The message '0' is handled by JOIN as PART command
    if (channel[0] == '0')
    {
        //NEED: run PART command
        return ;
    }

    //We split our channel and key strings into the vectors to handle joining of multiple channels per command
    channel_list = ft_split(channel, ',');
    if (!key.empty())
        key_list = ft_split(key, ',');
    if (key_list.size() != channel_list.size())
    {
        //if there are less keys than the channels name, we adjust sizes of vectors with empty lines
        for (size_t ind = key_list.size(); ind < channel_list.size(); ind++)
            key_list.push_back("");
    }

    //we check that the channel_name has correct mask. If it's not, we send error and erase it from the vector
    it_channel = channel_list.begin();
    while (it_channel != channel_list.end())
    {
        if (isValidChannelName(*it_channel))
        {
            it_channel++;
            continue ;
        }
        err_response = ERR_BADCHANMASK(client.getNick(), *it_channel);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        temp_ind =  it_channel - channel_list.begin();
        it_key = key_list.begin() + temp_ind;
        it_temp = it_channel + 1;
        channel_list.erase(it_channel);
        it_channel = it_temp;
        key_list.erase(it_key);
    }
    //if now we don't have any values in the channels vector, then we just return
    if (channel_list.empty())
        return ;

    //now, we iterater channels one by one from the vector
    for (size_t ind = 0; ind < channel_list.size(); ind++)
    {
        //if client has already joined it, we just silently skip this channel
        if (client.isAlreadyJoinedChannel(channel_list[ind]))
            continue ;

            
        //we check if the client reached the limit of maximum number of joined channels for user
        if (client.joinedChannelQuantity() >= MAXJOINEDCHANNELS)
        {
            err_response = ERR_TOOMANYCHANNELS(client.getNick(), channel_list[ind]);
            send(client.getFD(), err_response.c_str(), err_response.length(), 0);
            continue ;
        }

        //if channel doesn't exist, then we create it
        if (!isChannelExist(channel_list[ind]))
        {
            Channel new_channel;
            new_channel.setName(channel_list[ind]);
            new_channel.addOperator(client.getFD());
            this->_availableChannels[channel_list[ind]] = new_channel;
        }
        Channel& channel = this->_availableChannels[channel_list[ind]];

        //then we check the modes of the channel and whether our client follows all the restrictions to join it
        channel_modes = channel.getChannelModes();
        //is channel invite-only?
        if (channel_modes.find('i', 0) != std::string::npos)
        {
            err_response = ERR_INVITEONLYCHAN(client.getNick(), channel.getName());
            send(client.getFD(), err_response.c_str(), err_response.length(), 0);
            continue ;
        }

        //is there any limit for joined members for channel?
        if (channel_modes.find('l', 0) != std::string::npos && !channel.canBeJoined())
        {
            err_response = ERR_CHANNELISFULL(client.getNick(), channel.getName());
            send(client.getFD(), err_response.c_str(), err_response.length(), 0);
            continue ;
        }

        //does the channel have password? Is the key from command correct?
        if (channel_modes.find('k', 0) != std::string::npos && !channel.isKeyCorrect(key_list[ind]))
        {
            err_response = ERR_BADCHANMASK(client.getNick(), channel.getName());
            send(client.getFD(), err_response.c_str(), err_response.length(), 0);
            continue ;
        }
        
        //if everything is fine, the user can join the channel. We add client to its members. We add channel to the joined channels of the client
        channel.addMember(client.getFD(), &client);
        client.addChannel(channel.getName(), &channel);
        //and lastly, we send Welcome messages to the client and notify the rest members about his joining
        channel.sendJoinMessages(client);
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