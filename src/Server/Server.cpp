/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wailas <wailas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 12:00:01 by wailas            #+#    #+#             */
/*   Updated: 2026/05/06 16:53:20 by wailas           ###   ########.fr       */
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

void    Server::init_poll()
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
            int fd;
            
            fd = accept(this->serveur_fd, NULL, NULL);
            Client client(fd, this->next_id++);
            this->clients.push_back(client);
            this->fds.push_back(client.getCfp());
        }
        for (size_t i = 1; i < fds.size(); i++)
        {
            char buffer[512];
            if (fds[i].revents & POLLIN)
            {
                int result;
                
                result = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (result < 0)
                {
                    perror("funtion recv got an error");
                    exit(0);
                }
                else if (result == 0)
                    std::cout << "deconect"<< std::endl;
                else {
                    std::cout << "Client [" << i << "] sent message : \n" << buffer << std::endl;
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

