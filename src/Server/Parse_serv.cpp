#include "../../includes/irc.hpp"

void	Server::parse_token(char *buffer, int result, int index, char *av)
{
    clients[index].setBuffer(clients[index].getBuffer() + std::string(buffer, result));

    while (clients[index].getBuffer().find("\r\n") != std::string::npos)
    {
        std::string buf = clients[index].getBuffer();
        size_t      pos = buf.find("\r\n");
        std::string cmd = buf.substr(0, pos);
        buf.erase(0, pos + 2);
        clients[index].setBuffer(buf);

        std::cout << clients[index].getColor() << "Client [" << clients[index].getId()
                  << "] sent message : " << cmd << "\033[0m" << std::endl;

        int client_fd = clients[index].getFd();
		t_message msg = parse_message(cmd);

        if (!clients[index].getHasRegister())
        {
            authentication(cmd.c_str(), client_fd, index, av);
            check_register(client_fd, index);
        }
		else
		{
			if (msg.command == "NICK")
				change_nick(cmd.c_str(), client_fd, index);
			else
			{
				private_message(index, cmd.c_str(), client_fd);
				Create_channel(cmd.c_str(), clients[index]);
			}
		}
    }
}

t_message	Server::parse_message(const std::string &line) {

	t_message	msg;
	size_t		pos = line.find(" :");
	std::string	first_part;
	std::string	second_part;
	bool		has_second = false;

	if (pos != std::string::npos) {

		first_part = line.substr(0, pos);
		second_part = line.substr(pos + 2);
		has_second = true;
	}
	else {

		first_part = line;
	}

	std::istringstream	iss(first_part);
	iss >> msg.command;

	std::string word;
	while (iss >> word) {
		msg.params.push_back(word);
	}

	if (has_second) {
		msg.params.push_back(second_part);
	}

	return msg;
}

bool Server::is_valid_nick(std::string nick)
{
    if (nick.empty())
        return (false);

    char c = nick[0];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
        return (false);

    for (size_t i = 1; i < nick.size(); i++)
    {
        char ch = nick[i];
        bool is_letter = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
        bool is_digit  = (ch >= '0' && ch <= '9');
        bool is_special = (ch == '-' || ch == '_' || ch == '[' || ch == ']'
                        || ch == '{' || ch == '}' || ch == '\\' || ch == '`'
                        || ch == '^' || ch == '|');
        if (!is_letter && !is_digit && !is_special)
            return (false);
    }

    return (true);
}

std::string	Server::read_code(int code, std::string target, std::string params, std::string msg) {

	std::ostringstream oss;
	oss << ":ircserv " << code << " " << target;
	if (!params.empty())
		oss << " " << params;
	oss << " :" << msg << "\r\n";
	return oss.str();
}

void Server::change_nick(const char *buffer, int fd, size_t i)
{
    t_message msg = parse_message(buffer);
    std::string target = clients[i].getNickname();

    if (msg.params.empty())
    {
        std::string r = read_code(431, target, "", "No nickname given");
        send(fd, r.c_str(), r.size(), 0);
        return ;
    }
    if (!is_valid_nick(msg.params[0]))
    {
        std::string r = read_code(432, target, msg.params[0], "Erroneous nickname");
        send(fd, r.c_str(), r.size(), 0);
        return ;
    }
    if (exist_nick(msg.params[0]))
    {
        std::string r = read_code(433, target, msg.params[0], "Nickname is already in use");
        send(fd, r.c_str(), r.size(), 0);
        return ;
    }

    std::string old_nick = clients[i].getNickname();
    clients[i].setNickname(msg.params[0]);

    std::string notif = ":" + old_nick + " NICK :" + msg.params[0] + "\r\n";
    send(fd, notif.c_str(), notif.size(), 0);

    std::cout << clients[i].getColor() << "Client [" << clients[i].getId()
              << "] changed nick from " << old_nick << " to " << msg.params[0]
              << "\033[0m" << std::endl;
}