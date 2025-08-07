#include "../inc/Server.hpp"

int main(int ac, char **av)
{
    try
    {
        if (ac != 3)
            throw std::runtime_error("Usage: ./irc_serv <port> <password>");
        Server server(av);
        server.setup();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}