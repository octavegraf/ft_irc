#include "utils.hpp"
#include "commands.hpp"
#include <iomanip>

extern bool	interrupt;

void utils::debugSendRight(const std::string &message)
{
	std::cerr << std::right << std::setw(80) << message << std::endl;
}

static void	sigint_handler(int sig)
{
}

int	utils::setup_signal_action(struct sigaction *sa)
{
}

User *utils::searchUser(std::string nickname, std::map<int, User *> users)
{
	if (nickname.empty())
		return (NULL);
	for (std::map<int, User *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
		{
#ifdef DEBUG
			std::cerr << BLUE;
			std::cerr << "User " << nickname << " found." << std::endl;
			std::cerr << RESET;
			std::cerr << "==========" << std::endl;
#endif
			return (it->second);
		}
	}
	return (NULL);
}

Channel *utils::searchChannel(std::string name, const std::map<std::string, Channel>& channels)
{
	if (name.empty())
		return (NULL);
	std::map<std::string, Channel>::const_iterator it = channels.find(name);
	if (it != channels.end())
		return (const_cast<Channel *>(&it->second));
	return (NULL);
}

void utils::sendToUser(const std::string &message, const int &sfd)
{
	struct pollfd	send_pollfd;
	send_pollfd.fd = sfd;
	send_pollfd.events = POLLOUT;
	send_pollfd.revents = 0;

	size_t	total_sent = 0;
	size_t	to_send = message.length();
	
#ifdef DEBUG
	std::cerr << YELLOW;
	std::cerr << "Sending:" << std::endl;
#endif
	while (total_sent < to_send)
	{
		int res_poll = poll(&send_pollfd, 1, 1);
		if (res_poll == -1)
			throw std::exception();
		if (res_poll == 0)
			continue;
		
		ssize_t sent = send(send_pollfd.fd, message.c_str() + total_sent, to_send - total_sent, 0);
#ifdef DEBUG
		std::cerr << message.substr(total_sent, total_sent + sent) << std::endl;
#endif
		if (sent == -1)
			throw std::exception();
		total_sent += sent;
	}
#ifdef DEBUG
	std::cerr << RESET;
#endif
}

void utils::sendToUser(const std::string &message, const User *user)
{
	utils::sendToUser(message, user->getSfd());
}

void utils::sendToUser(const std::string &message, const std::map<int, User *> &users, const std::string nickname)
{
	for (std::map<int, User *>::const_iterator it=users.begin(); it!=users.end(); it++)
	{
		if (it->second->getNickname() == nickname)
			utils::sendToUser(message, it->first);
	}
}

void utils::dispatchCommand(t_msg *msg, Server &server)
{
	const std::string 
	commandsList[] = {"CAP", "PASS", "NICK", "USER", "PRIVMSG", "PING", "JOIN", "PART", "KICK", "INVITE", "MODE", "TOPIC"};
	for (int i = 0; i < 12; i++)
	{
		if (msg->command == commandsList[i])
		{
#ifdef DEBUG
			std::cerr << RED;
			std::cerr << "Executing command: " << msg->command << std::endl;
			std::cerr << RESET;
#endif
			switch (i)
			{
				case 0:
					cap(msg, server);
					return;
				case 1:
					pass(msg, server);
					return;
				case 2:
					nick(msg, server);
					return;
				case 3:
					user(msg, server);
					return;
				case 4:
					privmsg(msg, server);
					return;
				case 5:
					pingpong(msg, server);
					return;
				case 6:
					join(msg, server);
					return;
				case 7:
					part(msg, server);
					return;
				case 8:
					kick(msg, server);
					return;
				case 9:
					invite(msg, server);
					return;
				case 10:
					mode(msg, server);
					return;
				case 11:
					topic(msg, server);
					return;
				default:
					utils::sendToUser(ERR_UNKNOWNCOMMAND(server.getHostname(), msg->nickname, msg->command), server.getUsers(), msg->nickname);
					return;
			}
		}
	}
}
