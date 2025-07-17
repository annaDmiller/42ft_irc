#include "Server.hpp"

void Server::handlePing(Client& client, std::istringstream& args)
{
    std::string token, err_message, pong_response;
    
    args >> token;

    if (token.empty())
    {
        err_message = ERR_NOORIGIN(client.getNick(), PING);
        send(client.getFD(), err_message.c_str(), err_message.size(), 0);
        return ;
    }

    if (token[0] == ':')
        token = token.substr(1);

    std::string pong_reply = std::string(":") + HOST + " " + PONG + " " + client.getNick() + " " + std::string(":") + token + TERMIN;
    send(client.getFD(), pong_reply.c_str(), pong_reply.size(), 0);
    return ;
}
