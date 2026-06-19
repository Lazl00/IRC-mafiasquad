#include "../../includes/irc.hpp"

void	Server::parse_token(char *buffer, int result, int index, char *av) {

	clients[index].setBuffer(clients[index].getBuffer() + std::string(buffer, result));

	while (clients[index].getBuffer().find("\r\n") != std::string::npos) {

		std::string buf = clients[index].getBuffer();

		size_t		pos = buf.find("\r\n");
		std::string	cmd = buf.substr(0, pos);
		buf.erase(0, pos + 2);
		clients[index].setBuffer(buf);

		std::cout << clients[index].getColor() << "Client [" << clients[index].getId() 
          << "] sent message : " << cmd << "\033[0m" << std::endl;

		int	client_fd = clients[index].getFd();

		authentication(cmd.c_str(), client_fd, index, av);
		check_register(client_fd, index);
		private_message(index, cmd.c_str(), client_fd);
		Create_channel(cmd.c_str(), clients[index]);
	}
}