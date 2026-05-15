/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wailas <wailas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 12:00:01 by wailas            #+#    #+#             */
/*   Updated: 2026/05/15 16:10:13 by wailas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Server::Server() {};

Server::~Server()
{
    for (size_t i = 0; i < fds.size(); i++)
        close(fds[i].fd);
    fds.clear();
};

void    Server::init_server(int port)
{
    int opt;
    this->next_id = 1;

    opt = 1;
    serveur_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(serveur_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < -1)
    {
        perror("error on setsockopt");
        exit(1);
    }
    if (serveur_fd < 0)
    {
        perror("Error : socket server");
        exit(0);
    }
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

void    Server::init_poll(char *av)
{
    struct pollfd   serveur_fd_poll;
    int             i;

    i = 0;
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
            std::string msg;
            
            fd = accept(this->serveur_fd, NULL, NULL);
            clientColor = getBackgroundColorCode(this->next_id);
            Client client(fd, this->next_id++, clientColor);
            this->clients.push_back(client); 
            this->fds.push_back(client.getCfp());
            msg =
            "\033[36m:ircserv NOTICE AUTH :Welcome to ircserv!\r\n"
            ":ircserv NOTICE AUTH :Use PASS <password> to authenticate\r\n"
            ":ircserv NOTICE AUTH :Use NICK <nickname> to choose a nickname\r\n"
            ":ircserv NOTICE AUTH :Use USER <username> 0 * :<realname> to register\r\n\033[0m";
            send(fd , msg.c_str(), msg.size(), 0);
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
                {
                    perror("funtion recv got an error");
                    exit(0);
                }
                else if (result == 0) {
                    std::cout << clients[i - 1].getColor() << "Client [" << clients[i - 1].getId() << "] deconnected\033[0m" << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    clients.erase(clients.begin() + i - 1);
                    i--;
                }
                else {
                    std::cout << clients[i - 1].getColor() << "Client [" << clients[i - 1].getId() << "] sent message : " << buffer << "\033[0m" << std::endl;
                    authentication(buffer, fds[i].fd, i - 1, av);
                    check_register(fds[i].fd, i - 1);
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

void    Server::check_register(int fd, size_t i)
{
    std::cout << "coucou " << std::endl;
    if (clients[i].getHasRegister()
        && clients[i].getHasPassword()
        && clients[i].getHasNickname()
        && clients[i].getHasUser())
    {
        clients[i].setRegister(true);

        std::string msg =
            ":ircserv 001 " + clients[i].getNickname() +
            " :Welcome to the IRC server\r\n";

        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
}

void    Server::authentication(char *buffer, int fd, size_t i, char *argv)
{
    std::ostringstream  oss;
    std::istringstream  iss(buffer);
    std::string         result;
    std::string         message, remains;

    // faut faire ici le parsing les petits potes
    if (!clients[i].getHasRegister())
    {
        iss >> message;
        iss >> remains;
        if ((message == "PASS" || message == "pass") && clients[i].getHasPassword() == 0)
        {
            if (remains == argv) {
                clients[i].setHasPassword(1);
                oss << "\033[32m:ircserv : Password accepted\033[0m" << std::endl;
                result = oss.str();
                send(fd, result.c_str(), result.size(), 0);
            }
            else {
                oss << "\033[31m:ircserv ERROR :Password incorrect\033[0m" << std::endl;
                result = oss.str();
                send(fd, result.c_str(), result.size() ,0);
            }
            return ;
        }
        if (message == "NICK" || message == "nick")
        {
            if (remains == "") {
                std::string result_str;
                oss << "\033[31m:ircserv ERROR : there's no nickname to change\033[0m" << std::endl;
                result_str = oss.str();
                send(fd, result_str.c_str(), result_str.size(), 0);
            }
            else {
                clients[i].setNickname(remains);
                std::cout << clients[i].getColor() << "Client [" << clients[i].getId() << "] changed his nickname to " << clients[i].getNickname() << std::endl;
                std::string result_str;
                oss << "\033[32m:ircserv : new nickname for your profil\033[0m" << std::endl;
                result_str = oss.str();
                send(fd, result_str.c_str(), result_str.size(), 0);
            }
            return ;
        }
        if (message == "USER" || message == "user")
        {
            std::string char_1, char_2, char_3;
            iss >> message;
            iss >> char_1;
            iss >> char_2;
            iss >> char_3;
            iss >> remains;
            if (char_1 == "")
                return ;
            if (char_2 == "")
                return ;
            if (char_3 == "")
                return ;
            if (remains == "")
                return ;
            clients[i].setUsername(char_1);
            clients[i].setRealname(remains);
            clients[i].setRegister(true);
            clients[i].setHasUsername(true);
            std::cout << clients[i].getColor() << "Client [" << clients[i].getId() << "] changed his nickname to " << clients[i].getNickname() << std::endl;
            std::string result_str;
            oss << "\033[32m:ircserv : :ircserv USER created\033[0m" << std::endl;
            result_str = oss.str();
            send(fd, result_str.c_str(), result_str.size(), 0);
            if (clients[i].getHasPassword() && clients[i].getHasNickname() && clients[i].getHasUser())
                clients[i].setRegister(1);
            return ;
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
