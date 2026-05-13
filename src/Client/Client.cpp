/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wailas <wailas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 15:28:39 by wailas            #+#    #+#             */
/*   Updated: 2026/05/13 16:03:54 by wailas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"

Client::Client() {};
Client::~Client() {};
        
Client::Client(int fd, int id, std::string color) : fd(fd), id(id), color(color)
{
    this->id = id;
    this->buffer = "";
    this->nickname = "";
    this->cfp.fd = fd;
    this->cfp.events = POLLIN;
    this->cfp.revents = 0;
    this->has_password = 0;
    this->color = color;
    std::cout << color << "Client [" << this->id<< "] connecte"<< "\033[0m" << std::endl;
}

struct pollfd Client::getCfp()
{
    return (this->cfp);
}

struct pollfd Client::getCfp() const
{
    return cfp;
}

std::string Client::getNickname() const
{
    return nickname;
}

std::string Client::getBuffer() const
{
    return buffer;
}

int Client::getId() const
{
    return id;
}

int Client::getFd() const
{
    return fd;
}

bool Client::getHasPassword() const
{
    return has_password;
}

std::string Client::getColor() const
{
    return color;
}


void Client::setCfp(struct pollfd newCfp)
{
    cfp = newCfp;
}

void Client::setNickname(const std::string& newNickname)
{
    nickname = newNickname;
}

void Client::setBuffer(const std::string& newBuffer)
{
    buffer = newBuffer;
}

void Client::setId(int newId)
{
    id = newId;
}

void Client::setFd(int newFd)
{
    fd = newFd;
}

void Client::setHasPassword(bool status)
{
    has_password = status;
}
