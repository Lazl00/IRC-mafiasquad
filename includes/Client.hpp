#pragma once
#include <poll.h>
#include <string>
#include <iostream>

class Client
{
    private:
        struct pollfd   cfp;
        std::string     nickname;
        std::string     buffer;
        int             id;
        int             fd;
        bool            has_password;
    public:
        Client();
        Client(int fd, int id);
        ~Client();

        struct pollfd getCfp();
        struct pollfd getCfp() const;
        std::string getNickname() const;
        std::string getBuffer() const;
        int getId() const;
        int getFd() const;
        bool getHasPassword() const;

        void setCfp(struct pollfd newCfp);
        void setNickname(const std::string& newNickname);
        void setBuffer(const std::string& newBuffer);
        void setId(int newId);
        void setFd(int newFd);
        void setHasPassword(bool status);
};