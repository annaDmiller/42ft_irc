#include "Server.hpp"

//NEED: to update the logic of handleMode based on test cases described in the sheet

void Server::handleMode(Client& client, std::istringstream& args)
{
    std::string channel_name, params, err_message, message;
    std::vector<std::string> params_mode;

    args >> channel_name;
    std::getline(args, params);

    if (channel_name.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), MODE);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    Channel& channel = this->_availableChannels[channel_name];
    if (!client.isAlreadyJoinedChannel(channel_name) || !channel.isOperator(client.getFD()))
    {
        err_message = ERR_CHANOPRIVSNEEDED(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    //for command MODE #channel_name, server outputs the list of channel's modes
    if (params.empty())
    {
        channel.printModes(client);
        return ;
    }

    //if the params are not empty, it means, that we are handling channel's modes somehow
    if (params[0] == ' ')
        params = params.substr(1);

    //here we split arguments of parametres and then handle them to add/remove modes
    params_mode = ft_split(params, ' ');
    message = modeHandlingChannel(client, channel, params_mode);

    //after handling is done, we need to output message for the client itself and all member of the channel
    message = client.getPrefix() + MODE + channel_name + message;
    channel.sendMessageToAll(message);

    return ;
}

std::string Server::modeHandlingChannel(Client& client, Channel& channel,
        std::vector<std::string>& params)
{
    size_t ind_param = 0, ind_mode = 0, num_modes = 0;
    std::string &modes = params[0], err_message, pass, message;
    std::vector<std::string> params_for_message;
    std::vector<char> modes_for_message;
    bool isAdding = true;
    char incorrect_mode;
    int member_limit, target_fd;

    //even though we can handle up to 3 modes at one command, we need to input all modes in 1st(!!!) param
    //That's why we check valid modes only in the first iterator of params vector

    if (!isValidModes(modes, incorrect_mode))
    {
        err_message = ERR_UNKNOWNMODE(client.getNick(), incorrect_mode);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return (std::string());
    }

    //We check whether we need to remove or add some modes to the channel
    if (modes[0] == '-')
        isAdding = false;
        
    if (modes[0] != '-' && modes[0] != '+')
        modes = std::string("+") + modes;
        
    modes_for_message.push_back(modes[0]);

    //then we handle modes one after one
    //i - to make the channel invite-only; it doesn't need any additional parametres
    //l - to set the limit for members; it requires additional parameter as limit number
    //t - to make topic editable only for operators; no additional parameters required
    //k - to set a key(password) for the channel; required a parameter equal to the new key
    //o - to provide a channel's member(!) with operator privilage; requires members' nickname as additional parameter
    while (++ind_mode < modes.size())
    {
        if (num_modes > 3)
            return (message);

        switch (modes[ind_mode])
        {
            //for this and next modes' handling:
            //after each successful mode handling we increment the number of already handled modes
            case 'i':
                if (channel.handleInviteOnly(isAdding))
                    //and add the handled mode and its parameters to the special vectors which will participate in message composing later
                    modes_for_message.push_back('i');

                num_modes++;
                break;
            
            case 'l':
                if (ind_param >= params.size())
                {
                    err_message = ERR_NEEDMOREPARAMS(client.getNick(), MODE);
                    send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                    return (std::string());
                }

                member_limit = atoi(params[ind_param++].c_str());

                if (channel.handleMemberLimit(isAdding, member_limit))
                {
                    num_modes++;
                    modes_for_message.push_back('l');
                    if (member_limit != -1 && !isAdding)
                        params_for_message.push_back(params[ind_param - 1]);
                }

                break ;
            
            case 't':
                if (channel.handleTopicOper(isAdding))
                    modes_for_message.push_back('t');

                num_modes++;
                break ;
            
            case 'k':
                if (ind_param < params.size())
                    pass = params[ind_param++];

                if (channel.handleKey(isAdding, pass, client))
                {
                    num_modes++;
                    modes_for_message.push_back('k');
                    if (!pass.empty() && !isAdding)
                        params_for_message.push_back(params[ind_param - 1]);
                }

                break ;
            
            case 'o':
                if (ind_param >= params.size())
                {
                    err_message = ERR_NEEDMOREPARAMS(client.getNick(), MODE);
                    send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                    return (std::string());
                }

                target_fd = findUserbyNickname(params[ind_param++]);
                if (target_fd == -1)
                {
                    err_message = ERR_NOSUCHNICK(client.getNick(), params[ind_param - 1]);
                    send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                    break ;
                }

                if (channel.handleOperators(isAdding, target_fd, client, params[ind_param - 1]))
                {
                    num_modes++;
                    modes_for_message.push_back('o');
                    if (target_fd != -1)
                        params_for_message.push_back(params[ind_param - 1]);
                }

                break ;
            
            default:
                break ;
        }
    }

    message = composeMessage(modes_for_message, params_for_message);
    return (message);
}

bool Server::isValidModes(const std::string& modes, char& incorrect_mode)
{
    std::vector<char> allowed_chars;
    allowed_chars.push_back('+');
    allowed_chars.push_back('-');
    allowed_chars.push_back('i');
    allowed_chars.push_back('l');
    allowed_chars.push_back('t');
    allowed_chars.push_back('k');
    allowed_chars.push_back('o');

    for (size_t ind = 0; ind < modes.size(); ind++)
    {
        if (std::find(allowed_chars.begin(), allowed_chars.end(), modes[ind]) == allowed_chars.end())
        {
            incorrect_mode = modes[ind];
            return (false);
        }
    }
    return (true);
}

//here we compose the message for channel's members out of 2 vectors: modes and its parameters
std::string Server::composeMessage(std::vector<char>& modes, std::vector<std::string>& params) const
{
    std::string message;
    char car;
    
    for (size_t ind = 0; ind < modes.size(); ind++)
    {
        car = modes[ind];
        message += car;
    }
    
    for (size_t ind = 0; ind < params.size(); ind++)
    {
        message += std::string(" ");
        message += params[ind];
    }

    return (message);
}