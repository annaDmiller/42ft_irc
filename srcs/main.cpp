#include "Client.hpp"
#include "Server.hpp"

bool isValidArgs(std::string port_num, std::string password)
{
    long port;
    char *pscalar_end;

    if (port_num.empty() || password.empty())
        return (false);
    
    port = std::strtol(port_num.c_str(), &pscalar_end, 10);
    if (*pscalar_end != '\0' || port == LONG_MIN || port == LONG_MAX || port < 1024 || port > 65535)
        return (false);
    if (port_num.find_first_not_of("0123456789") != std::string::npos)
        return (false);
    
    return (true);
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "You must run the server with port and password as arguments" << std::endl;
        return (1);
    }

    if (!isValidArgs(argv[1], argv[2]))
    {
        std::cerr << "Incorrect port number or password" << std::endl;
        return (1);
    }

    Server serv;
    try
    {
        std::signal(SIGTERM, Server::signalHandler);
        std::signal(SIGINT, Server::signalHandler);
        serv.initServer(argv[1], argv[2]);
        std::cout << "[DEBUG] Server is launched" << std::endl;
        serv.runServer();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
}