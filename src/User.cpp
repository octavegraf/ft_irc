#include "User.hpp"

User::User(std::string nickname, int sfd) : _nickname(nickname), _sfd(sfd), _completeInfos(false)
{
	#ifdef DEBUG
		std::cout << "User constructor called: " << nickname << " created with fd " << sfd << "." << std::endl;
	#endif
}

std::string User::getNickname() const { return (_nickname); }

std::string User::getUsername() const { return (_username);}

std::string User::getRealName() const { return (_realname); }

int User::getFd() const { return (_sfd); }

int User::command(t_msg *msg)
{
	// @octavegraf TODO
	return (0);
}