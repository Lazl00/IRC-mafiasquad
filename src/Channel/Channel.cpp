/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wailas <wailas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 12:35:32 by wailas            #+#    #+#             */
/*   Updated: 2026/06/09 17:57:27 by wailas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Channel::Channel() {};

Channel::Channel(std::string &name)
{
    _name = name;
}

Channel::~Channel() {};

void Channel::addMember(Client name)
{
    _members.push_back(&name);
}

void    Channel::setName(std::string name)
{
    this->_name = name;
}

void Server::Create_channel(char *buffer, Client &client)
{
    std::istringstream  iss(buffer);
    std::string         cmd, channelName;
    
    iss >> cmd;
    iss >> channelName;
    
    if (cmd != "JOIN")
        return;

    Channel *chan = getChannel(channelName);
    
    if (!chan)
    {
        std::cout << "Channel created" << std::endl;
        chan = new Channel(channelName);
        channel[channelName] = chan;
        chan->addOperator(&client);
    }
    chan->addMember(client);
}

std::vector<Client*> Channel::getMembers()
{
    return (_members);
}

void    Server::Broadcast(Channel *chan, std::string msg)
{
    std::vector<Client*> members = chan->getMembers();
    for (size_t i = 0; i != members.size(); i++)
    {
        send(clients[i].getFd(), msg.c_str(), sizeof(msg), 0);
    }
}

void    Channel::addOperator(Client* name)
{
    _operator.push_back(name);
}
