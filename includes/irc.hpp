#ifndef IRC_HPP
# define IRC_HPP

#include <iostream>
#include <string>       
#include <vector>       
#include <map> 
#include <cstring>
#include <cstdlib> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include "Server.hpp"
#include "Client.hpp"

void    handle_sig(int sig);

#endif