#ifndef IRC_HPP
# define IRC_HPP

#include <iostream>
#include <string>       
#include <vector>       
#include <map>
#include <sstream> 
#include <cstring>
#include <cstdlib> 
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include "Server.hpp"
#include "Client.hpp"
#include <fcntl.h>
#include <stdio.h>

extern Server *g_serv;
extern bool g_running;

void    handle_sig(int sig);
std::string concatParams(const t_message &msg, size_t start);


#endif