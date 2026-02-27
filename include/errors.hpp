#pragma once

#define ERROR "Error: "
#define NOT_FOUND " not found."
#define ERR_INVALID_ARGC ERROR "Invalid arguments. Usage: ./ircserv <port> <password>"
#define ERR_INVALID_PORT ERROR "Invalid port number. Please use a port between 1 and 65535."
#define ERR_NOT_DIGIT ERROR "Digits expected."
#define ERR_INVALID_PARAMS "Invalid parameters."

// Source : https://www.rfc-editor.org/rfc/rfc1459.html

// Generic messages / variables
#define ERR_UNKNOWNCOMMAND(servername, client, command) (":" + servername + " 421 " + client + " " + command + " :Unknown command\r\n")
#define ERR_NEEDMOREPARAMS(servername, client, command) (":" + servername + " 461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_NOTREGISTERED(servername, client) (":" + servername + " 451 " + client + " :You have not registered\r\n")
#define CLIENT_ID(nickname, user, host) (":" + nickname + "!" + user + "@" + host)

// Client errors
#define ERR_NOPRIVILEGES(servername, nickname) (":" + servername + " 481 " + nickname + " :Permission Denied- You're not an IRC operator\r\n")
#define ERR_ALREADYREGISTERED(servername, client) (":" + servername + " 462 " + client + " :Unauthorized command (already registered)\r\n")

// CAP
#define CAP(servername, nickname) (":" + servername + " CAP LS :\r\n") // NOT IMPLEMENTED

// PASS
// No response if the password is correct
#define ERR_PASSWDMISMATCH(servername, client) (":" + servername + " 464 " + client + " :Password incorrect\r\n")

// NICK
#define NICK(old_nickname, user, host, nickname) (":" + old_nickname + "!" + user + "@" + host + " NICK " + nickname + "\r\n")
#define ERR_NONICKNAMEGIVEN(servername, client) (":" + servername + " 431 " + client + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(servername, client, nickname) (":" + servername + " 432 " + client + " " + nickname + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(servername, client, nickname) (":" + servername + " 433 " + client + " " + nickname + " :Nickname is already in use\r\n")

// USER
#define RPL_WELCOME(servername, nickname, user, host) (":" + servername + " 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "!" + user + "@" + host + "\r\n")
#define ERR_ALREADYREGISTRED(servername, client) (":" + servername + " 462 " + client + " :Unauthorized command (already registered)\r\n")
#define RPL_MOTDSTART(servername, nickname) (":" + servername + " 375 " + nickname + " :- " + servername + " Message of the Day -\r\n")
#define RPL_MOTD(servername, nickname, motd_line) (":" + servername + " 372 " + nickname + " :" + motd_line + "\r\n")
#define RPL_ENDOFMOTD(servername, nickname) (":" + servername + " 376 " + nickname + " :End of /MOTD command.\r\n")

// PRIVMSG / NOTICE
#define ERR_NOTEXTTOSEND(servername, nickname) (":" + servername + " 412 " + nickname + " :No text to send\r\n")

// PING / PONG
#define PING(CLIENT_ID, param) (CLIENT_ID + " PING :" + param + "\r\n")
#define PONG(CLIENT_ID, param) (CLIENT_ID + " PONG :" + param + "\r\n")
#define RPL_PONG(servername, param) (":" + servername + " PONG :" + param + "\r\n")

// OP
#define RPL_YOUREOPER(servername, nickname) (":" + servername + " 381 " + nickname + " :You are now an IRC operator\r\n")

// QUIT
#define QUIT(CLIENT_ID, reason) (CLIENT_ID + " QUIT :" + reason + "\r\n")

// KILL
#define KILL(source, reason) ("You have been disconeected from the server by " + source + ", because " + reason + "\r\n")
#define KILL_WOREASON(source) ("You have been disconeected from the server by " + source + "\r\n")

// Channel errors
#define ERR_NOSUCHCHANNEL(servername, nickname, channel) (":" + servername + " 403 " + nickname + " " + channel + " :No such channel\r\n")
#define ERR_USERNOTINCHANNEL(servername, nickname, target, channel) (":" + servername + " 441 " + nickname + " " + target + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(servername, nickname, channel) (":" + servername + " 442 " + nickname + " " + channel + " :You're not on that channel\r\n")
#define ERR_USERONCHANNEL(servername, nickname, target, channel) (":" + servername + " 443 " + nickname + " " + target + " " + channel + " :is already on channel\r\n")
#define ERR_BADCHANMASK(servername, nickname, channel) (":" + servername + " 476 " + nickname + " " + channel + " :Bad Channel Mask\r\n")
#define ERR_NOSUCHNICK(servername, nickname, target) (":" + servername + " 401 " + nickname + " " + target + " :No such nickname/channel\r\n")

// JOIN
#define JOIN(nickname, user, host, channel) (":" + nickname + "!" + user + "@" + host + " JOIN " + channel + "\r\n")
#define CREATEDCHANNEL(channel) (channel + " channel created\r\n")
#define ERR_INVITEONLYCHAN(servername, nickname, channel) (":" + servername + " 473 " + nickname + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(servername, nickname, channel) (":" + servername + " 475 " + nickname + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_CHANNELISFULL(servername, nickname, channel) (":" + servername + " 471 " + nickname + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_TOOMANYCHANNELS(servername, nickname, channel) (":" + servername + " 405 " + nickname + " " + channel + " :You have joined too many channels\r\n")

// INVITE
#define RPL_INVITING(servername, nickname, target, channel) (":" + servername + " 341 " + nickname + " " + target + " " + channel + "\r\n")
#define INVITE(nickname, user, host, target, channel) (":" + nickname + "!" + user + "@" + host + " INVITE " + target + " :" + channel + "\r\n")

// NAMES
#define RPL_NAMREPLY(servername, nickname, channel, clients) (":" + servername + " 353 " + nickname + " = " + channel + " :" + clients + "\r\n")
#define RPL_ENDOFNAMES(servername, nickname, channel) (":" + servername + " 366 " + nickname + " " + channel + " :End of /NAMES list\r\n")

// KICK
#define KICK(nickname, user, host, channel, target, reason) (":" + nickname + "!" + user + "@" + host + " KICK " + channel + " " + target + " :" + reason + "\r\n")

// PART
#define PART(nickname, user, host, channel, reason) (":" + nickname + "!" + user + "@" + host + " PART " + channel + " :" + reason + "\r\n")

// TOPIC
#define RPL_TOPIC(servername, nickname, channel, topic) (":" + servername + " 332 " + nickname + " " + channel + " :" + topic + "\r\n")
#define TOPIC(nickname, user, host, channel, topic) (":" + nickname + "!" + user + "@" + host + " TOPIC " + channel + " :" + topic + "\r\n")
#define RPL_NOTOPIC(servername, nickname, channel) (":" + servername + " 331 " + nickname + " " + channel + " :No topic is set\r\n")

// MODE
#define ERR_CHANOPRIVSNEEDED(servername, nickname, channel) (":" + servername + " 482 " + nickname + " " + channel + " :You're not channel operator\r\n")
#define MODE(nickname, user, host, target, modes, params) (":" + nickname + "!" + user + "@" + host + " MODE " + target + " " + modes + (params.empty() ? "" : " " + params) + "\r\n")
#define RPL_UMODEIS(servername, nickname, modes) (":" + servername + " 221 " + nickname + " " + modes + "\r\n")
#define ERR_KEYSET(servername, nickname, channel) (":" + servername + " 467 " + nickname + " " + channel + " :Channel key already set\r\n")
#define RPL_CHANNELMODEIS(servername, nickname, channel, modes) (":" + servername + " 324 " + nickname + " " + channel + " " + modes + "\r\n")
#define ERR_UNKNOWNMODE(servername, nickname, modechar) (":" + servername + " 472 " + nickname + " " + modechar + " :is unknown mode char to me\r\n")

// PRIVMSG
#define PRIVMSG(nickname, user, host, target, message) (":" + nickname + "!" + user + "@" + host + " PRIVMSG " + target + " :" + message + "\r\n")
#define NOTICE(nickname, user, host, target, message) (":" + nickname + "!" + user + "@" + host + " NOTICE " + target + " :" + message + "\r\n")
#define RPL_AWAY(servername, nickname, target, awaymsg) (":" + servername + " 301 " + nickname + " " + target + " :" + awaymsg + "\r\n")
