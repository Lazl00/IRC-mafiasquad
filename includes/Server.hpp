#pragma once
#include "irc.hpp"
#include <vector>

class Server {
    private:
        int                         serveur_fd;
        std::vector<struct pollfd>  fds;
    public:
        Server();
        ~Server();
        void        init_server(int port);
        int         getServerFd() const;
        void        init_poll();
        std::vector<struct pollfd> getFds() const;
};