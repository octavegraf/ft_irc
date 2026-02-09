#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <poll.h>
#include <sys/socket.h>
#include <strings.h>

#include "Channel.hpp"
#include "User.hpp"
#include "commands.hpp"

#define CLIENT_LIMIT 1024

class Server
{
	private:
		int const _port;
		int const _listenSfd; // SFD =  Socket file descriptor
		struct pollfd _pollfds[CLIENT_LIMIT];
		std::string const _hostname;
		std::string const _password;
		std::vector<Channel *> _channels;
		std::vector<User *> _users;

		int createChannel(std::string const &channelName);
		int deleteChannel(std::string const &channelName);
		int connectUser(User &user);
		int disconnectUser(User &user);
		int privateMsg(User const &sender, User const &target, std::string const &msg);
		int joinChannel(User const &user, std::string const &channelName, std::string const &password);
		int leaveChannel(User const &user, std::string const &channelName);

	public:
		Server(char const *port, int const listenSfd, std::string const hostname);
		Server(char const *port, int const listenSfd, std::string const hostname, std::string const password);
		~Server();

		// getters
		std::string const getHostname();
		std::string const getPassword();
		std::vector<Channel *> getChannels();
		std::vector<User *> getUsers();

		int respond(User *user, std::string message);
		int respond(std::string nickname, std::string message); // alias for simple use
		int command(t_msg *msg);
};

static int dispatchCommand(t_msg *msg, Server &server);

#endif