#pragma once
#include "Client.hpp"
#include <vector>

class Server
{
    private:
        int                         serveur_fd;
        std::vector<struct pollfd>  fds;
        std::vector<Client>         clients;
        int                         next_id;
    public:
        Server();
        ~Server();
        void        init_server(int port);
        int         getServerFd() const;
        void        init_poll();
        void        exec(char *buffer, int fd);
        std::string getBackgroundColorCode(int socket);
        std::vector<struct pollfd> getFds() const;
};