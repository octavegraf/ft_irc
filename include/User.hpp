#pragma once

#include <string>
#include <map>
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
		std::string _parseBuffer; // Parsing buffer for this user

		friend std::ostream&	operator<<(std::ostream& os, const User& user);
		friend std::ostream&	operator<<(std::ostream& os, const std::map<int, User *>& users);

	public:
		User(const int sfd);
		~User();

		// getters
		const std::string& getNickname(void) const;
		const std::string& getUsername(void) const;
		const std::string& getRealName(void) const;
		const int& getSfd() const ;
		std::string& getParseBuffer(void);

		// setters
		void setNickname(std::string const &nickname);
		void setUsername(std::string const &username);
		void setRealname(std::string const &realname);
		void completeInfo(const bool& status);
		int command(t_msg *msg);
};
