#pragma once

#include <map>

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
		std::map<int, User *> _users;
		std::map<int, User *> _whitelist;
		std::map<int, User *> _operators;
		unsigned int _maxUsers;
		bool _isTopicRestricted;
		bool _isWhitelisted;

		User const &stringToUser(std::string const &nickname);
		int changePassword(User const &sender, std::string const &password);
		int changeRights(User const &sender, User const &target);
		int changeUserLimits(User const &sender, unsigned int maxUsers);

		int sendMsg(User const &sender, User const &target, std::string const &msg);
		int kickUser(User const &sender, User const &target, std::string const &reason);
		int inviteUser(User const &sender, User const &target);
		int topic(User const &sender, std::string const &param);
		int changeMode(User const &sender, ChannelMode mode, std::string const &param);

	public:
		Channel(void);
		~Channel(void);
		std::string getName(void);
		int command(t_msg *msg);
};
