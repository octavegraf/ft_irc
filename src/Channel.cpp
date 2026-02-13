#include "Channel.hpp"
#include "utils.hpp"

static bool	isValidPassword(const std::string& password)
{
	(void)password;
	return (true);
}

bool	Channel::isUser(const User& user)
{
	return (this->_users.find(user.getSfd()) != this->_users.end());
}

bool	Channel::isWhitelist(const User& user)
{
	return (this->_whitelist.find(user.getSfd()) != this->_whitelist.end());
}

bool	Channel::isOperator(const User& user)
{
	return (this->_operators.find(user.getSfd()) != this->_operators.end());
}

void	Channel::changePassword(const User& sender, const std::string& password)
{
	if (this->isOperator(sender))
	{
		if (isValidPassword(password))
		{
			this->_password = password;
			// send message to sender
		}
	}
}

void Channel::addUserRights(const User& sender, const User& target)
{
	if (this->isOperator(sender))
	{
		if (this->isUser(target) && this->isOperator(target) == false)
			this->_operators[target.getSfd()] = const_cast<User *>(&target);
		// send message OK?
	}
	// send message KO?
}

void Channel::removeUserRights(const User& sender, const User& target)
{
	if (this->isOperator(sender))
	{
		if (this->isUser(target) && this->isOperator(target))
			this->_operators.erase(target.getSfd());
		// send message OK?
	}
	// send message KO?
}

void	Channel::changeMaxUsersLimit(const User& sender, const unsigned int maxUsers)
{
	if (this->isOperator(sender))
	{
		if (this->getNbUsers() <= maxUsers)
			this->_maxUsers = maxUsers;
		// send message OK?
	}
	// send message KO?
}


Channel::Channel(const std::string& name):
	_name(name), _password(""), _topic(""), _nbUsers(0), _maxUsers(UINT_MAX), _isTopicRestricted(false), _isWhitelisted(false), _users(), _whitelist(), _operators()
{
}


Channel::~Channel(void)
{
}

const unsigned int&	Channel::getNbUsers(void) const
{
	return (this->_nbUsers);
}

const std::string& Channel::getName(void) const
{
	return (this->_name);
}

std::ostream&	operator<<(std::ostream& os, const Channel& channel)
{
	os << "**\tname: " << channel._name << std::endl;
	os << "\tpassword: " << channel._password << std::endl;
	os << "\ttopic: " << channel._topic << std::endl;
	os << "\tmaxUsers: " << channel._maxUsers << std::endl;
	os << "\tisTopicRestricted" << channel._isTopicRestricted << std::endl;
	os << "\tisWhitelisted" << channel._isWhitelisted << std::endl;
	os << "\tUsers:" << std::endl << channel._users << std::endl;
	os << "\twhitelist:" << std::endl << channel._whitelist << std::endl;
	os << "\toperators:" << std::endl << channel._operators << std::endl;
	return (os);
}

std::ostream& operator<<(std::ostream& os, const std::map<std::string, Channel>& channels)
{
	for (std::map<std::string, Channel>::const_iterator it=channels.begin(); it!=channels.end(); it++)
	{
		os << "*";
		os << it->second;
	}
	return (os);
}

/*
void sendMsg(const User& sender, const User& target, const std::string& msg)
{
	(void)sender;
	sendToUser(msg.c_str(), msg.length(), target);
	// send message to sender?
}*/

void Channel::kickUser(const User& sender, const User& target, const std::string& reason)
{
	if (this->isOperator(sender))
	{
		if (this->isUser(target))
		{
			this->_users.erase(target.getSfd());
			if (reason.compare("") == 0)
			{
				// send message to target and all _users
			}
		}
		// send message OK?
	}
	// send message KO?
}

void Channel::inviteUser(const User& sender, const User& target)
{
	if (this->isOperator(sender))
	{
		// check if whitelist restricted?
		if (this->isWhitelist(target) == false)
			this->_whitelist[target.getSfd()] = const_cast<User *>(&target);
			// send message OK?
		// send message KO?
	}
	// send message KO?
}

void	Channel::topic(const User& sender, const std::string& param)
{
	if (this->isOperator(sender))
	{
		this->_topic = param;
		// send message OK?
	}
	// send message KO?
}

//void changeMode(const User& sender, ChannelMode mode, const std::string& param);
