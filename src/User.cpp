#include "User.hpp"

User::User(const int sfd) : 
	_nickname(""), _username(""), _realname(""), _sfd(sfd)
{}

User::~User()
{
}

std::string User::getNickname() const { return (_nickname); }

std::string User::getUsername() const { return (_username);}

std::string User::getRealName() const { return (_realname); }

int User::getFd() const { return (_sfd); }

void User::setNickname(const std::string& nickname)
{
	_nickname = nickname;
}

void	User::setUsername(const std::string& username)
{
	_username = username;
}

void	User::setRealname(const std::string& realname)
{
	_realname = realname;
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
