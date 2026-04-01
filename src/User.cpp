#include "User.hpp"

User::User(const int sfd) : 
	_nickname(""), _username(""), _realname(""), _sfd(sfd), _completeInfos(false), _parseBuffer(""), _authenticated(false)
{}

User::~User()
{
}

const std::string& User::getNickname(void) const
{
	return (this->_nickname);
}

const std::string& User::getUsername(void) const
{
	return (this->_username);
}

const std::string& User::getRealName(void) const
{
	return (this->_realname);
}

const int& User::getSfd(void) const
{
	return (this->_sfd);
}

std::string& User::getParseBuffer(void)
{
	return (this->_parseBuffer);
}

bool User::getCompleteInfo(void) const
{
	return (this->_completeInfos);
}

bool User::isPasswordAuthenticated(void) const
{
	return (this->_authenticated);
}

void User::setNickname(const std::string& nickname)
{
	this->_nickname = nickname;
}

void	User::setUsername(const std::string& username)
{
	this->_username = username;
}

void	User::setRealname(const std::string& realname)
{
	this->_realname = realname;
}

void User::setPasswordAuthenticated(const bool& status)
{
	this->_authenticated = status;
}

void User::completeInfo(const bool& status)
{
	this->_completeInfos = status;
}

std::ostream&	operator<<(std::ostream& os, const User& user)
{

}

std::ostream&	operator<<(std::ostream& os, const std::map<int, User *>& users)
{

}

