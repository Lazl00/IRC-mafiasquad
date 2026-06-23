#include "../includes/irc.hpp"

Bot::Bot(const std::string& password, const std::string& nickname) : _password(password), _nickname(nickname) {};

Bot::~Bot() {};
	
void	Bot::command_bot(int socket_fd_serv, char *buffer, char *mdp)
{	
	_sock = socket(AF_INET, SOCK_STREAM, 0);

	std::stringstream	oss;
	std::string 		message_mdp;

	(void)buffer;
	std::cout << "Bot connection" << std::endl;
	if  (!_sock)
	{
		oss << "PASS " << mdp << "\r\n";
		oss >> message_mdp;
		send(socket_fd_serv, message_mdp.c_str(), message_mdp.size(), 0);
		send(socket_fd_serv, "NICK bot\r\n", sizeof(11), 0);
		send(socket_fd_serv, "USER bot 0 * : bot\r\n", sizeof(21), 0);
	}
}