#include "includes/irc.hpp"

Server *g_serv = NULL;

int main(int ac, char **av)
{
    Server  Serv;
    int     port;
    g_serv = &Serv;

    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    signal(SIGINT, handle_sig);
    port = atoi(av[1]);
    Serv.init_server(port);

    std::cout << "Server listening on port " << port << std::endl;

    Serv.init_poll(av[2]);
    return (0);
}

void handle_sig(int sig)
{
    (void)sig;
    std::cout << "\nServer Down" << std::endl;
    if (g_serv)
        close(g_serv->getServerFd());
    exit(0);
}
