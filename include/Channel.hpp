#pragma once

#include <map>
#include <string>

#include "parsing.hpp"
#include "User.hpp"

class Channel
{
	private:
		std::string _name;
		std::string _password;
		std::string _topic;
		unsigned int _nbUsers;
		unsigned int _maxUsers;
		bool _isTopicRestricted;
		bool _isWhitelisted;
		std::map<int, User *> _users;
		std::map<int, User *> _whitelist;
		std::map<int, User *> _operators;

		friend std::ostream&	operator<<(std::ostream& os, const Channel& channel);
		friend std::ostream&	operator<<(std::ostream& os, const std::map<std::string, Channel>& channels);

	public:
		Channel(const std::string& name);
		// Channel(const Channel& channel); // implement because of vector dynamic allocation in Server
		~Channel(void);

		bool	isUser(const User& user);
		bool	isWhitelist(const User& user);
		bool	isOperator(const User& user);

		const std::string& getName(void) const;
		const unsigned int& getNbUsers(void) const;
		const std::map<int, User *>& getUsers(void) const;
		const std::string& getTopic(void) const;
		const std::string& getPassword(void) const;
		bool isFull(void) const;
		bool isWhitelisted(void) const;
		bool isPasswordProtected(void) const;
		bool isTopicRestricted(void) const;
		unsigned int getMaxUsers(void) const;

		int addUser(User *user);
		int removeUser(const User& user);
		int addOP(const User& user);
		int removeOP(const User& user);

		void setWhitelisted(bool value);
		void setTopicRestricted(bool value);
		void setPassword(const std::string& password);
		void setMaxUsers(unsigned int max);
		void setTopic(const std::string& topic);
		void addToWhitelist(const User& user);
		void clearWhitelist(void);
};
