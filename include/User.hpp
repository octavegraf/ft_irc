#pragma once

#include <string>
#include <sys/socket.h>
#include "message.hpp"

class User
{
	private:
		std::string	_nickname; // Server unique identifier
		std::string	_username; // Not unique, server identifier
		std::string	_realname; // For real name, contains spaces
		const int	_sfd; // unique
		// bool _isOnline;

	public:
		User(const int sfd); // Contructor offline / online
		User(const std::string& nickname, const std::string& username, const std::string& realname, int sfd); // Contructor offline / online
		~User(void);

		int command(t_msg *msg);

		// setters; check if names are mutable or not
		void	setNickname(const std::string& nickname);
		void	setUsername(const std::string& username);
		void	setRealname(const std::string& realname);

		const std::string&	getNickname(void);
};
