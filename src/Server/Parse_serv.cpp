#include "../../includes/irc.hpp"

void Server::parse_token(char *buffer, int result, int index, char *av)
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

		if (msg.command == "PING")
		{
			std::string pong = "PONG ";
			if (!msg.params.empty())
				pong += msg.params[0];
			pong += "\r\n";
			send(client_fd, pong.c_str(), pong.size(), 0);
			continue ;
		}

        if (!clients[index].getHasRegister())
        {
            authentication(cmd.c_str(), client_fd, index, av);
            check_register(client_fd, index);
        }
        else
        {
            if (msg.command == "NICK")
                change_nick(cmd.c_str(), client_fd, index);
            else if (msg.command == "PRIVMSG")
                private_message(index, cmd.c_str());
            else if (msg.command == "JOIN")
                Create_channel(cmd.c_str(), clients[index]);
            else if (msg.command == "KICK")
            {
                // funct KICK;
            }
            else if (msg.command == "INVITE")
            {
                // funct INVITE
            }
            else if (msg.command == "TOPIC")
            {
                // funct TOPIC;
            }
            else if (msg.command == "MODE")
            {
                // funct MODE;
            }
        }
    }
}

t_message	Server::parse_message(const std::string &line) {

	t_message	msg;
	size_t		pos = line.find(" :");
	std::string	first_part;
	std::string	trailing;
	bool		has_trailing = false;

	if (pos != std::string::npos) {

		first_part = line.substr(0, pos);
		trailing = line.substr(pos + 2);
		has_trailing = true;
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

	if (has_trailing) {
		msg.params.push_back(trailing);
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

void	Server::search_channel(std::string &old_nick, std::string &new_nick)
{
	std::set<Client*> ToNotify;
	std::cout << "force" << std::endl;
	std::map<std::string, Channel*>::iterator it = channel.begin();
	for (; it != channel.end(); it++)
	{
		for (size_t i = 0; i < it->second->getMembers().size(); i++)
		{
			if (it->second->getMembers()[i]->getNickname() == old_nick)
			{
				for (size_t j = 0; j < it->second->getMembers().size(); j++)
				{
					ToNotify.insert(it->second->getMembers()[j]);
				}
			}
		}
	}
	std::set<Client*>::iterator it_list = ToNotify.begin();
	for (; it_list != ToNotify.end(); it_list++)
	{
		std::cout << "ce Client est un ami a :"<< (*it_list)->getNickname() << std::endl;
	}
	std::ostringstream	oss;
	std::string			final_message;

	oss << ":" << old_nick << " NICK :" << new_nick << "\r\n";
	final_message = oss.str();
	std::set<Client*>::iterator it_set = ToNotify.begin();
	for (; it_set != ToNotify.end(); it_set++)
	{
		send((*it_set)->getFd(), final_message.c_str(), final_message.size(), 0);
	}
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
	search_channel(old_nick, msg.params[0]);
    clients[i].setNickname(msg.params[0]);

    std::string notif = ":" + old_nick + " NICK :" + msg.params[0] + "\r\n";
    send(fd, notif.c_str(), notif.size(), 0);

    std::cout << clients[i].getColor() << "Client [" << clients[i].getId()
              << "] changed nick from " << old_nick << " to " << msg.params[0]
              << "\033[0m" << std::endl;
}

bool Server::parse_channel(const std::string &name) {
	
    if (name.length() < 2 || name[0] != '#')
        return false;

    if (name.find_first_of(" ,\r\n\t") != std::string::npos)
        return false;

    return true;
}