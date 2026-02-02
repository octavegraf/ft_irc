#pragma once

#include <iostream>
#include <vector>

#include "Channel.hpp"
#include "User.hpp"

class Server
{
	private:
		int const _port;
		std::string const _hostname;
		std::string const _password;
		std::vector<Channel> _channels;
		std::vector<User> _users;

		int createChannel(std::string const &channelName);
		int deleteChannel(std::string const &channelName);
		int connectUser(User &user);
		int disconnectUser(User &user);
		int privateMsg(User const &sender, User const &target, std::string const &msg);
		int joinChannel(User const &user, std::string const &channelName, std::string const &password);
		int leaveChannel(User const &user, std::string const &channelName);
	public:
		Server();
		~Server();
		int command(t_msg *msg);
};