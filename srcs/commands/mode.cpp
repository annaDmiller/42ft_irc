#include "Server.hpp"

//NEED: to handle the message sending to the users about mode change
//NEED: to test more cases with modes adding/removing with one/multiple modes and check messages and effect

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

    if (params.empty())
    {
        //NEED: print already applied modes for the channel - reply 324, 329
        return ;
    }

    if (!client.isAlreadyJoinedChannel(channel_name))
    {
        //NEED: ERR_USERNOTINCHANNEL error
        return ;
    }

    Channel& channel = this->_availableChannels[channel_name];
    if (!channel.isOperator(client.getFD()))
    {
        //NEED: ERR_CHANOPRIVSNEEDED error
        return ;
    }

    if (params[0] == ' ')
        params = params.substr(1);

    params_mode = ft_split(params, ' ');
    message = modeHandlingChannel(client, channel, params_mode);

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

    if (!isValidModes(modes, incorrect_mode))
    {
        //NEED: ERR_UNKNOWNMODE error
        return (std::string());
    }

    if (modes[0] == '-')
        isAdding = false;
        
    if (modes[0] != '-' && modes[0] != '+')
        modes = std::string("+") + modes;
        
    modes_for_message.push_back(modes[0]);

    while (++ind_mode < modes.size())
    {
        if (num_modes > 3)
            return (message);

        switch (modes[ind_mode])
        {
            case 'i':
                if (isAdding)
                    channel.addMode('i');
                else
                    channel.removeMode('i');

                num_modes++;
                modes_for_message.push_back('i');

                break;
            
            case 'l':
                if (ind_param >= params.size())
                {
                    //NEED: error 461
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
                if (isAdding)
                    channel.addMode('t');
                else
                    channel.removeMode('t');

                num_modes++;
                modes_for_message.push_back('t');

                break ;
            
            case 'k':
                if (ind_param >= params.size())
                {
                    //NEED: error 461
                    return (std::string());
                }

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
                    //NEED: error 461
                    return (std::string());
                }

                target_fd = findUserbyNickname(params[ind_param++]);
                if (target_fd == -1)
                {
                    //NEED: error 401
                    break ;
                }

                if (channel.handleOperators(isAdding, target_fd))
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