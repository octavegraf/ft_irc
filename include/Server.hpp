#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>

#include "Channel.hpp"
#include "User.hpp"

#define CLIENT_LIMIT 1024

class Server
{
	private:
		const int				_port;
		const std::string		_password;
		const int				_listenSfd; // SFD =  Socket file descriptor
		struct pollfd			_pollfds[CLIENT_LIMIT];
		std::vector<Channel *>	_channels;
		std::vector<User *>		_users;

		int createChannel(std::string const &channelName);
		int deleteChannel(std::string const &channelName);
		int connectUser(User &user);
		int disconnectUser(User &user);
		int privateMsg(User const &sender, User const &target, std::string const &msg);
		int joinChannel(User const &user, std::string const &channelName, std::string const &password);
		int leaveChannel(User const &user, std::string const &channelName);

	public:
		Server(const char *port);
		Server(const char *port, const char *password);
		~Server(void);

		// getters
		const std::string				getPassword(void);
		const std::vector<Channel *>	&getChannels(void);
		const std::vector<User *>		&getUsers(void);

		int command(t_msg *msg);

		// exec
		// singatures in progreess
		void	acceptNewConnections(void);
		void	fetchNewEvents(void);
		void	handleNewEvents(void);
};

//static int dispatchCommand(t_msg *msg, Server &server);
