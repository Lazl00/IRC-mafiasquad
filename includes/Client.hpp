#pragma once
#include <poll.h>
#include <string>

class Client
{
    private:
        struct pollfd   cfp;
        std::string     nickename;
        std::string     buffer;
        int             id;
        int             fd;
    public:
        Client();
        Client(int fd, int id);
        ~Client();

        struct pollfd getCfp();
};