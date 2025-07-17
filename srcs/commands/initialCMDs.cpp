#include "Server.hpp"

void Server::handleInitCommands(Client& client, std::string& cmd, std::istringstream& args)
{
    //NEED: TO ADD LOGIC TO CHECK IF THERE IS PASSWORD IN NICK AND USER COMMANDS - SHALL WE SEND AN ERROR?
    std::string welcome_mess, err_response;
    //as we have initial commands in uppercase, we need to transfrom our command
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    if (cmd == NICK)
        handleNickname(client, args);
    else if (cmd == USER)
        handleUsername(client, args);
    else if (cmd == PASS)
        handlePassword(client, args);
    else if (cmd == QUIT)
    {
        handleQuit(client, args);
        return ;
    }
    else
    {
        err_response = ERR_NOTREGISTERED(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }
    
    if (client.tryAuthenticate())
    {
        welcome_mess = RPL_WELCOME(client.getNick(), client.getUsername(), HOST);
        send(client.getFD(), welcome_mess.c_str(), welcome_mess.length(), 0);
        welcome_mess = RPL_YOURHOST(client.getNick(), SERVERNAME, VERSION);
        send(client.getFD(), welcome_mess.c_str(), welcome_mess.length(), 0);
        welcome_mess = RPL_CREATED(client.getNick(), CREATEDDATE);
        send(client.getFD(), welcome_mess.c_str(), welcome_mess.length(), 0);
        welcome_mess = RPL_MYINFO(client.getNick(), SERVERNAME, VERSION, USERMODES, CHANNELMODES);
        send(client.getFD(), welcome_mess.c_str(), welcome_mess.length(), 0);
    }

    return ;
}

void Server::handlePassword(Client& client, std::istringstream& args)
{
    std::string pass, err_response;
    args >> pass;

    if (client.isRegistered())
    {
        err_response = ERR_ALREADYREGISTERED(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }
    
    if (pass.empty())
    {
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), PASS);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    if (this->_password != pass)
    {
        err_response = ERR_PASSWDMISMATCH(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    //if we don't detect any errors above, then the user input correct password. We save it
    client.checkPassword();
    
    return ;
}

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

    //NEED: for the case when we use this function being registered, we need to add logic to inform other people in channel about it

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

void Server::handleUsername(Client& client, std::istringstream& args)
{
    //I named the below vars based on the params of command according to RFC 2812
    //However, mode and unused params are not used in our project
    std::string username, mode, unused, realname, err_response;
    args >> username >> mode >> unused;
    std::getline(args, realname); //-> we are using getline instead of ">>" to copy the rest part of the line. Realname can contain spaces

    //NEED: Add logic to check password presence before check of username - shall we send error?

    if (username.empty() || realname.empty())
    {
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), USER);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    if (client.isRegistered())
    {
        err_response = ERR_ALREADYREGISTERED(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    if (realname[0] == ':')
        realname = realname.substr(1);

    client.setUsername(username);
    client.setRealname(realname);

    return ;
}