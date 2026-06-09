#pragma once
#include <poll.h>
#include <string>
#include <iostream>

class Client
{
    private:
        struct pollfd   cfp;
        std::string     nickname;
        std::string     username;
        std::string     realname;
        std::string     buffer;
        int             id;
        int             fd;
        std::string     color;
        bool            has_password;
        bool            has_nickname;
        bool            has_user;
        bool            has_register;
        bool            hasWelcome;
    public:
        Client();
        Client(int fd, int id, std::string color);
        ~Client();

        struct pollfd getCfp();
        struct pollfd getCfp() const;
        std::string getNickname() const;
        std::string getBuffer() const;
        std::string getName() const;
        int getId() const;
        int getFd() const;
        bool getHasPassword() const;
        bool getHasNickname() const;
        bool getHasUser() const;
        bool getHasWelcome() const;
        bool getHasRegister() const;
        std::string getColor() const;

        void setCfp(struct pollfd newCfp);
        void setBuffer(const std::string& newBuffer);
        void setId(int newId);
        void setFd(int newFd);
        void setRealname(std::string str);
        void setUsername(std::string str);
        void setHasPassword(bool status);
        void setHasNickname(bool status);
        void setHasUsername(bool status);
        void setHasWelcome(bool status);
        void setNickname(const std::string& newNickname);
        void setRegister(bool status);
};