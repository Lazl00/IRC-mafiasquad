#pragma once
#include "Client.hpp"
#include <vector>
#include <set>
#include <limits.h>

struct t_message {

    std::string                 command;
    std::vector<std::string>    params;
};

class Channel
{
    private:
        std::string              _name; // channels name
        std::string              _topic; // channels topic
        std::string              _key; // channels key
        unsigned long            _limit; // channels limit
        std::vector<Client*>     _members; // channels members
        std::vector<Client*>     _invited; // channels invited clients
        std::vector<Client*>     _operators; // channels operators
        bool _i; // Invite-only (on-off)
        bool _t; // open TOPIC command (on-off)
        bool _k; // Key (on-off)
        bool _l; // User limit (on-off)

    public:
        Channel();
        ~Channel();
        Channel(std::string &name);
        const std::string& getChannelName() const;
        const std::vector<Client*>& getMembers() const;
        const std::string&			getTopic() const;
        bool isLimited() const;
        bool hasKey() const;
        bool isInviteOnly() const;
        bool isTopicProtected() const;
        unsigned long getLimit() const;
        void setLimit(unsigned long limit);
        const std::string &getKey() const;
        void addMember(Client *client);
        void addOperator(Client *client);
        void removeOperator(Client *client);
        void removeInvited(Client *client);
        void invite(Client *client);
        void setName(std::string name);
        void setTopic(std::string topic);
        void seeTopic();
        void kick(Client *client);
        bool isInvited(Client *client) const;
        bool isMember(Client *client) const;
        bool isOperator(Client *client) const;
        void set_i(bool type);
        void set_t(bool type);
        void set_k(bool type);
        void set_l(bool type);
        void setKey(std::string key);
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

        int     getSock() const;
        bool    connect_to_server(const std::string& host, int port);
        void    command_bot();
        void	parsing_bot(std::string &buffer);
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
        void                        check_register(int fd, int i);
        Channel*                    getChannel(const std::string &name);
		bool	                	parse_token(char *buffer, int result, int index, char *av);
        t_message                   parse_message(const std::string &line);
        bool                        is_valid_nick(std::string nick);
        std::string	                read_code(int code, std::string target, std::string params, std::string msg);
        void                        change_nick(const char *buffer, int fd, size_t i);
        size_t						find_client_by_nick(std::string nickname);
		bool						parse_channel(const std::string &name);
        void	                    search_channel(std::string &old_nick, std::string &new_nick);
		void						cleanup(size_t index, std::string msg);
        bool                        handleOpCmds(Client *sender, const t_message &msg);
        bool                        channelExists(const std::string &name) const;
        bool                        handleTopic(Client *sender, Channel* chan, const t_message &msg);
        bool                        handleKick(Client *sender, Channel* chan, const t_message &msg);
        bool                        handleInvite(Client *sender, Channel* chan, const t_message &msg);
        bool                        handleMode(Client *sender, Channel* chan, const t_message &msg);
        Client*                     getClientByNick(const std::string &nick);

};