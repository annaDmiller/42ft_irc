#include "Server.hpp"

void Server::handleOper(Client& client, std::istringstream& args)
{
    std::string nick, password, err_message, message;

    args >> nick >> password;
    
    if (nick.empty() || password.empty())
    {
        err_message = ERR_NEEDMOREPARAMS(client.getNick(), OPER);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (nick != OPER_NAME || password != OPER_PASSWORD)
    {
        err_message = ERR_PASSWDMISMATCH(client.getNick());
        client.appendSendBuffer(err_message);
        return ;
    }

    client.setOperator(true);
    message = RPL_YOUREOPER(client.getNick());
    client.appendSendBuffer(message);
    return ;
}