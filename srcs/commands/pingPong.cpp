#include "Server.hpp"

void Server::handlePingCmd(Client& client, std::istringstream& args)
{
    std::string token, err_message, pong_response;
    
    args >> token;

    if (token.empty())
    {
        err_message = ERR_NOORIGIN(client.getNick(), PING);
        client.appendSendBuffer(err_message);
        return ;
    }

    if (token[0] == ':')
        token = token.substr(1);

    std::string pong_reply = std::string(":") + HOST + " " + PONG + " " + client.getNick() + " " + std::string(":") + token + TERMIN;
    client.appendSendBuffer(pong_reply);
    return ;
}
