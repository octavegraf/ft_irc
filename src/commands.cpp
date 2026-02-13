#include "commands.hpp"
#include "utils.hpp"

void cap(t_msg *msg, Server &server)
{
	// return (utils::sendToUser(msg->nickname, CAP(server.getHostname(), msg->nickname)));
	std::cout << "test";
	(void)msg; (void)server;
}

void pass(t_msg *msg, Server &server)
{
	if (msg->params.size() < 1)
		utils::sendToUser(ERR_PASSWDMISMATCH(server.getHostname(), msg->nickname), msg->sfd);
	else if (msg->params[0] != server.getPassword())
		utils::sendToUser(ERR_PASSWDMISMATCH(server.getHostname(), msg->nickname), msg->sfd);
}

void nick(t_msg *msg, Server &server)
{
	if (msg->params.size() < 1)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), "", msg->command), msg->sfd);
		return;
	}
	
	std::map<int, User *> users = server.getUsers();
	if (utils::searchUser(msg->params[0], users))
	{
		utils::sendToUser(ERR_NICKNAMEINUSE(server.getHostname(), msg->nickname, msg->params[0]), msg->sfd);
		return;
	}
	else
	{
		std::string oldNickname = msg->nickname;
		User *user = utils::searchUser(oldNickname, users);
		if (user)
		{
			user->setNickname(msg->params[0]);
			utils::sendToUser(NICK(oldNickname, user->getUsername(), user->getRealName(), msg->params[0]), msg->sfd);
		}
		else
			utils::sendToUser(ERR_NOTREGISTERED(server.getHostname(), msg->nickname), msg->sfd);
	}
}

void user(t_msg *msg, Server &server)
{
	if (msg->params.size() < 4)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}

	std::map<int, User *>::const_iterator it = server.getUsers().find(msg->sfd);
	if (it == server.getUsers().end())
		return;
	
	User* newUser = it->second;
	// Check if this user is already registered (has a username set)
	if (!newUser->getUsername().empty())
	{
		utils::sendToUser(ERR_ALREADYREGISTRED(server.getHostname(), msg->nickname), msg->sfd);
		return;
	}
	
	newUser->setUsername(msg->params[0]);
	newUser->setRealname(msg->params[3]);
	utils::sendToUser(RPL_WELCOME(server.getHostname(), msg->nickname, newUser->getUsername(), newUser->getRealName()), msg->sfd);
	utils::sendToUser(RPL_WELCOME(server.getHostname(), msg->nickname, newUser->getUsername(), newUser->getRealName()), msg->sfd);
}

void privmsg(t_msg *msg, Server &server)
{
	if (msg->params.size() < 2)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}
	User *target = utils::searchUser(msg->params[0], server.getUsers());
	if (!target)
	{
		utils::sendToUser(ERR_NOSUCHNICK(server.getHostname(), msg->nickname, msg->params[0]), msg->sfd);
		return;
	}
	std::string message = msg->params[1];
	utils::sendToUser(CLIENT_ID(msg->nickname, msg->username, msg->hostname) + " PRIVMSG " + target->getNickname() + " :" + message, target);
}

void pingpong(t_msg *msg, Server &server)
{
	(void)msg;
	(void)server;
	// IDK if its for ping @user or test the server. Dont know how to implement it. @octavegraf
}

void mode(t_msg *msg, Server &server)
{
	(void)msg;
	(void)server;
	// @octavegraf TODO
	return;
}

/*	./a.out localhost 6666
	ai_flags: 0
	ai_family: 2
	ai_socktype: 1
	ai_protocol: 6
	ai_addrlen: 16
	ai_addr->sa_data: 26.10.127.0.0.1.0.0.0.0.0.0.0.0.
	ai_canonname: (null)
	sfd: 3, read_sfd: 4
	CAP LS
	PASS motdepasse
	NICK ocgraf
		// nickname
	USER ocgraf ocgraf localhost :Octave Graf
		// username hostname servername realname
*/