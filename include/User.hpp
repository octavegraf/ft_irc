#pragma once

#include <string>
#include <sys/socket.h>

#include "message.hpp"

class User
{
	private:
		std::string _nickname; // Server unique identifier
		std::string _username; // Not unique, server identifier
		std::string _realname; // For real name, contains spaces
		int _sfd; // Socket file descriptor
		bool _completeInfos;

	public:
		User(const int sfd);
		User(const std::string& nickname, const std::string& username, const std::string& realname, int sfd); // Contructor offline / online
		~User();

		// getters
		const std::string& getNickname() const ;
		const std::string& getUsername() const ;
		const std::string& getRealName() const ;
		const int& getFd() const ;

		// setters
		void setNickname(std::string const &nickname);
		void setUsername(std::string const &username);
		void setRealname(std::string const &realname);
		void completeInfo(bool status);
		int command(t_msg *msg);
};
