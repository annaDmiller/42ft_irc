# include "Server.hpp"

void Server::handleNickname(Client& client, std::istringstream& args)
{
    std::string nick, err_response;
    args >> nick;

    if (!client.isPassChecked())
    {
        err_response = "ERROR :Password required\r\n";
        send(client.getFD(), err_response.c_str(), err_response.size(), 0);
        return ;
    }

    if (nick.empty())
    {
        err_response = ERR_NONICKNAMEGIVEN(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }
    
    if (this->checkDupNicknamesOnServer(nick))
    {
        err_response = ERR_NICKNAMEINUSE(client.getNick(), nick);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    if (!this->isValidNickname(nick))
    {
        err_response = ERR_ERRONEUSNICKNAME(client.getNick(), nick);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    //If the already registered user changes the nick, the proper message is sent to all joined channels
    if (client.isRegistered())
        client.sendToAllJoinedChannels(*this, nick, NICK, true, false);

    client.setNickname(nick);

    return ;
}

bool Server::isValidNickname(const std::string& nick) const
{
    if (!std::isalpha(nick[0]) && !isSpecial(nick[0]))
        return (false);
    
    if (nick.length() > 9)
        return (false);

    for (size_t ind = 1; ind < nick.length(); ind++)
    {
        if (!std::isalnum(nick[ind]) && !isSpecial(nick[ind]) && nick[ind] != '-')
            return (false);
    }
    return (true);
}

bool Server::checkDupNicknamesOnServer(std::string& nick)
{
    std::string upper_nick, tmp_nick;

    upper_nick = toUpperString(nick);
    std::cout << "upper_nick: " << upper_nick << std::endl;//test
    std::cout << "this->_clients.size(): " << this->_clients.size() << std::endl;//test
    for (size_t ind = 0; ind < this->_clients.size(); ind++)
    {
        tmp_nick = toUpperString(this->_clients[ind].getNick());
        std::cout << "tmp_nick: " << tmp_nick << std::endl;//test
        if (this->_clients[ind].isHaveNick() && tmp_nick == upper_nick)
            return (true);
    }
    return (false);
}

std::string Server::toUpperString(std::string str)
{
    std::string new_str(str);

    for (size_t i = 0; i < str.size(); i++)
        new_str[i] = std::toupper(str[i]);
    return (new_str);
}

