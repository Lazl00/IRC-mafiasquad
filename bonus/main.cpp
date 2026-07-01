#include "../includes/irc.hpp"

int main(int argc, char **argv)
{
    Bot bot(argv[2], "bot");

    if (!bot.connect_to_server("127.0.0.1", std::atoi(argv[1])))
        return 1;

    bot.command_bot();

    char buf[512];
    while (true)
    {
        int n = recv(bot.getSock(), buf, sizeof(buf) - 1, 0);
        if (n <= 0)
            break;
        buf[n] = '\0';
        std::cout << buf;
    }
    return (0);
}
