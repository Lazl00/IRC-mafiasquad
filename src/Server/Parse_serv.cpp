#include "../../includes/irc.hpp"


void	Server::parse_token(char *buffer, int result, int index, char *av) {

	clients[index].setBuffer(clients[index].getBuffer() + std::string(buffer, result));

	while (clients[index].getBuffer().find("\r\n") != std::string::npos) {

		std::string buf = clients[index].getBuffer();

		if (buf.find("\r\n")) {
			
		}
	}

}