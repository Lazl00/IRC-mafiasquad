/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wailas <wailas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 15:28:39 by wailas            #+#    #+#             */
/*   Updated: 2026/05/06 16:34:57 by wailas           ###   ########.fr       */
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
}

struct pollfd Client::getCfp()
{
    return (this->cfp);
}