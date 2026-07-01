#include "../includes/irc.hpp"

Bot::Bot(const std::string& password, const std::string& nickname) : _password(password), _nickname(nickname) {};

Bot::~Bot() {};

bool	Bot::connect_to_server(const std::string& host, int port)
{
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock < 0)
	{
		std::cerr << "Erreur socket " << std::endl;
		return (false);
	}

	struct sockaddr_in serv_addr;
	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_port = htons(port);
	serv_addr.sin_family = AF_INET;
	in_addr_t addr = inet_addr(host.c_str());

	if (addr == INADDR_NONE)
	{
		std::cerr << "Adress IP invalide";
		return (false);
	}
	serv_addr.sin_addr.s_addr = addr;

	if (connect(_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr) < 0))
	{
		std::cerr << "Error Connect()" << std::endl;
		return (false);
	}
	std::cout << "Connecté au serveur avec succès, fd : " << _sock << std::endl;
	return (true);
}

void	Bot::command_bot()
{	
	std::string msg_pass = "PASS " + _password + "/r/n";
	send(_sock, msg_pass.c_str(), msg_pass.size(), 0);

	std::string msg_nick = "NICK " + _nickname + "/r/n";
	send(_sock, msg_nick.c_str(), msg_nick.size(), 0);

	std::string msg_user = "USER " + _nickname + " 0 * : " + _nickname + "/r/n";
	send(_sock, msg_user.c_str(), msg_user.size(), 0);
}