#include "User.hpp"

User::User(const int sfd) : 
	_nickname(""), _username(""), _realname(""), _sfd(sfd)
{}

User::~User()
{
}

const std::string& User::getNickname() const { return (_nickname); }

const std::string& User::getUsername() const { return (_username);}

const std::string& User::getRealName() const { return (_realname); }

const int& User::getFd() const { return (_sfd); }

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

void User::completeInfo(bool status)
{
	_completeInfos = status;
}

int User::command(t_msg *msg)
{
	// @octavegraf TODO
	(void)msg;
	return (0);
}
