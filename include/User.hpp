#pragma once

#include <string>
#include <sys/socket.h>

class User
{
	private:
		std::string _nickname; // Server unique identifier
		std::string _username; // Not unique, server identifier
		std::string _realname; // For real name, contains spaces
		struct sockaddr _address; // sockaddr ? sockaddr_in ? sockaddr_in6 ?
		bool _isOnline;

		int setNickname(std::string const &nickname);
		int setUsername(std::string const &username);
		int setRealname(std::string const &realname);

	public:
		User(); // Contructor offline / online
		~User();
		int command(t_msg *msg);
};