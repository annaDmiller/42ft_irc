#include "Server.hpp"

void Server::handleInitCommands(Client& client, std::string& raw_cmd)
{
    std::istringstream str(raw_cmd); // it allows to use a string as a stream. Stream send words divided by ' ' (space) symbol
    std::string command;
    str >> command; // it means that we put 1st word from str into command

    //as we have initial commands in uppercase, we need to transfrom our command
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "NICK")
        handleNickname(client, str);
    else if (command == "USER")
        handleUsername(client, str);
    else if (command == "PASS")
        handlePassword(client, str);
    else
        sendUnknownCMDReply(client, command);
    
    return ;
}

void Server::handlePassword(Client& client, std::istringstream& args)
{
    std::string param;
    args >> param;
    std::string wrong_pass = "Wrong password";

    if (param == this->_password)
        client.checkPassword();
    else
        send(client.getFD(), wrong_pass.c_str(), wrong_pass.length(), 0);
    
    return ;
}

void Server::handleNickname(Client& client, std::istringstream& args)
{
    std::string param;
    args >> param;

    client.setNickname(param);
    return ;
}

void Server::handleUsername(Client& client, std::istringstream& args)
{
    std::string param;
    args >> param;

    client.setUsername(param);
    return ;
}