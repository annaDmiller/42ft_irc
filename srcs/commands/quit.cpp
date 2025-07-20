#include "Server.hpp"

void Server::handleQuit(Client& client, std::istringstream& args)
{
    std::string message, rpl_message;

    std::getline(args, message);

    //If there is no message indicated for the QUIT command, then we will use default message
    if (message.empty())
        message = "Client Quit";
    else
    {
        if (message[0] == ' ')
            message = message.substr(1);
        if (message[0] == ':')
            message = message.substr(1);
    }

    //we need to send the message to other users only if the client is registered
    if (client.isRegistered())
        client.sendToAllJoinedChannels(*this, message, QUIT, true, false);

    //otherwise, we just send a quit and disconnection messages to himself and clean it from server
    rpl_message = QUIT_MESS(client.getIPAddr(), message);
    send(client.getFD(), rpl_message.c_str(), rpl_message.size(), 0);
    rpl_message = DISCONNECTION_MESS;
    send(client.getFD(), rpl_message.c_str(), rpl_message.size(), 0);
    this->clearClient(client.getFD());
    return ;       
}