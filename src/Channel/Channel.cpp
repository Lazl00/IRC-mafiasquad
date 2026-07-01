/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ainthana <ainthana@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 12:35:32 by wailas            #+#    #+#             */
/*   Updated: 2026/07/01 15:42:01 by ainthana         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Channel::Channel()
{
    _i = false;
    _t = false;
    _k = false;
    _o = false;
    _l = false;   
}

Channel::Channel(std::string &name)
{
    _name = name;
    _i = false;
    _t = false;
    _k = false;
    _o = false;
    _l = false;
    
}

Channel::~Channel() {};

void Channel::addMember(Client *name)
{
    _members.push_back(name);
}

void    Channel::setName(std::string name)
{
    this->_name = name;
}

void    Channel::setTopic(std::string topic)
{
    this->_topic = topic;
}

void    Channel::seeTopic()
{
    std::cout << this->_topic << std::endl;
}

void    Channel::kick(Client *client)
{
    for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
    {
        if (*it == client)
        {
            _members.erase(it);
            return;
        }
    }
}


void Channel::invite(Client* client)
{
    for (std::vector<Client*>::iterator it = _invited.begin(); it != _invited.end(); ++it)
    {
        if (*it == client)
            return;
    }
    _invited.push_back(client);
}

void Server::Create_channel(const char *buffer, Client &client)
{
    std::istringstream iss(buffer);
    std::string cmd, channelName;

    if (!(iss >> cmd) || !(iss >> channelName))
    {
        if (cmd == "JOIN")
        {
            std::string err = read_code(461, client.getNickname(), "JOIN", "Not enough parameters");
            send(client.getFd(), err.c_str(), err.length(), 0);
        }
        return;
    }
    
    if (cmd != "JOIN")
        return;

    if (!parse_channel(channelName))
    {
        std::string err = read_code(403, client.getNickname(), channelName, "No such channel");
        send(client.getFd(), err.c_str(), err.length(), 0);
        return;
    }

    Channel *chan = getChannel(channelName);
    if (!chan)
    {
        std::cout << "Channel created: " << channelName << std::endl;
        chan = new Channel(channelName);
        channel[channelName] = chan;
        chan->addOperator(&client);
    }

    std::vector<Client*> members = chan->getMembers();
    for (size_t i = 0; i < members.size(); i++)
    {
        if (members[i] == &client)
            return;
    }

    chan->addMember(&client);
    std::string msg = ":" + client.getNickname() + " JOIN " + channelName + "\r\n";
    Broadcast(chan, msg);
}

const std::vector<Client*>& Channel::getMembers() const
{
    return _members;
}

void    Server::Broadcast(Channel *chan, std::string msg)
{
    std::vector<Client*> members = chan->getMembers();
    
    for (size_t i = 0; i != members.size(); i++)
    {
        send(members[i]->getFd(), msg.c_str(), msg.size(), 0);
    }
}

void Channel::addOperator(Client* client)
{
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (*it == client)
            return;
    }
    _operators.push_back(client);
}