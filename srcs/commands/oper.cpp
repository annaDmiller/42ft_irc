#include "Server.hpp"

void Server::handleOper(Client& client, std::istringstream& args)
{
    std::string nick, password, err_message, message;

    args >> nick >> password;
    
    if (nick.empty() || password.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), OPER);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (nick != OPER_NAME || password != OPER_PASSWORD)
    {
        err_message = ERR_PASSWDMISMATCH(client.getNick());
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    client.setOperator(true);
    message = RPL_YOUREOPER(client.getNick());
    send(client.getFD(), message.c_str(), message.size(), 0);
    return ;
}