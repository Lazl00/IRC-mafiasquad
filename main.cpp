#include "includes/irc.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = atoi(av[1]);

    // 1. socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // 2. config address
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // 3. bind
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    // 4. listen
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    // 5. accept (test)
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
    if (client_fd < 0)
    {
        perror("accept");
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    close(client_fd);
    close(server_fd);

    return 0;
}