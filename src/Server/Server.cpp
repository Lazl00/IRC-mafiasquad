/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ainthana <ainthana@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 12:00:01 by wailas            #+#    #+#             */
/*   Updated: 2026/07/10 23:56:52 by ainthana         ###   ########.fr       */
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

// void    Server::sig_handler()
// {
// 	for (size_t i = 0; i < fds.size(); i++) {
// 			close(fds[i].fd);
// 	}
// 	//fds.clear();
// 	//clients.clear();
// }

void    Server::init_poll(char *av)
{
    struct pollfd   serveur_fd_poll;
    serveur_fd_poll.fd = serveur_fd;
    serveur_fd_poll.events = POLLIN;
    serveur_fd_poll.revents = 0;
    fds.push_back(serveur_fd_poll);
    
    while (g_running)
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
					//i--;
                    break; //pour que irssi voit broadcast QUIT : POLL recalcule l'index
				}
                
				else {
					if (parse_token(buffer, result, i - 1, av))
                        //i--; //same
						break;
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

void Server::check_register(int fd, int i)
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
		
		else if (getChannel(target) != NULL && getChannel(target)->isMember(&clients[i]))
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

void Server::cleanup(size_t index, std::string msg)
{
    std::string prefix = clients[index].getNickname() + "!"
                       + clients[index].getName() + "@localhost";
    if (msg.empty())
        msg = "Leaving";

    std::string final_msg = ":" + prefix + " QUIT :" + msg + "\r\n";

    // ÉTAPE 1 : sauvegarder les fd de tous les channels AVANT tout erase
    // (les pointeurs sont encore valides ici)
    std::map<std::string, std::vector<int> > memberFds;
    std::map<std::string, std::vector<int> > operatorFds;
    std::map<std::string, std::vector<int> > invitedFds;

    for (std::map<std::string, Channel*>::iterator it = channel.begin();
         it != channel.end(); ++it)
    {
        const std::vector<Client*>& m = it->second->getMembers();
        for (size_t j = 0; j < m.size(); j++)
            memberFds[it->first].push_back(m[j]->getFd());

        const std::vector<Client*>& ops = it->second->getOperators();
        for (size_t j = 0; j < ops.size(); j++)
            operatorFds[it->first].push_back(ops[j]->getFd());

        const std::vector<Client*>& inv = it->second->getInvited();
        for (size_t j = 0; j < inv.size(); j++)
            invitedFds[it->first].push_back(inv[j]->getFd());
    }

    // ÉTAPE 2 : broadcast QUIT et retirer le client de ses channels
    for (std::map<std::string, Channel*>::iterator it = channel.begin();
         it != channel.end();)
    {
        if (it->second->isMember(&clients[index]))
        {
            Broadcast(it->second, final_msg);
            it->second->kick(&clients[index]);
        }
        if (it->second->getMembers().empty())
        {
            delete it->second;
            memberFds.erase(it->first);
            operatorFds.erase(it->first);
            invitedFds.erase(it->first);
            channel.erase(it++);
        }
        else
            ++it;
    }

    // ÉTAPE 3 : supprimer le client du vector
    close(fds[index + 1].fd);
    fds.erase(fds.begin() + index + 1);
    clients.erase(clients.begin() + index);

    // ÉTAPE 4 : rebuilder les pointeurs depuis les fd sauvegardés
    for (std::map<std::string, Channel*>::iterator it = channel.begin();
         it != channel.end(); ++it)
    {
        const std::string& name = it->first;
        Channel *chan = it->second;

        std::vector<Client*> newM, newO, newI;

        for (size_t j = 0; j < memberFds[name].size(); j++)
            for (size_t k = 0; k < clients.size(); k++)
                if (clients[k].getFd() == memberFds[name][j])
                    { newM.push_back(&clients[k]); break; }

        for (size_t j = 0; j < operatorFds[name].size(); j++)
            for (size_t k = 0; k < clients.size(); k++)
                if (clients[k].getFd() == operatorFds[name][j])
                    { newO.push_back(&clients[k]); break; }

        for (size_t j = 0; j < invitedFds[name].size(); j++)
            for (size_t k = 0; k < clients.size(); k++)
                if (clients[k].getFd() == invitedFds[name][j])
                    { newI.push_back(&clients[k]); break; }

        chan->rebuildMembers(newM);
        chan->rebuildOperators(newO);
        chan->rebuildInvited(newI);
    }
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
    {
        std::string err = read_code(401, sender->getNickname(), msg.params[1], "No such nick/channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }
    
    if (!chan->isMember(sender))
    {
        std::string err = read_code(442, sender->getNickname(), chanName, "You're not on that channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (!chan->isMember(sender))
    {
        std::string err = read_code(442, sender->getNickname(), chanName, "You're not on that channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (!chan->isOperator(sender) && msg.command != "TOPIC")
    {
        std::string err = read_code(482, sender->getNickname(), chan->getChannelName(), "You're not a channel operator");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (msg.command == "KICK")
        return handleKick(sender, chan, msg);
    else if (msg.command == "INVITE")
        return handleInvite(sender, chan, msg);
    else if (msg.command == "TOPIC")
        return handleTopic(sender, chan, msg);
    else if (msg.command == "MODE")
        return handleMode(sender, chan, msg);

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
        if (chan->getTopic().empty())
        {
            std::string r = read_code(331, sender->getNickname(), chan->getChannelName(), "No topic is set");
            send(sender->getFd(), r.c_str(), r.length(), 0);
        }
        else
        {
            std::string r = read_code(332, sender->getNickname(), chan->getChannelName(), chan->getTopic());
            send(sender->getFd(), r.c_str(), r.length(), 0);
        }
        return true;
    }

    if (chan->isTopicProtected() && !chan->isOperator(sender))
    {
        std::string err = read_code(482, sender->getNickname(), chan->getChannelName(), "You're not channel operator");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    chan->setTopic(concatParams(msg, 1));
    std::string prefix = sender->getNickname() + "!" + sender->getName() + "@localhost";
    std::string topic_msg = ":" + prefix + " TOPIC " + chan->getChannelName() + " :" + concatParams(msg, 1) + "\r\n";
    Broadcast(chan, topic_msg);
    return true;
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

    std::string prefix = sender->getNickname() + "!" + sender->getName() + "@localhost";
    std::string reason = (msg.params.size() >= 3) ? msg.params[2] : sender->getNickname();
    std::string kick_msg = ":" + prefix + " KICK " + chan->getChannelName() + " " + target->getNickname() + " :" + reason + "\r\n";
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

    Client *target = getClientByNick(msg.params[1]);

    if (target == NULL)
    {
        std::string err = read_code(401, sender->getNickname(), msg.params[1], "No such nick");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (target == sender)
    {
        std::string err = read_code(443, sender->getNickname(), msg.params[1], "is already on channel");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }
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

bool Server::handleMode(Client *sender, Channel *chan, const t_message &msg) //faut check que le channel existe ou le mode existe "MODE #inconnu +z"
{
    if (msg.params.size() < 2)
    {
        std::string err = read_code(461, sender->getNickname(), "MODE", "Not enough parameters.");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    bool type = true;
    
    if (msg.params[1][0] == '-')
        type = false;
    else if (msg.params[1][0] == '+')
        type = true;
    else
    {
        std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }

    if (msg.params[1][1] == 'i')
    {
        if (msg.params.size() != 2)
        {
            std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
            send(sender->getFd(), err.c_str(), err.length(), 0);
            return false;
        }
        chan->set_i(type);
        
        std::string prefix = sender->getNickname() + "!" + sender->getName() + "@localhost";
        std::string mode = type ? "+i" : "-i";
        std::string mode_msg = ":" + prefix + " MODE " + chan->getChannelName() + " " + mode + "\r\n";
        Broadcast(chan, mode_msg);

        return true;
    }

    if (msg.params[1][1] == 't')
    {
        if (msg.params.size() != 2)
        {
            std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
            send(sender->getFd(), err.c_str(), err.length(), 0);
            return false;
        }
        chan->set_t(type);

        std::string prefix = sender->getNickname() + "!" + sender->getName() + "@localhost";
        std::string mode = type ? "+t" : "-t";
        std::string mode_msg = ":" + prefix + " MODE " + chan->getChannelName() + " " + mode + "\r\n";
        Broadcast(chan, mode_msg);

        return true;
    }
    
    if (msg.params[1][1] == 'k')
    {
        if (type == false)
        {
            if (msg.params.size() != 2)
            {
                std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
                send(sender->getFd(), err.c_str(), err.length(), 0);
                return false;
            }
        }
        if (type == true)
        {
            if (msg.params.size() != 3)
            {
                std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
                send(sender->getFd(), err.c_str(), err.length(), 0);
                return false;
            }
        }
        chan->set_k(type);

        std::string prefix = sender->getNickname() + "!" + sender->getName() + "@localhost";

        if (type == false)
        {
            std::string mode_msg = ":" + prefix + " MODE " + chan->getChannelName() + " -k\r\n";
            Broadcast(chan, mode_msg);
            chan->setKey(""); 
        }
        if (type == true)
        {
            chan->setKey(msg.params[2]);
            std::string mode_msg = ":" + prefix + " MODE " + chan->getChannelName() + " +k\r\n";
            Broadcast(chan, mode_msg);
        }

        return true;
    }
    
    if (msg.params[1][1] == 'o')
    {
        if (msg.params.size() != 3)
        {
            std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
            send(sender->getFd(), err.c_str(), err.length(), 0);
            return false;
        }
        Client *target = getClientByNick(msg.params[2]);
        if (target == NULL)
        {
            std::string err = read_code(401, sender->getNickname(), msg.command, "No such nick");
            send(sender->getFd(), err.c_str(), err.length(), 0);
            return false;
        }
        if (type == true)
        {
            chan->addOperator(target);
        }
        if (type == false)
        {
            chan->removeOperator(target);
        }

        std::string prefix = sender->getNickname() + "!" + sender->getName() + "@localhost";
        std::string mode = type ? "+o" : "-o";
        std::string mode_msg = ":" + prefix + " MODE " + chan->getChannelName() + " " + mode + " " + target->getNickname() + "\r\n";
        Broadcast(chan, mode_msg);
        return true;
    }
    
    if (msg.params[1][1] == 'l')
    {
        if (type == false)
        {
            if (msg.params.size() != 2)
            {
                std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
                send(sender->getFd(), err.c_str(), err.length(), 0);
                return false;
            }
        }
        if (type == true)
        {
            if (msg.params.size() != 3)
            {
                std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
                send(sender->getFd(), err.c_str(), err.length(), 0);
                return false;
            }
        }

        std::string prefix = sender->getNickname() + "!" + sender->getName() + "@localhost";

        if (type == false)
        {
            chan->set_l(false);
            chan->setLimit(ULONG_MAX);
            std::cout << "l set à false" << std::endl;

            std::string mode_msg = ":" + prefix + " MODE " + chan->getChannelName() + " -l\r\n";
            Broadcast(chan, mode_msg);

            return true;
        }
        if (type == true)
        {
            if (chan->getMembers().size() > (size_t)atol(msg.params[2].c_str()))
            {
                std::string err = read_code(401, sender->getNickname(), msg.command, "Can only set a limit above the current number of members.");
                send(sender->getFd(), err.c_str(), err.length(), 0);
                return false;
            }
            chan->set_l(true);
            chan->setLimit(atol(msg.params[2].c_str()));
            std::cout << "l set à true" << std::endl;

            std::string mode_msg = ":" + prefix + " MODE " + chan->getChannelName() + " +l " + msg.params[2] + "\r\n";
            Broadcast(chan, mode_msg);
        }
        return true;
    }
    else
    {
        std::string err = read_code(461, sender->getNickname(), "MODE", "Wrong input parameters.");
        send(sender->getFd(), err.c_str(), err.length(), 0);
        return false;
    }
    
    return false;
}

std::string concatParams(const t_message &msg, size_t start)
{
    std::string result;

    for (size_t i = start; i < msg.params.size(); i++)
    {
        if (i != start)
            result += " ";
        result += msg.params[i];
    }
    return result;
}