#pragma once

#include <map>
#include <string>

#include "parsing.hpp"
#include "User.hpp"

enum ChannelMode
{
	WHITELIST,
	TOPIC_RESTRICTED,
	PASSWORD_PROTECTED,
	OPERATORS,
	MAX_USERS
};

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

		void changePassword(const User& sender, const std::string& password);
		void addUserRights(const User& sender, const User& target); // check error message
		void removeUserRights(const User& sender, const User& target); // check error message
		void changeMaxUsersLimit(const User& sender, unsigned int maxUsers);

//		int sendMsg(const User& sender, const User& target, const std::string& msg);
		void inviteUser(const User& sender, const User& target);
		void topic(const User& sender, const std::string& param);
		void changeMode(const User& sender, ChannelMode mode, const std::string& param);

		friend std::ostream&	operator<<(std::ostream& os, const Channel& channel);
		friend std::ostream&	operator<<(std::ostream& os, const std::map<std::string, Channel>& channels);

	public:
		Channel(const std::string& name);
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
		int addUser(User *user);
		int removeUser(const User& user);
		int addOP(const User& user);
		int removeOP(const User& user);
		int command(t_msg *msg);
};
