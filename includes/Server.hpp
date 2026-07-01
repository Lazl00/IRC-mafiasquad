#pragma once
#include "Client.hpp"
#include <vector>

struct t_message {

    std::string                 command;
    std::vector<std::string>    params;
};

class Channel
{
    private:
        std::string              _name;
        std::string              _topic;
        std::vector<Client*>     _members;
        std::vector<Client*>     _invited;
        std::vector<Client*>     _operators;
        bool _i;
        bool _t;
        bool _k;
        bool _o;
        bool _l;

    public:
        Channel();
        ~Channel();
        Channel(std::string &name);
        const std::string& getName() const;
        const std::vector<Client*>& getMembers() const;
        void addMember(Client *client);
        void addOperator(Client *client);
        void invite(Client *client);
        void setName(std::string name);
        void setTopic(std::string topic);
        void seeTopic();
        void mode(char cmd, std::string arg);
        void kick(Client *client);
};

class Bot
{
    private:
        int         _sock;
        std::string _recvBuffer;
        std::string _password;
        std::string _nickname;
    public:
        Bot(const std::string& password, const std::string& nickname);
        ~Bot();

        void command_bot(int socket_fd_serv, char *buffer, char *mdp);

};

class Server
{
    private:
        int										serveur_fd;
        std::vector<struct pollfd>				fds;
        std::vector<Client>						clients;
        std::map<std::string, Channel*>			channel;
        int										next_id;
        bool                                    _flagBot;

    public:
        Server();
        ~Server();
        void                        Create_channel(const char *buffer, Client &client);
        void                        Broadcast(Channel *chan, std::string msg);
        void                        sig_handler();
        void                        init_server(int port);
        int                         getServerFd() const;
        void                        init_poll(char *av);
        void                        authentication(const char *buffer, int fd, size_t i, char *av);
        void                        private_message(int i, const char *buffer);
        bool                        exist_nick(std::string nickname);
        std::string                 getBackgroundColorCode(int socket);
        std::vector<struct pollfd>  getFds() const;
        void                        check_register(int fd, size_t i);
        Channel*                    getChannel(const std::string &name);
		void		                parse_token(char *buffer, int result, int index, char *av);
        t_message                   parse_message(const std::string &line);
        bool                        is_valid_nick(std::string nick);
        std::string	                read_code(int code, std::string target, std::string params, std::string msg);
        void                        change_nick(const char *buffer, int fd, size_t i);
        size_t						find_client_by_nick(std::string nickname);
		bool						parse_channel(const std::string &name);
};