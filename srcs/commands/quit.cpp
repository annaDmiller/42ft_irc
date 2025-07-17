#include "Server.hpp"

void Server::handleQuit(Client& client, std::istringstream& args)
{
    std::string message, rpl_message;

    std::getline(args, message);

    if (message.empty())
        message = "Client Quit";
    else
        message = message.substr(1);

    if (!client.isRegistered())
    {
        rpl_message = QUIT_MESS(client.getIPAddr(), message);
        send(client.getFD(), rpl_message.c_str(), rpl_message.size(), 0);
        rpl_message = DISCONNECTION_MESS;
        send(client.getFD(), rpl_message.c_str(), rpl_message.size(), 0);
        this->clearClient(client.getFD());
        return ;
    }

    //NEED: to handle quit of user while sending message to the members of joined channels
    //NEED: important - the message must be sent only once to the client
    //NEED: send exit messages to the client
    //NEED: to clear the client
    return ;       
}