#pragma once

#include "Server.hpp"

#include <string>
#include <sys/socket.h>

class User
{
	private:
		std::string _nickname; // Server unique identifier
		std::string _username; // Not unique, server identifier
		std::string _realname; // For real name, contains spaces
		int _sfd; // Socket file descriptor
		// bool _isOnline;

		int setNickname(std::string const &nickname);
		int setUsername(std::string const &username);
		int setRealname(std::string const &realname);

	public:
		User(std::string nickname, std::string username, std::string realname, int sfd); // Contructor offline / online
		~User();
		std::string getNickname();
		int command(t_msg *msg);
};