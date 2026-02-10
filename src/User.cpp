#include "User.hpp"

User::User(const int sfd) : 
	_nickname(""), _username(""), _realname(""), _sfd(sfd)
{}

std::string User::getNickname() const { return (_nickname); }

std::string User::getUsername() const { return (_username);}

std::string User::getRealName() const { return (_realname); }

int User::getFd() const { return (_sfd); }

int	User::setNickname(const std::string& nickname)
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

void User::completeInfo(bool status)
{
	_completeInfos = true;
}

int User::command(t_msg *msg)
{
	// @octavegraf TODO
	return (0);
}
