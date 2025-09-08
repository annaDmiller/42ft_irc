#include "Server.hpp"

void Server::handleQuitCmd(Client& client, std::istringstream& args)
{
    std::string message, rpl_message;

    if (args.peek() == ' ')
        args.get();
    if (args.peek() == ':')
        std::getline(args, message);
    else
        args >> message;

    if (message.empty())
        message = "Client Quit";
    else
    {
        if (message[0] == ' ')
            message = message.substr(1);
        if (message[0] == ':')
            message = message.substr(1);
    }

    if (client.isRegistered())
        client.sendToAllJoinedChannels(*this, message, QUIT, true, false);

    rpl_message = QUIT_MESS(client.getIPAddr(), message);
    if (rpl_message.length() > MAXLINELENGTH)
        rpl_message = rpl_message.substr(0, MAXLINELENGTH - 3) + ")" + TERMIN;
    client.splitSendBuffer(0, client.getSendBuffer().length());
    client.appendSendBuffer(rpl_message);
    if (client.hasMessToSend())
        this->sendReply(client.getFD());
    this->clearClient(client.getFD());
    
    return ;
}