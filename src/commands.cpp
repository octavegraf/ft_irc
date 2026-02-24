#include "commands.hpp"
#include "utils.hpp"

void cap(t_msg *msg, Server &server)
{
	if (msg->params.size() < 1)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}
	// CAP LS response
	if (msg->params[0] == "LS")
	{
		utils::sendToUser(":" + server.getHostname() + " CAP * LS :\r\n", msg->sfd);
	}
	// CAP END response
	else if (msg->params[0] == "END")
	{
		// Client acknowledges CAP, connection can proceed
	}
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
	
	// Use * as placeholder for unregistered clients
	std::string client_id = msg->nickname.empty() ? "*" : msg->nickname;
	
	// Check if new nickname is already taken
	if (utils::searchUser(msg->params[0], users))
	{
		utils::sendToUser(ERR_NICKNAMEINUSE(server.getHostname(), client_id, msg->params[0]), msg->sfd);
		return;
	}
	
	// Find user by their socket fd
	std::map<int, User *>::iterator it = users.find(msg->sfd);
	if (it == users.end())
	{
		utils::sendToUser(ERR_NOTREGISTERED(server.getHostname(), client_id), msg->sfd);
		return;
	}
	
	User *user = it->second;
	std::string oldNickname = user->getNickname();
	if (oldNickname.empty())
		oldNickname = msg->params[0];
	
	// Double-check that nobody took this nick while we were here
	std::map<int, User *> users_check = server.getUsers();
	if (utils::searchUser(msg->params[0], users_check))
	{
		utils::sendToUser(ERR_NICKNAMEINUSE(server.getHostname(), client_id, msg->params[0]), msg->sfd);
		return;
	}
	
	user->setNickname(msg->params[0]);
	utils::sendToUser(NICK(oldNickname, user->getUsername(), server.getHostname(), msg->params[0]), msg->sfd);
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
	
	// NICK MUST be set before USER (RFC 1459)
	if (newUser->getNickname().empty())
	{
		utils::sendToUser(ERR_NOTREGISTERED(server.getHostname(), "*"), msg->sfd);
		return;
	}
	
	// Check if this user is already registered (has a username set)
	if (!newUser->getUsername().empty())
	{
		utils::sendToUser(ERR_ALREADYREGISTRED(server.getHostname(), msg->nickname), msg->sfd);
		return;
	}
	
	newUser->setUsername(msg->params[0]);
	newUser->setRealname(msg->params[3]);

	// Send welcome message (NICK and USERNAME are now both set)
	utils::sendToUser(RPL_WELCOME(server.getHostname(), newUser->getNickname(), newUser->getUsername(), server.getHostname()), msg->sfd);

	// Send motd
	utils::sendToUser(RPL_MOTDSTART(server.getHostname(), newUser->getNickname()), msg->sfd);
	utils::sendToUser(RPL_MOTD(server.getHostname(), newUser->getNickname(), "-"), msg->sfd);
	utils::sendToUser(RPL_MOTD(server.getHostname(), newUser->getNickname(), "Welcome to IRC"), msg->sfd);
	utils::sendToUser(RPL_ENDOFMOTD(server.getHostname(), newUser->getNickname()), msg->sfd);

	// Mark user as registered
	newUser->completeInfo(true);
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
	utils::sendToUser(PRIVMSG(msg->nickname, msg->username, msg->hostname, target->getNickname(), message), target);
}

void pingpong(t_msg *msg, Server &server)
{
	if (msg->params.size() < 1)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}
	
	// Send PONG response with the parameter received
	utils::sendToUser(RPL_PONG(server.getHostname(), msg->params[0]), msg->sfd);
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