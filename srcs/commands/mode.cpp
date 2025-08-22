#include "Server.hpp"

void Server::handleMode(Client& client, std::istringstream& args)
{
    std::string channel_name, params, err_message, message;
    std::vector<std::string> params_mode;

    args >> channel_name;

    std::cout << "channel_name: " << channel_name << "!" << std::endl; //test

    std::getline(args, params);

    if (channel_name.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), MODE);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (checkDupNicknamesOnServer(channel_name) == true) //test
        return ;

    if (isChannelExist(channel_name) == false)
    {
        std::cout << "handleMode 1: " << std::endl; //test
        err_message = ERR_NOSUCHCHANNEL(client.getNick(), channel_name);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    Channel& channel = this->_availableChannels[channel_name];
    if (!client.isAlreadyJoinedChannel(channel_name) || !channel.isOperator(client.getFD()))
    {
        std::cout << "handleMode 2: " << std::endl; //test
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

    if (message.size() > 0) //test
    {
        //after handling is done, we need to output message for the client itself and all member of the channel
        // message = client.getPrefix() + MODE + channel_name + message;
        message = client.getPrefix() + " " + MODE + " " + channel_name + " " + message + TERMIN;//test
        channel.sendMessageToAll(message);
    }
    return ;
}

std::string Server::modeHandlingChannel(Client& client, Channel& channel,
        std::vector<std::string>& params)
{
    std::cout << "modeHandlingChannel" << std::endl; //test
    size_t ind_param = 1, ind_mode = 0;
    std::string &modes = params[0], err_message, pass, message, tmp_param;
    std::vector<std::string> params_for_message;
    std::vector<char> modes_for_message;
    bool isAdding = true;
    // char incorrect_mode;
    int member_limit = -1, target_fd;
    long limit;
    char *pscalar_end;

     std::cout << "modes: " << modes << std::endl; //test


    //even though we can handle up to 3 modes at one command, we need to input all modes in 1st(!!!) param
    //That's why we check valid modes only in the first iterator of params vector

    //test a modifier et executer les modes valides precedant le mauvais mode
    // if (!isValidModes(modes, incorrect_mode))
    // {
    //     err_message = ERR_UNKNOWNMODE(client.getNick(), incorrect_mode);
    //     send(client.getFD(), err_message.c_str(), err_message.size(), 0);
    //     return (std::string());
    // }

    if (modes[0] != '-' && modes[0] != '+')
        modes = std::string("+") + modes;

    //then we handle modes one after one
    //i - to make the channel invite-only; it doesn't need any additional parametres
    //l - to set the limit for members; it requires additional parameter as limit number
    //t - to make topic editable only for operators; no additional parameters required
    //k - to set a key(password) for the channel; required a parameter equal to the new key
    //o - to provide a channel's member(!) with operator privilage; requires members' nickname as additional parameter
    while (ind_mode < modes.size())
    {
        std::cout << "1.mode[" << ind_mode << "]: " << modes[ind_mode] << std::endl; //test
        if (modes[ind_mode] == '-')
        {
            isAdding = false;
            if (ind_mode + 1 < modes.size() && isValidMode(modes[ind_mode + 1]) == true)
                modes_for_message.push_back(modes[ind_mode]);
            ind_mode++;
        }
        else if (modes[ind_mode] == '+')
        {
            isAdding = true;
            if (ind_mode + 1 < modes.size() && isValidMode(modes[ind_mode + 1]) == true)
                modes_for_message.push_back(modes[ind_mode]);
            ind_mode++;
        }

        if (ind_mode >= modes.size())
            break;
        std::cout << "2.mode[" << ind_mode << "]: " << modes[ind_mode] << std::endl; //test
        switch (modes[ind_mode])
        {
            //for this and next modes' handling:
            //after each successful mode handling we increment the number of already handled modes
            case 'i':
                if (channel.handleInviteOnly(isAdding))
                    //and add the handled mode and its parameters to the special vectors which will participate in message composing later
                    modes_for_message.push_back('i');
                break;
            
            case 'l':
                if (isAdding)
                {
                    if (ind_param >= params.size())
                    {
                        err_message = ERR_NEEDMOREPARAMS(client.getNick(), MODE);
                        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                        return (std::string());
                    }
                    tmp_param = params[ind_param];
                    limit = std::strtol(params[ind_param++].c_str(), &pscalar_end, 10);
                    if (*pscalar_end != '\0' || limit == LONG_MIN || limit == LONG_MAX
                        || limit < std::numeric_limits<int>::min() || limit > std::numeric_limits<int>::max())
                    {
                        err_message = ERR_INVALIDMODEPARAM(client.getNick(), channel.getName(), "l", tmp_param, "Invalid limit");
                        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                        return (std::string());
                    }
                    member_limit = static_cast<int>(limit);
                }
                if (channel.handleMemberLimit(isAdding, member_limit))
                {
                    modes_for_message.push_back('l');
                    // if (member_limit != -1 && !isAdding)
                    if (member_limit != -1 && isAdding)
                        params_for_message.push_back(params[ind_param - 1]);
                }
                member_limit = -1;
                break ;
            
            case 't':
                if (channel.handleTopicOper(isAdding))
                    modes_for_message.push_back('t');
                break ;
            
            case 'k':
                if (isAdding && ind_param < params.size())
                        pass = params[ind_param++];

                if (channel.handleKey(isAdding, pass, client))
                {
                    modes_for_message.push_back('k');
                    if (isAdding)
                        params_for_message.push_back(params[ind_param - 1]);
                    else
                        params_for_message.push_back(std::string("*"));
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

                if (target_fd == client.getFD() && isAdding)
                    break ;

                if (channel.handleOperators(isAdding, target_fd, client, params[ind_param - 1]))
                {
                    modes_for_message.push_back('o');
                    if (target_fd != -1)
                        params_for_message.push_back(params[ind_param - 1]);
                }
                break ;
            
            default:
                err_message = ERR_UNKNOWNMODE(client.getNick(), modes[ind_mode]);
                send(client.getFD(), err_message.c_str(), err_message.size(), 0);
                removeOperMode(modes_for_message);
                message = composeMessage(modes_for_message, params_for_message);
                return (message);         
                break ;
        }
        ind_mode++;
    }
    removeOperMode(modes_for_message);
    message = composeMessage(modes_for_message, params_for_message);
    return (message);
}

bool Server::isValidMode(char mode)
{
    std::vector<char> allowed_chars;
    allowed_chars.push_back('i');
    allowed_chars.push_back('l');
    allowed_chars.push_back('t');
    allowed_chars.push_back('k');
    allowed_chars.push_back('o');

    for (size_t ind = 0; ind < allowed_chars.size(); ind++)
    {
        if (allowed_chars[ind] == mode)
            return (true);
    }
    return (false);
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

void Server::removeOperMode(std::vector<char>& modes)
{
    size_t ind = 0;
    while (ind < modes.size())
    {
        if ((modes[ind] == '-' || modes[ind] == '+')
            && (ind >= (modes.size() - 1) || isValidMode(modes[ind + 1] == false)))
        {
            modes.erase(modes.begin() + ind);
            ind = 0;
        }
        else
            ind++;
    }
    return ;
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