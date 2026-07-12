/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcournoy <lcournoy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 12:35:32 by wailas            #+#    #+#             */
/*   Updated: 2026/07/12 16:18:28 by lcournoy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Channel::Channel()
{
    _limit = ULONG_MAX;
    _i = false;
    _t = false;
    _k = false;
    _l = false;   
}

Channel::Channel(std::string &name) : _name(name)
{
    _limit = ULONG_MAX;
    _i = false;
    _t = false;
    _k = false;
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
            break;
        }
    }

    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (*it == client)
        {
            _operators.erase(it);
            break;
        }
    }
}

void    Channel::removeInvited(Client *client)
{
    for (std::vector<Client*>::iterator it = _invited.begin(); it != _invited.end(); ++it)
    {
        if (*it == client)
        {
            _invited.erase(it);
            return;
        }
    }
}

void    Channel::removeOperator(Client *client)
{
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (*it == client)
        {
            _operators.erase(it);
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
    t_message msg = parse_message(buffer);

    if (msg.command != "JOIN")
        return;

    if (msg.params.empty())
    {
        std::string err = read_code(461, client.getNickname(), "JOIN", "Not enough parameters");
        sendToClient(client, err);
        return;
    }

    std::string channelName = msg.params[0];
    std::string key = "";

    if (msg.params.size() >= 2)
        key = msg.params[1];

    if (!parse_channel(channelName))
    {
        std::string err = read_code(476, client.getNickname(), channelName, "Bad Channel Mask");
        sendToClient(client, err);
        return;
    }

    Channel *chan = getChannel(channelName);

    if (!chan)
    {
        chan = new Channel(channelName);
        channel[channelName] = chan;

        chan->addMember(&client);
        chan->addOperator(&client);

        std::string prefix = client.getNickname() + "!" + client.getName() + "@localhost";
        std::string join = ":" + prefix + " JOIN " + channelName + "\r\n";
        Broadcast(chan, join);
        return;
    }

    const std::vector<Client*>& members = chan->getMembers();
    for (size_t i = 0; i < members.size(); i++)
    {
        if (members[i] == &client)
            return;
    }

    if (chan->isInviteOnly() && !chan->isInvited(&client))
    {
        std::string err = read_code(473, client.getNickname(), channelName, "Cannot join channel (+i)");
        sendToClient(client, err);
        return;
    }

    // +k
    if (chan->hasKey() && key != chan->getKey())
    {
        std::string err = read_code(475, client.getNickname(), channelName, "Cannot join channel (+k)");
        sendToClient(client, err);
        return;
    }

    // +l
    if (chan->isLimited() && members.size() >= chan->getLimit())
    {
        std::string err = read_code(471, client.getNickname(), channelName, "Cannot join channel (+l)");
        sendToClient(client, err);
        return;
    }

    chan->addMember(&client);
    chan->removeInvited(&client);

    std::string prefix = client.getNickname() + "!" + client.getName() + "@localhost";
    std::string join = ":" + prefix + " JOIN " + channelName + "\r\n";
    Broadcast(chan, join);
}

const std::vector<Client*>& Channel::getMembers() const
{
    return _members;
}

const std::string& Channel::getTopic() const
{
    return _topic;
}

void    Server::Broadcast(Channel *chan, std::string msg)
{
    std::vector<Client*> members = chan->getMembers();
    
    for (size_t i = 0; i != members.size(); i++)
    {
        sendToClient(*members[i], msg);
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

bool Channel::isInviteOnly() const
{
    return _i;
}

bool Channel::isTopicProtected() const
{
    return _t;
}

bool Channel::hasKey() const
{
    return _k;
}

bool Channel::isLimited() const
{
    return _l;
}

const std::string &Channel::getKey() const
{
    return _key;
}

unsigned long Channel::getLimit() const
{
    return _limit;
}

const std::string& Channel::getChannelName() const
{
    return _name;
}

bool Channel::isInvited(Client *client) const
{
    for (size_t i = 0; i < _invited.size(); i++)
    {
        if (_invited[i] == client)
            return true;
    }
    return false;
}

bool Channel::isMember(Client *client) const
{
    for (size_t i = 0; i < _members.size(); i++)
    {
        if (_members[i] == client)
            return true;
    }
    return false;
}

bool Channel::isOperator(Client *client) const
{
    for (size_t i = 0; i < _operators.size(); i++)
    {
        if (_operators[i] == client)
            return true;
    }
    return false;
}

void Channel::setLimit(unsigned long limit)
{
    _l = true;
    _limit = limit;
}

void Channel::set_i(bool type)
{
    _i = type;
}

void Channel::set_t(bool type)
{
    _t = type;
}

void Channel::set_k(bool type)
{
    _k = type;
}

void Channel::set_l(bool type)
{
    _l = type;
}

void Channel::setKey(std::string key)
{
    _key = key;
}

void Channel::rebuildMembers(const std::vector<Client*> &newMembers)
{
    _members = newMembers;
}

void Channel::rebuildOperators(const std::vector<Client*> &newOps)
{
    _operators = newOps;
}

void Channel::rebuildInvited(const std::vector<Client*> &newInv)
{
    _invited = newInv;
}

const std::vector<Client*>& Channel::getOperators() const 
{ 
    return _operators;
}

const std::vector<Client*>& Channel::getInvited() const
{ 
    return _invited;
}