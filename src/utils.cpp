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
	(void)sig;
	interrupt = true;
}

int	utils::setup_signal_action(struct sigaction *sa)
{
	if (!sa)
		return (-1);
	std::memset(sa, 0, sizeof(*sa));
	sa->sa_handler = sigint_handler;
	sigemptyset(&sa->sa_mask);
	sa->sa_flags = 0;

	if (sigaction(SIGINT, sa, NULL) == -1)
	{
		std::cerr << "Error: failed to set up SIGINT handler" << std::endl;
		return (-1);
	}
	return (0);
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
	if(sfd < 0)
		return ;
	
	std::string out = message;
	if(out.size() < 2 || out.substr(out.size() - 2) != "\r\n")
		out += "\r\n";
	const char* buf = out.c_str();
	size_t totalSent = 0;
	size_t len = out.size();
	while (totalSent < len)
	{
		ssize_t sent = send(sfd, buf + totalSent, len - totalSent, 0);
		if(sent > 0)
			totalSent += static_cast<size_t>(sent);
		else if (sent == -1 && (errno == EWOULDBLOCK || errno == EAGAIN))
			break;
		else
			break;
	}
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
			//std::cerr << RED;
			std::cerr << "Executing command: " << msg->command << std::endl;
			//std::cerr << RESET;
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

std::ostream& operator<<(std::ostream& os, const User& user)
{
    os << "User[nick=" << user.getNickname()
       << ", user=" << user.getUsername()
       << ", sfd=" << user.getSfd() << "]";
    return os;
}
