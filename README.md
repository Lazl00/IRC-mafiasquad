<div align="center">

# 📡 ft_irc — MafiaSquad IRC Server

*A fully functional IRC server written in C++ as part of the 42 curriculum.*

[![Language](https://img.shields.io/badge/Language-C++98-blue.svg)](https://en.cppreference.com/w/cpp/98)
[![School](https://img.shields.io/badge/School-42_Paris-black.svg)](https://42.fr)
[![Status](https://img.shields.io/badge/Status-Finished-brightgreen.svg)]()

**Authors — [ainthana](https://github.com/ainthana) · [wailas](https://github.com/wailas) · [lcournoy](https://github.com/lcournoy)**

</div>

---

## 📖 About

**ft_irc** is a group project from the 42 curriculum. The goal is to implement an IRC *(Internet Relay Chat)* server in C++ from scratch, following the [RFC 1459](https://www.rfc-editor.org/rfc/rfc1459) protocol.

The project covers key systems programming concepts:

- **Non-blocking I/O** — all socket operations use `fcntl` and are multiplexed with `poll()`
- **TCP/IP networking** — handling multiple simultaneous client connections over sockets
- **IRC protocol** — parsing and handling standard IRC commands
- **Object-oriented design** — C++ classes for Server, Client, Channel, etc.

---

## 🚀 Getting Started

### Prerequisites

- A C++ compiler (`c++` / `g++`)
- `make`
- `ncat` or `irssi` (client to connect)

### Build & Run

```bash
# Clone the repository
git clone https://github.com/<your-repo>/ft_irc.git
cd ft_irc

# Compile
make

# Launch the server (choose any port and password)
./ircserv <port> <password>
```

The server is now listening for incoming connections.

---

## 🔌 Connecting to the Server

### With Ncat (quick test)

```bash
ncat -C <server-ip> <port>
# Use 'localhost' if you're on the same machine
```

Once connected, authenticate manually:

```
PASS <password>
NICK <your_nickname>
USER <username> 0 * :<realname>
```

### With Irssi (recommended IRC client)

```bash
/connect <server-ip> <port> <password>
```

---

## 💬 Supported Commands

### General

| Command | Syntax | Description |
|---------|--------|-------------|
| `PASS` | `PASS <password>` | Authenticate to the server |
| `NICK` | `NICK <nickname>` | Set or change your nickname |
| `USER` | `USER <user> 0 * :<realname>` | Register your user info |
| `JOIN` | `JOIN <#channel>` | Join or create a channel |
| `PRIVMSG` | `PRIVMSG <target> :<message>` | Send a message to a user or channel |
| `QUIT` | `QUIT [:<message>]` | Disconnect from the server |

### Channel Operator Commands

| Command | Syntax | Description |
|---------|--------|-------------|
| `KICK` | `KICK <#channel> <user> [:<reason>]` | Remove a user from a channel |
| `INVITE` | `INVITE <user> <#channel>` | Invite a user to a channel |
| `TOPIC` | `TOPIC <#channel> [:<new topic>]` | View or change the channel topic |
| `MODE` | `MODE <#channel> <+/-flag> [args]` | Change channel settings |

### Channel Modes (`MODE`)

| Flag | Description |
|------|-------------|
| `+i` / `-i` | Toggle **invite-only** mode |
| `+t` / `-t` | Restrict **TOPIC** changes to operators only |
| `+k` / `-k` | Set / remove a channel **password** |
| `+o` / `-o` | Grant / revoke **operator** privileges |
| `+l` / `-l` | Set / remove a **user limit** on the channel |

---

## ⭐ Bonus Features

### 🤖 Bot

A bot is available on the server. Send it commands via private message:

```
PRIVMSG bot !help
```

### 📁 File Transfer (Irssi only)

Send a file to another user:

```
/dcc send <nickname> <filepath>
```

Receive a file:

```
/dcc get <nickname>
```

---

## 🏗️ Architecture Overview

```
ft_irc/
├── src/
│   ├── Server.cpp       # Core server loop (poll, accept, dispatch)
│   ├── Client.cpp       # Client state & message parsing
│   ├── Channel.cpp      # Channel management & modes
│   ├── Commands/        # One file per IRC command
│   └── Bot.cpp          # Bonus bot logic
├── include/
├── Makefile
└── README.md
```

> The server uses a single `poll()` loop to handle all connected clients without blocking. No threads — pure event-driven I/O.

---

## 📚 Resources

- [RFC 1459 — Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459)
- [Modern IRC client protocol reference](https://modern.ircdocs.horse/)
- IA like CLaude, Gemini etc

---

<div align="center">

* — MafiaSquad*

</div>
