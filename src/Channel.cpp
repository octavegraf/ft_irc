#include "Channel.hpp"
#include "utils.hpp"

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

const std::map<int, User *>& Channel::getUsers(void) const
{
	return (this->_users);
}

const std::string& Channel::getTopic(void) const
{
	return (this->_topic);
}

const std::string& Channel::getPassword(void) const
{
	return (this->_password);
}

bool Channel::isFull(void) const
{
	return (this->_nbUsers >= this->_maxUsers);
}

bool Channel::isWhitelisted(void) const
{
	return (this->_isWhitelisted);
}

bool Channel::isPasswordProtected(void) const
{
	return (!this->_password.empty());
}

int Channel::addUser(User *user)
{
	if (this->_users.find(user->getSfd()) != this->_users.end())
		return (-1); // User already in channel
	this->_users[user->getSfd()] = user;
	this->_nbUsers++;
	return (0);
}

int Channel::removeUser(const User& user)
{
	if (this->_users.find(user.getSfd()) == this->_users.end())
		return (-1); // User not in channel
	this->_users.erase(user.getSfd());
	this->_nbUsers--;
	return (0);
}

int Channel::addOP(const User &user)
{
	if (this->isUser(user) && this->isOperator(user) == false)
		this->_operators[user.getSfd()] = const_cast<User *>(&user);
	return 0;
}

int Channel::removeOP(const User &user)
{
	if (this->isOperator(user))
		this->_operators.erase(user.getSfd());
	return 0;
}

bool Channel::isTopicRestricted(void) const
{
	return (this->_isTopicRestricted);
}

unsigned int Channel::getMaxUsers(void) const
{
	return (this->_maxUsers);
}

void Channel::setWhitelisted(bool value)
{
	this->_isWhitelisted = value;
}

void Channel::setTopicRestricted(bool value)
{
	this->_isTopicRestricted = value;
}

void Channel::setPassword(const std::string& password)
{
	this->_password = password;
}

void Channel::setMaxUsers(unsigned int max)
{
	this->_maxUsers = max;
}
void Channel::setTopic(const std::string& topic)
{
	this->_topic = topic;
}
void Channel::addToWhitelist(const User& user)
{
	this->_whitelist[user.getSfd()] = const_cast<User *>(&user);
}

void Channel::clearWhitelist(void)
{
	this->_whitelist.clear();
}

std::ostream&	operator<<(std::ostream& os, const Channel& channel)
{
	os << "**\tname: " << channel._name << std::endl;
	os << "\tpassword: " << channel._password << std::endl;
	os << "\ttopic: " << channel._topic << std::endl;
	os << "\tmaxUsers: " << channel._maxUsers << std::endl;
	os << "\tisTopicRestricted: " << channel._isTopicRestricted << std::endl;
	os << "\tisWhitelisted: " << channel._isWhitelisted << std::endl;
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
