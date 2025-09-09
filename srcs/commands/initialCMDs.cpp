#include "Server.hpp"

void Server::handleInitCommands(Client& client, std::string& cmd, std::istringstream& args)
{
    std::string welcome_mess, err_response;

    if (cmd == NICK)
        handleNicknameCmd(client, args);
    else if (cmd == USER)
        handleUsernameCmd(client, args);
    else if (cmd == PASS)
        handlePasswordCmd(client, args);
    else if (cmd == CAP)
        handleCapCmd(client, args);
    else if (cmd == QUIT)
    {
        handleQuitCmd(client, args);
        return ;
    }
    else
    {
        err_response = ERR_NOTREGISTERED(client.getNick());
        client.appendSendBuffer(err_response);
        return ;
    }

    if (client.getCapNegotiation() == false && client.tryAuthenticate())
    {
		char output[50];
		time_t setTime = time(NULL);
		struct tm dateTime = *localtime(&setTime);
		strftime(output, 50, "%m/%d/%Y", &dateTime);

        welcome_mess = RPL_WELCOME(client.getNick());
        client.appendSendBuffer(welcome_mess);
        welcome_mess = RPL_YOURHOST(client.getNick(), SERVERNAME, VERSION);
        client.appendSendBuffer(welcome_mess);
        welcome_mess = RPL_CREATED(client.getNick(), output);
        client.appendSendBuffer(welcome_mess);
        welcome_mess = RPL_MYINFO(client.getNick(), SERVERNAME, VERSION, USERMODES, CHANNELMODES);
        client.appendSendBuffer(welcome_mess);
        welcome_mess = RPL_ISUPPORT(client.getNick(), ISUPPORT);
        client.appendSendBuffer(welcome_mess);
    }

    return ;
}

void Server::handleCapCmd(Client& client, std::istringstream& args)
{
    std::string arg, err_response;
    args >> arg;

    if (arg == "LS")
    {
        client.setCapNegotiation(true);
        std::string cap = "CAP * LS :\r\n";
        client.appendSendBuffer(cap);
        return ;
    }
    else if (arg == "END")
        client.setCapNegotiation(false);
    else
    {
        err_response = ERR_NOTREGISTERED(client.getNick());
        client.appendSendBuffer(err_response);
        return ;  
    }
}

void Server::handlePasswordCmd(Client& client, std::istringstream& args)
{
    std::string pass, err_response;
    args >> pass;

    if (client.isRegistered())
    {
        err_response = ERR_ALREADYREGISTERED(client.getNick());
        client.appendSendBuffer(err_response);
        return ;
    }

    if (client.isPassChecked())
        return ;

    if (pass.empty())
    {
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), PASS);
        client.appendSendBuffer(err_response);;
        return ;
    }

    if (this->_password != pass)
    {
        err_response = ERR_PASSWDMISMATCH(client.getNick());
        client.appendSendBuffer(err_response);
        return ;
    }

    client.checkPassword();
    
    return ;
}

void Server::handleUsernameCmd(Client& client, std::istringstream& args)
{
    std::string username, mode, unused, realname, err_response;
    args >> username >> mode >> unused;
    std::getline(args, realname); 
    
    if (client.isRegistered())
    { 
        err_response = ERR_ALREADYREGISTERED(client.getNick());
        client.appendSendBuffer(err_response);
        return ;
    }

    if (!client.isPassChecked())
    {
        err_response = "ERROR :Password required\r\n";
        client.appendSendBuffer(err_response);
        return ;
    }

    if (username.empty() || realname.empty()
        || username.find_first_not_of(" \t\n\v\f\r") == std::string::npos
        || realname.find_first_not_of(" \t\n\v\f\r") == std::string::npos)
    {
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), USER);
        client.appendSendBuffer(err_response);
        return ;
    }
    if (realname[0] == ':')
        realname = realname.substr(1);

    client.setUsername(username);
    client.setRealname(realname);

    return ;
}