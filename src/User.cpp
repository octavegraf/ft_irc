#include "User.hpp"

User::User(const int sfd) :
	_nickname(""), _username(""), _realname(""), _sfd(sfd)
{
}

User::~User(void)
{
}

void	User::setNickname(const std::string& nickname)
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

const std::string& User::getNickname(void)
{
	return (_nickname);
}

int User::command(t_msg *msg)
{
	(void)msg;
	return (0);
}
