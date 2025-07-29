#include "Server.hpp"

//NEED: to handle the message sending to the users about mode change
//NEED: to test more cases with modes adding/removing with one/multiple modes and check messages and effect

void Server::handleMode(Client& client, std::istringstream& args)
{
    std::string channel_name, params, err_message, message;
    std::vector<std::string> params_mode;
    size_t params_for_message;

    args >> channel_name;
    std::getline(args, params);

    if (channel_name.empty() || params.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), MODE);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
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
    params_for_message = modeHandlingChannel(client, channel, params_mode);

    return ;
}

size_t Server::modeHandlingChannel(Client& client, Channel& channel,
        std::vector<std::string>& params)
{
    size_t ind_param = 1, ind_mode = 0;
    std::string& modes = params[0], err_message, pass;
    bool isAdding = true;
    bool enoughParams = true;
    char incorrect_mode;
    int member_limit, target_fd;

    if (!isValidModes(modes, incorrect_mode))
    {
        //NEED: ERR_UNKNOWNMODE error
        return ;
    }

    if (modes[0] == '-')
    {
        isAdding = false;
    }

    if (modes[0] != '-' && modes[0] != '+')
        modes = std::string("+") + modes;

    while (++ind_mode < modes.size())
    {
        if (ind_mode > 3)
            break ;

        switch (modes[ind_mode])
        {
            case 'i':
                if (isAdding)
                    channel.addMode('i');
                else
                    channel.removeMode('i');

                break;
            
            case 'l':
                if (ind_param >= params.size())
                    break ;

                member_limit = atoi(params[ind_param++].c_str());
                channel.handleMemberLimit(isAdding, member_limit);

                break ;
            
            case 't':
                if (isAdding)
                    channel.addMode('t');
                else
                    channel.removeMode('t');

                break ;
            
            case 'k':
                if (ind_param >= params.size())
                    break ;

                pass = params[ind_param++];
                channel.handleKey(isAdding, pass, client);

                break ;
            
            case 'o':
                if (ind_param >= params.size())
                    break ;

                target_fd = findUserbyNickname(params[ind_param++]);
                channel.handleOperators(isAdding, target_fd);

                break ;
            
            default:
                break ;
        }
    }

    return ;
}

bool Server::isValidModes(const std::string& modes, char& incorrect_mode)
{
    std::vector<char> allowed_chars = {'+', '-', 'i', 'l', 't', 'k', 'o'};

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