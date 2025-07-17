# include "Server.hpp"

void Server::handleNickname(Client& client, std::istringstream& args)
{
    std::string nick, err_response;
    args >> nick;

    //NEED: Add logic to check password presence before check of nickname - shall we send error?

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

    if (client.isRegistered())
        client.sendToAllJoinedChannels(*this, nick, NICK);

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
    for (size_t ind = 0; ind < this->_clients.size(); ind++)
    {
        if (this->_clients[ind].isHaveNick() && this->_clients[ind].getNick() == nick)
            return (true);
    }
    return (false);
}
