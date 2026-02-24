#include "User.hpp"

User::User(const int sfd) : 
	_nickname(""), _username(""), _realname(""), _sfd(sfd), _completeInfos(false), _parseBuffer("")
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

void User::completeInfo(const bool& status)
{
	this->_completeInfos = status;
}

int User::command(t_msg *msg)
{
	// @octavegraf TODO
	(void)msg;
	return (0);
}

std::ostream&	operator<<(std::ostream& os, const User& user)
{
	os << "\tnickname: " << user._nickname << std::endl;
	os << "\tusername: " << user._username << std::endl;
	os << "\trealname: " << user._realname << std::endl;
	os << "\tsfd: " << user._sfd << std::endl;
	os << "\tcompleteInfos: " << user._completeInfos << std::endl;
	return (os);
}

std::ostream&	operator<<(std::ostream& os, const std::map<int, User *>& users)
{
	for (std::map<int, User *>::const_iterator it=users.begin(); it != users.end(); it++)
	{
		os << "*";
		os << *(it->second);
	}
	return (os);
}

