#pragma once

#include <iostream>
#include <map>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>

#include "Channel.hpp"
#include "User.hpp"

#define CLIENT_LIMIT 1024

class Server
{
	private:
		const int						_port;
		const std::string				_password;
		const int						_listenSfd;
		std::vector<struct pollfd>		_pollfds;
		std::map<std::string, Channel>	_channels; 
		std::map<int, User *>			_users; // store pointers because Channels will need to point to their users

		int	createChannel(const std::string& channelName);
		int	deleteChannel(const std::string& channelName);
		int	connectUser(User& user);
		int	disconnectUser(User& user);
		int	privateMsg(const User& sender, const User& target, const std::string& msg);
		int	joinChannel(const User& user, const std::string& channelName, const std::string& password);
		int	leaveChannel(const User& user, const std::string& channelName);

	public:
		Server(const char *port);
		Server(const char *port, const char *password);
		~Server(void); // free Users pointed to by _users

		// getters
		const std::string&						getPassword(void);
		const std::map<std::string, Channel>&	getChannels(void);
		const std::map<int, User *>&			getUsers(void);

		int command(t_msg *msg);

		// exec
		// signatures in progreess
		void	acceptNewConnections(void);
		void	fetchNewEvents(void);
		void	handleNewEvents(void);
};

//static int dispatchCommand(t_msg *msg, Server &server);
