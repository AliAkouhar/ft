#include "../inc/Server.hpp"

int main(int ac, char **av)
{
    (void)ac;
    Server server(av);
    server.setup();
}
