#include "Server.hpp"

void Server::handleInitCommands(Client& client, std::string& cmd, std::istringstream& args)
{
    std::string welcome_mess, err_response;

    std::cout << "handleInitCommands" << std::endl;//test
    std::cout << "cmd: " << cmd << std::endl;//test

    if (cmd == NICK)
        handleNickname(client, args);
    else if (cmd == USER)
        handleUsername(client, args);
    else if (cmd == PASS)
        handlePassword(client, args);
    else if (cmd == CAP)
        handleCap(client, args);
    else if (cmd == QUIT)
    {
        handleQuit(client, args);
        return ;
    }
    else
    {
        std::cout << "ERR_NOTREGISTERED " << std::endl;//test
        err_response = ERR_NOTREGISTERED(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }
    
    if (client.getCapNegotiation() == false && client.tryAuthenticate())
    {
        std::cout << "tryAuthenticate " << std::endl;//test
        welcome_mess = RPL_WELCOME(client.getNick());
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

void Server::handleCap(Client& client, std::istringstream& args)
{
    std::string arg, err_response;
    args >> arg;
    std::cout << "arg: " << arg << "!" << std::endl;//test

    if (arg == "LS")
    {
        client.setCapNegotiation(true);
        std::string cap = "CAP * LS :\r\n";
        send(client.getFD(), cap.c_str(), cap.length(), 0);//test
        return ;
    }
    else if (arg == "END")
    {
        client.setCapNegotiation(false);
        std::cout << "END " << arg << std::endl;//test
    }
    else
    {
        err_response = ERR_NOTREGISTERED(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;  
    }
}

void Server::handlePassword(Client& client, std::istringstream& args)
{
    std::string pass, err_response;
    args >> pass;

    //Registered user can't use PASS again
    if (client.isRegistered())
    {
        err_response = ERR_ALREADYREGISTERED(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    //if user tries to use PASS command multiple times, for ft_irc I decided just to skip this command
    if (client.isPassChecked())
        return ;

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

void Server::handleUsername(Client& client, std::istringstream& args)
{
    std::cout << "handleUsername" << std::endl;//test
    //I named the below vars based on the params of command according to RFC 2812
    //However, mode and unused params are not used in our project
    std::string username, mode, unused, realname, err_response;
    args >> username >> mode >> unused;
    std::getline(args, realname); //-> we are using getline instead of ">>" to copy the rest part of the line. Realname can contain spaces

    if (client.isRegistered())
    { 
        std::cout << "handleUsername 1" << std::endl;//test
        err_response = ERR_ALREADYREGISTERED(client.getNick());
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }

    if (!client.isPassChecked())
    {
         std::cout << "handleUsername 2" << std::endl;//test
        err_response = "ERROR :Password required\r\n";
        send(client.getFD(), err_response.c_str(), err_response.size(), 0);
        return ;
    }

    if (username.empty() || realname.empty())
    {
         std::cout << "handleUsername 3" << std::endl;//test
        err_response = ERR_NEEDMOREPARAMS(client.getNick(), USER);
        send(client.getFD(), err_response.c_str(), err_response.length(), 0);
        return ;
    }
    std::cout << "handleUsername 4" << std::endl;//test
    if (realname[0] == ':')
        realname = realname.substr(1);
     std::cout << "username " << username << std::endl;//test
      std::cout << "realname " << realname << std::endl;//test

    client.setUsername(username);
    client.setRealname(realname);

    return ;
}