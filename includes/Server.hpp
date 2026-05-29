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
        void        sig_handler();
        void        init_server(int port);
        int         getServerFd() const;
        void        init_poll(char *av);
        void        authentication(char *buffer, int fd, size_t i, char *av);
        void        private_message(int i, char *buffer, int fd);
        size_t      exist_nick(std::string nickname);
        std::string getBackgroundColorCode(int socket);
        std::vector<struct pollfd> getFds() const;
        void        check_register(int fd, size_t i);
};