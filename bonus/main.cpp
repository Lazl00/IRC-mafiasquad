#include "../includes/irc.hpp"

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << "<port> <password>" << std::endl;
		return (1);
	}
	Bot	bot(av[2], "MyBot");
	return (0);
}