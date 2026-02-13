#pragma once

#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>

#include "Channel.hpp"
#include "User.hpp"
#include "message.hpp"

#define CLIENT_LIMIT 1024
#define BUFFER_SIZE 1024

class Server
{
	private:
		const int						_port;
		const std::string				_hostname;
		const std::string				_password;
		const int						_listenSfd;
		struct pollfd					_pollfds[CLIENT_LIMIT];
		unsigned int					_nbUsers;
		std::map<std::string, Channel>	_channels; 
		std::map<int, User *>			_users; // store pointers because Channels will need to point to their users

		int	createChannel(const std::string& channelName);
		int	deleteChannel(const std::string& channelName);
		int	connectUser(User& user);
		int	disconnectUser(User& user);
		int	privateMsg(const User& sender, const User& target, const std::string& msg);
		int	joinChannel(const User& user, const std::string& channelName, const std::string& password);
		int	leaveChannel(const User& user, const std::string& channelName);

		void	printPollfds(void) const;
		friend std::ostream&	operator<<(std::ostream& os, const Server& server);
	public:
		Server(const char *port);
		Server(const char *port, const char *password);
		~Server(); // free Users pointed to by _users

		// getters
		const std::string& getPassword() const;
		const std::string& getHostname() const;
		const std::map<std::string, Channel>& getChannels() const;
		const std::map<int, User *>& getUsers() const;

		int addUser(User *user);
		int removeUser(User *user);
		int respond(int sfd, std::string message);
		int respond(User *user, std::string message);
		int respond(std::string nickname, std::string message);
		int command(t_msg *msg);

		// exec
		void	addPollfd(int client_sfd);
		void	acceptNewConnections(void);
		int		fetchNewEvents(void);
		void	handleNewEvents(void);
};

int dispatchCommand(t_msg *msg, Server &server);

