#pragma once

#include <iostream>
#include <map>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
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
		const int						_port;
		const std::string				_hostname;
		const std::string				_password;
		const int						_listenSfd;
		std::map<int, struct pollfd>	_pollfds;
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
		~Server(); // free Users pointed to by _users

		// getters
		const std::string& getPassword();
		const std::string& getHostname();
		const std::map<std::string, Channel>& getChannels();
		const std::map<int, User *>& getUsers();
		int respond(User *user, std::string message);
		int respond(std::string nickname, std::string message);
		int command(t_msg *msg);

		// exec
		// signatures in progreess
		void	acceptNewConnections();
		void	fetchNewEvents();
		void	handleNewEvents();
};

//static int dispatchCommand(t_msg *msg, Server &server);
