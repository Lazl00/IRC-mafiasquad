This project has been created as part of the 42 curriculum by ainthana, wailas, lcournoy.


Description

Basically, this project teaches us how to use port opening to connect multiple users to an "IRC Server". We`ll have to manage mutiple connections without any blocking issues by using fcntl functions, and poll to catch users inputs. We'll also learn how to use sockets and use classes and objects in a concrete project.

Instructions

To create the server, you'll need to compile and lauch the executable by follozing steps:

- make                          (in the working dir)
- ./ircserv <port> <password>   (port and password of your choice)

The server now must be wainting for external connections.
You can now join it from a different computer of the network, or use multiple cmd terminals.

Type the following stuff in the terminal:

Ncat:
ncat -C <servers IP> <port>     (you can use 'localhost' if you're on the same computer than the server)

You should now be connected, to gain the IRC access you'll still need to give the server a few more infos, for security and authentification means. 

PASS <password>
NICK <nickname>
USER <username> 0 * :<realname>

Irssi:
/connect <servers IP> <port> <password>     (you can use 'localhost' if you're on the same computer than the server)

You should now have gained access to Mafiasquad's IRC !

Here's a few commands you might wanna try:

JOIN <#channel>
PRIVMSG <target/#channel> <:your message.>
QUIT (<:quiting message>)
NICK <new nickname>

Channel's specific commands:

KICK - Eject a client from the channel
INVITE - Invite a client to a channel
TOPIC - Change or view the channel topic
MODE - Change the channel’s mode (+/- to set or remove a flag)
 > i: Set/remove Invite-only channel
 > t: Set/remove the restrictions of the TOPIC command to channel operators
 > k: Set/remove the channel key (password)
 > o: Give/take channel operator privilege
 > l: Set/remove the user limit to channel

Resources

Documentation, AI


Bonus

Bot:
PRIVMSG bot !help

File transfer: (only on irssi)

/dcc send <target> <path>

/dcc get <sender>

Thank you for testing our project !
     -MafiaSquad