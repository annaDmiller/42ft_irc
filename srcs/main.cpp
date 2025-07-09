#include "Client.hpp"
#include "Server.hpp"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "You must run the server with port and password as arguments" << std::endl;
        return (1);
    }

    Server serv;
    try
    {
        serv.initServer(argv[1]);
        serv.runServer(argv[2]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
}