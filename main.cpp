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

    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int client_fd = accept(Serv.getServerFd(), (struct sockaddr*)&client_addr, &len);
    if (client_fd < 0)
    {
        perror("accept");
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    close(client_fd);
    close(Serv.getServerFd());
    return 0;
}

void handle_sig(int sig)
{
    (void)sig;
    std::cout << "\nServer Down" << std::endl;
    if (g_serv)
        close(g_serv->getServerFd());
    exit(0);
}
