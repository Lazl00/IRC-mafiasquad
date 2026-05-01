/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wailas <wailas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 12:00:01 by wailas            #+#    #+#             */
/*   Updated: 2026/05/01 14:20:18 by wailas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Server::Server() {};

Server::~Server() {};

void    Server::init_server(int port)
{
    int opt;

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

int     Server::getServerFd() const
{
    return (this->serveur_fd);
}

