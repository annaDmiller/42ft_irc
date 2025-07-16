#include "Server.hpp"

void Server::handlePrivateMessage(Client& client, std::istringstream& args)
{
    std::string receivers, message;
    
    args >> receivers >> message;

    if (receivers.empty())
    // = send ERR_NORECIPIENT
        return;

    if (message.empty())
    // = send ERR_NOTEXTTOSEND
        return ;
}