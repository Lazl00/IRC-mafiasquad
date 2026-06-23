# ================== VARIABLES ================== #

NAME        = ircserv
BOT_NAME    = bot
CC          = c++
CFLAGS      = -Wall -Wextra -Werror -std=c++98
RM          = rm -rf

SRCS        = main.cpp \
              src/Server/Server.cpp \
              src/Client/Client.cpp \
              src/Channel/Channel.cpp

BOT_SRCS    = bonus/main.cpp \
              bonus/bot.cpp \

DIR_OBJS    = objs
OBJS        = $(SRCS:%.cpp=$(DIR_OBJS)/%.o)
BOT_OBJS    = $(BOT_SRCS:%.cpp=$(DIR_OBJS)/%.o)

# ================== COLORS ================== #

RED         = \033[0;31m
GREEN       = \033[0;32m
YELLOW      = \033[0;33m
RESET       = \033[0m

# ================== RULES ================== #

all: $(NAME) $(BOT_NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@printf "$(GREEN)✓ $(NAME) compilé avec succès$(RESET)\n"

$(BOT_NAME): $(BOT_OBJS)
	@$(CC) $(CFLAGS) $(BOT_OBJS) -o $(BOT_NAME)
	@printf "$(GREEN)✓ $(BOT_NAME) compilé avec succès$(RESET)\n"

$(DIR_OBJS)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "$(YELLOW)Compilation de $<...$(RESET)\n"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) $(DIR_OBJS)
	@printf "$(RED)Suppression des objets$(RESET)\n"

fclean: clean
	@$(RM) $(NAME) $(BOT_NAME)
	@printf "$(RED)Suppression de $(NAME) et $(BOT_NAME)$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re