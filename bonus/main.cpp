#include "../includes/irc.hpp"
#include "../includes/Server.hpp"
#include <sstream>
#include <cerrno>

bool g_running = true;

void handle_sig(int sig)
{
    (void)sig;
    g_running = false;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "./bot port password" << std::endl;
        return (1);
    }
    Bot bot(argv[2], "bot");

    if (!bot.connect_to_server("127.0.0.1", std::atoi(argv[1])))
        return (1);

    bot.command_bot();

    char buf[512];
    signal(SIGINT, handle_sig);
    while (g_running)
    {
        int n = recv(bot.getSock(), buf, sizeof(buf) - 1, 0);
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            break;
        }
        if (n == 0)
            break;
        buf[n] = '\0';
        std::string buffer(buf);
        std::cout << buf;
        bot.parsing_bot(buffer);
    }
    return (0);
}
