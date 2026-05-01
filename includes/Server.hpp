#pragma once
#include "irc.hpp"

class Server {
    private:
        int                 serveur_fd;
        std::vector<int>    Tab_client;
    public:
        Server();
        ~Server();
        void    init_server(int port);
        int     getServerFd() const;
};