#pragma once

#include <poll.h>
#include <string>
#include <signal.h>

#include "User.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "message.hpp"

namespace utils
{
	int setup_signal_action(struct sigaction *sa);
	User *searchUser(std::string nickname, std::map<int, User *> users);
	Channel *searchChannel(std::string name, const std::map<std::string, Channel>& channels);
	void sendToUser(const std::string& message, const int& sfd);
	void sendToUser(const std::string &message, const User *user);
	void sendToUser(const std::string &message, const std::map<int, User *> &users, const std::string nickname);
	void dispatchCommand(t_msg *msg, Server &server);
	void debugSendRight(const std::string &message);
}
