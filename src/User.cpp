#include "User.hpp"

User::User(const int sfd) :
	_nickname(""), _username(""), _realname(""), _sfd(sfd)
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

std::string User::getNickname()
{
	return (_nickname);
}

int User::command(t_msg *msg)
{
	return (0);
}
