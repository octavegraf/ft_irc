#pragma once

#include <vector>

#include "parsing.hpp"
#include "Server.hpp"
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
		bool _isTopicRestricted;
		std::vector<User *> _users;
		bool _isWhitelisted;
		std::vector<User *> _whitelist;
		std::vector<User *> _operators;
		unsigned int _maxUsers;

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
		Channel();
		~Channel();
		std::string getName();
		int command(t_msg *msg);
};
