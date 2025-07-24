#include "../inc/Server.hpp"

// ft_irc port passwd

int main(int ac, char **av)
{
    (void)ac;
    if (ac == 3)
    {
        Server server(av);
        server.setup();
    }
}
