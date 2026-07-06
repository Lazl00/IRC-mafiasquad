/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ainthana <ainthana@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 12:00:01 by wailas            #+#    #+#             */
/*   Updated: 2026/07/06 17:40:28 by ainthana         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Server::Server() {};

Server::~Server()
{
    for (size_t i = 0; i < fds.size(); i++)
        close(fds[i].fd);
    fds.clear();
    
    clients.clear();
    
    std::map<std::string, Channel*>::iterator it;
    for (it = channel.begin(); it != channel.end(); ++it)
    {
        delete it->second;
    }
    channel.clear();
}

void    Server::init_server(int port)
{
    int opt;
	clients.reserve(1024);
    this->next_id = 1;

    opt = 1;
    serveur_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (serveur_fd < 0)
    {
        perror("Error : socket server");
        exit(0);
    }
    
    if (setsockopt(serveur_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("error on setsockopt");
        exit(1);
    }
    
    fcntl(serveur_fd, F_SETFL, O_NONBLOCK);
    
    struct sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(port);
    addrServer.sin_addr.s_addr = INADDR_ANY;
    std::memset(&addrServer.sin_zero, 0, sizeof(addrServer.sin_zero));
    
    if (bind(serveur_fd, (const sockaddr*)&addrServer, sizeof(addrServer)) == -1)
    {
        perror("Error on the binding Server");
        exit(1);
    };
    
    if (listen(serveur_fd, 10) < 0)
    {
        perror("Error on the listening of the Server");
        exit(1);
    }
};

void    Server::sig_handler()
{
    fds.clear();
}

void    Server::init_poll(char *av)
{
    struct pollfd   serveur_fd_poll;
    serveur_fd_poll.fd = serveur_fd;
    serveur_fd_poll.events = POLLIN;
    serveur_fd_poll.revents = 0;
    fds.push_back(serveur_fd_poll);
    
    while (true)
    {
        poll(fds.data(), fds.size(), -1);
        
        if (fds[0].revents & POLLIN)
        {
            int         fd;
            std::string clientColor;
            
            fd = accept(this->serveur_fd, NULL, NULL);
            fcntl(fd, F_SETFL, O_NONBLOCK);
            clientColor = getBackgroundColorCode(this->next_id);
            Client client(fd, this->next_id++, clientColor);
            this->clients.push_back(client); 
            this->fds.push_back(client.getCfp());
        }
        
        for (size_t i = 1; i < fds.size(); i++)
        {
            char buffer[1024];
            if (fds[i].revents & POLLIN)
            {
                int result;
                
                memset(buffer, 0, sizeof(buffer));
                result = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                
                if (result < 0)
                    continue;
                
				else if (result == 0) {
					std::cout << clients[i - 1].getColor() << "Client [" << clients[i - 1].getId() << "] disconnected\033[0m" << std::endl;
					cleanup(i - 1, "");
					i--;
				}
                
				else {
					if (parse_token(buffer, result, i - 1, av))
						i--;
				}
            }
        }
    }
}

std::vector<struct pollfd> Server::getFds() const
{
    return (this->fds);
}

int     Server::getServerFd() const
{
    return (this->serveur_fd);
}

void Server::check_register(int fd, size_t i)
{
    if (clients[i].getHasPassword()
        && clients[i].getHasNickname()
        && clients[i].getHasUser()
        && !clients[i].getHasWelcome())
    {
        clients[i].setRegister(1);
        clients[i].setHasWelcome(true);
        std::string msg =
            ":ircserv 001 " + clients[i].getNickname() +
            " :Welcome to the IRC server\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
}

void    Server::authentication(const char *buffer, int fd, size_t i, char *argv)
{

    if (!clients[i].getHasRegister())
    {
        t_message msg = parse_message(buffer);
        std::string target = clients[i].getNickname();
        if (target.empty())
            target = "*";

        if (msg.command == "CAP")
        {
            if (!msg.params.empty() && msg.params[0] == "LS")
            {
                std::string r = "CAP * LS :\r\n";
                send(fd, r.c_str(), r.size(), 0);
            }
            else if (!msg.params.empty() && msg.params[0] == "REQ")
            {
                // CAP REQ, pas besoin de gerer
                std::string r = "CAP * NAK :\r\n";
                send(fd, r.c_str(), r.size(), 0);
            }
            // CAP END, rien a faire
            return ;
        }

        if ((msg.command == "PASS" || msg.command == "pass") && clients[i].getHasPassword() == 0)
        {
            if (msg.params.empty())
            {
                std::string result = read_code(461, target, "PASS", "Not enough parameters");
                send(fd, result.c_str(), result.size(), 0);
                return ;
            }
            
            if (msg.params[0] == argv)
            {
                clients[i].setHasPassword(1);
            }
            
            else
            {
                std::string result = read_code(464, target, "", "Password incorrect");
                send(fd, result.c_str(), result.size(), 0);
            }
            return ;
        }

        if (!clients[i].getHasPassword())
        {
            std::string result = read_code(464, target, "", "Password required first");
            send(fd, result.c_str(), result.size(), 0);
            return ;
        }

        if (msg.command == "NICK" || msg.command == "nick")
        {
            if (msg.params.empty())
            {    
                std::string result = read_code(431, target, "", "No nickname given");
                send(fd, result.c_str(), result.size(), 0);
                return ;
            }

            if (!is_valid_nick(msg.params[0]))
            {
                std::string result = read_code(432, target, msg.params[0], "Erroneous nickname");
                send(fd, result.c_str(), result.size(), 0);
                return ;
            }
            
            if (exist_nick(msg.params[0]))
            {
                std::string result = read_code(433, target, msg.params[0], "Nickname is already in use");
                send(fd, result.c_str(), result.size(), 0);
                return ;
            }
            
            clients[i].setNickname(msg.params[0]);
            clients[i].setHasNickname(true);
            std::cout << clients[i].getColor() << "Client [" << clients[i].getId()
                      << "] changed his nickname to " << clients[i].getNickname() << "\033[0m" << std::endl;
            return ;
        }

        if (msg.command == "USER" || msg.command == "user")
        {
            if (msg.params.size() < 4)
            {
                std::string result = read_code(461, target, "USER", "Not enough parameters");
                send(fd, result.c_str(), result.size(), 0);
                return ;
            }
            clients[i].setUsername(msg.params[0]);
            clients[i].setRealname(msg.params[3]);
            clients[i].setHasUsername(true);
            return ;
        }
    }
}

bool Server::exist_nick(std::string nickname)
{
    for (size_t i = 0; i < clients.size(); i++)
        if (clients[i].getNickname() == nickname)
            return (true);
    return (false);
}

void Server::private_message(int i, const char* buffer)
{
    t_message   msg = parse_message(buffer);

    if (msg.command != "PRIVMSG")
		return;
	
	if (msg.params.empty())
	{
		std::string err = read_code(411, clients[i].getNickname(), "", "No recipient given (PRIVMSG)");
		send(clients[i].getFd(), err.c_str(), err.length(), 0);
		return;
	}
	if (msg.params.size() < 2)
	{
		std::string err = read_code(412, clients[i].getNickname(), "", "No text to send");
		send(clients[i].getFd(), err.c_str(), err.length(), 0);
		return;
	}
	
	std::string	target = msg.params[0];
	std::string	text = msg.params[msg.params.size() - 1];

	if (text.empty())
	{
		std::string err = read_code(412, clients[i].getNickname(), "", "No text to send");
		send(clients[i].getFd(), err.c_str(), err.length(), 0);
		return;
	}

	if (clients[i].getHasRegister())
	{
		std::string prefix = clients[i].getNickname() + "!" 
                       + clients[i].getName() + "@localhost";
		std::string	final_msg = ":" + prefix + " PRIVMSG " + target + " :" + text + "\r\n";
		
		size_t	target_index = find_client_by_nick(target);

		if (target_index != clients.size())
			send(clients[target_index].getFd(), final_msg.c_str(), final_msg.size(), 0);
		
		else if (getChannel(target) != NULL)
        {
            Channel *chan = getChannel(target);
            std::vector<Client*> members = chan->getMembers();
            for (size_t j = 0; j < members.size(); j++)
            {
                if (members[j]->getFd() != clients[i].getFd())
                    send(members[j]->getFd(), final_msg.c_str(), final_msg.size(), 0);
            }
        }
        else
        {
            std::string err = read_code(401, clients[i].getNickname(), target, "No such nick/channel");
            send(clients[i].getFd(), err.c_str(), err.length(), 0);
        }
    }
}

std::string Server::getBackgroundColorCode(int socket)
{
	int colorCode;
    
    colorCode = 41 + (socket % 7);
	std::stringstream color;

	color << "\033[1;97;" << colorCode << "m";
	return (color.str());
}

Channel* Server::getChannel(const std::string &name)
{
    std::map<std::string, Channel *>::iterator it;
    
    it = channel.find(name);
    
    if (it != channel.end())
    {
        return (it->second);
    }
    return (NULL);
}

size_t  Server::find_client_by_nick(std::string nickname) {
    
    for (size_t i = 0; i < clients.size(); i++)
        if (clients[i].getNickname() == nickname)
            return (i);
    return (clients.size());
}

void	Server::cleanup(size_t index, std::string msg) {
	
	std::string prefix = clients[index].getNickname() + "!" 
                       + clients[index].getName() + "@localhost";
	if (msg.empty())
		msg = "Leaving";

	std::string final_msg = ":" + prefix + " QUIT :" + msg + "\r\n";

	std::map<std::string, Channel*>::iterator it;
	for (it = channel.begin(); it != channel.end();)
	{
		std::vector<Client*> members = it->second->getMembers();
		bool found = false;
		
		for (size_t j = 0; j < members.size(); j++)
		{
			if (members[j] == &clients[index])
			{
				Broadcast(it->second, final_msg);
				it->second->kick(&clients[index]);
				found = true;
				break;
			}
		}
		
		if (found && it->second->getMembers().empty())
		{
			delete it->second;
			channel.erase(it++);
		}
		else
		{
			++it;
		}
	}

	close(fds[index + 1].fd);
	fds.erase(fds.begin() + index + 1);
	clients.erase(clients.begin() + index);
}

bool Server::channelExists(const std::string &name) const
{
    return channel.find(name) != channel.end();
}

bool Server::handleOpCmds(Client *sender, const t_message &msg)
{
    if (msg.params.empty())
        return false;

    const std::string &chanName = msg.params[0];

    if (!parse_channel(chanName))
        return false;

    Channel *chan = getChannel(chanName);
    if (!chan)
        return false;
    
    if (!chan->isMember(sender))
    {
        std::string err = read_code(442, sender->getNickname(), chanName, "You're not on that channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (msg.command == "KICK")
        return handleKick(sender, chan, msg);
    else if (msg.command == "INVITE")
        return handleInvite(sender, chan, msg);
    else if (msg.command == "TOPIC")
        return handleTopic(sender, chan, msg);
    // else if (msg.command == "MODE")
    //     return handleMode(sender, chan, msg);

    return false;
}

Client* Server::getClientByNick(const std::string &nick)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].getNickname() == nick)
            return &clients[i];
    }
    return NULL;
}

bool Server::handleTopic(Client *sender, Channel *chan, const t_message &msg)
{
    if (msg.params.size() == 1)
    {
        chan->seeTopic();
        return true;
    }
    
    else if(chan->isOperator(sender) || !chan->isTopicProtected())
    {
        chan->setTopic(msg.params[1]);

        std::cout << "Nouveau topic : " << msg.params[1] << std::endl;
        return true;
    }
    
    std::string err = read_code(482, sender->getNickname(), chan->getChannelName(), "You're not channel operator");
    send(sender->getFd(), err.c_str(), err.length(), 0);
    return false;
}

bool Server::handleKick(Client *sender, Channel *chan, const t_message &msg)
{
    
    if (msg.params.size() < 2)
    {
        std::string err = read_code(461, sender->getNickname(), "KICK", "Not enough parameters");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }
    
    Client *target = getClientByNick(msg.params[1]);

    // if (target == sender)
    // {
    //     std::cout << "You can't kick yourself." << std::endl;
    //     return false;
    // }
    
    if (!chan->isOperator(sender))
    {
        std::string err = read_code(482, sender->getNickname(), chan->getChannelName(), "You're not channel operator");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (target == NULL)
    {
        std::string err = read_code(401, sender->getNickname(), msg.params[1], "No such nick/channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (!chan->isMember(target))
    {
        std::string err = read_code(441, sender->getNickname(), msg.params[1], "They aren't on that channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    //faut je revois pour le Broadcast pas sur c'est le bon format
    std::string prefix = sender->getNickname() + "!" 
                   + sender->getName() + "@localhost";
    std::string kick_msg = ":" + prefix + " KICK " + target->getNickname() + " :" + "\r\n";
    Broadcast(chan, kick_msg);
    
    chan->kick(target);
    return true;
}

bool Server::handleInvite(Client *sender, Channel *chan, const t_message &msg)
{
    if (msg.params.size() < 2)
    {
        std::string err = read_code(461, sender->getNickname(), "INVITE", "Not enough parameters");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (!chan->isOperator(sender))
    {
        std::string err = read_code(482, sender->getNickname(), chan->getChannelName(), "You're not channel operator");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    Client *target = getClientByNick(msg.params[1]);

    if (target == NULL)
    {
        std::string err = read_code(401, sender->getNickname(), msg.params[1], "No such nick");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (target == sender)
        return false;

    if (chan->isMember(target))
    {
        std::string err = read_code(443, sender->getNickname(), msg.params[1], "is already on channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    chan->invite(target);

    // confirmation au sender (341 RPL_INVITING)
    std::string confirm = read_code(341, sender->getNickname(), msg.params[1], chan->getChannelName());
    send(sender->getFd(), confirm.c_str(), confirm.length(), 0);

    // notification à la cible
    std::string notif = ":" + sender->getNickname() + " INVITE " + target->getNickname() + " " + chan->getChannelName() + "\r\n";
    send(target->getFd(), notif.c_str(), notif.length(), 0);

    return true;
}
