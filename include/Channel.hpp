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

		bool	isUser(const User& user);
		bool	isWhitelist(const User& user);
		bool	isOperator(const User& user);

		void changePassword(const User& sender, const std::string& password);
		void addUserRights(const User& sender, const User& target); // check error message
		void removeUserRights(const User& sender, const User& target); // check error message
		void changeMaxUsersLimit(const User& sender, unsigned int maxUsers);

//		int sendMsg(const User& sender, const User& target, const std::string& msg);
		void kickUser(const User& sender, const User& target, const std::string& reason);
		void inviteUser(const User& sender, const User& target);
		void topic(const User& sender, const std::string& param);
		void changeMode(const User& sender, ChannelMode mode, const std::string& param);

		friend std::ostream&	operator<<(std::ostream& os, const Channel& channel);
		friend std::ostream&	operator<<(std::ostream& os, const std::map<std::string, Channel>& channels);

	public:
		Channel(const std::string& name);
		Channel(const Channel& channel); // implement because of vector dynamic allocation in Server
		~Channel(void);
		const std::string& getName(void) const;
		const unsigned int& getNbUsers(void) const;
		int command(t_msg *msg);
};
