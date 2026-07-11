/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ainthana <ainthana@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 15:28:39 by wailas            #+#    #+#             */
/*   Updated: 2026/07/11 17:04:05 by ainthana         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"

Client::Client() {};

Client::~Client() {};
        
Client::Client(int fd, int id, std::string color) : id(id), fd(fd), color(color)
{
    this->id = id;
    this->buffer = "";
    this->nickname = "";
    this->username = "";
    this->realname = "";
    this->cfp.fd = fd;
    this->cfp.events = POLLIN;
    this->cfp.revents = 0;
    this->has_password = 0;
    this->has_register = 0;
    this->has_nickname = 0;
	this->hasWelcome = 0;
    this->has_user = 0;
    this->color = color;
    std::cout << color << "Client [" << this->id<< "] connecte"<< "\033[0m" << std::endl;
}

struct pollfd Client::getCfp()
{
    return (this->cfp);
}

struct pollfd Client::getCfp() const
{
    return (cfp);
}

std::string Client::getNickname() const
{
    return (nickname);
}

std::string Client::getBuffer() const
{
    return (buffer);
}

int Client::getId() const
{
    return (id);
}

int Client::getFd() const
{
    return (fd);
}

bool Client::getHasPassword() const
{
    return (has_password);
}

std::string Client::getColor() const
{
    return (color);
}

bool Client::getHasNickname() const
{
    return (this->has_nickname);
}

bool Client::getHasUser() const
{
    return (this->has_user);
}

bool Client::getHasRegister() const
{
    return (this->has_register);
}

std::string Client::getName() const
{
    return (this->username);
}

bool Client::getHasWelcome() const
{
    return (this->hasWelcome);
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

void Client::setRegister(bool status)
{
    this->has_register = status;
}

void Client::setRealname(std::string str)
{
    this->realname = str;
}

void Client::setUsername(std::string str)
{
    this->username = str;
}

void Client::setHasNickname(bool status)
{
    this->has_nickname = status;
}

void Client::setHasUsername(bool status)
{
    this->has_user = status;
}

void Client::setHasWelcome(bool status)
{
    this->hasWelcome = status;
}

void Client::appendSendBuffer(const std::string &data)
{
    _sendBuffer += data;
}

const std::string& Client::getSendBuffer() const
{
    return (this->_sendBuffer);
}

void Client::clearSendBuffer(size_t bytes)
{
    _sendBuffer.erase(0, bytes);
}

bool Client::hasPendingData() const
{
    if (_sendBuffer.empty())
        return false;
    return true;
}