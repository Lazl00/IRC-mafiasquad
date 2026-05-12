/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wailas <wailas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 15:28:39 by wailas            #+#    #+#             */
/*   Updated: 2026/05/11 16:20:28 by wailas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"

Client::Client() {};
Client::~Client() {};
        
Client::Client(int fd, int id) : fd(fd), id(id)
{
    this->id = id;
    this->buffer = "";
    this->nickename = "";
    this->cfp.fd = fd;
    this->cfp.events = POLLIN;
    this->cfp.revents = 0;
    std::cout << "\033[1;41mClient [" << this->id<< "] connecte\033[0m"<< std::endl;
}

struct pollfd Client::getCfp()
{
    return (this->cfp);
}