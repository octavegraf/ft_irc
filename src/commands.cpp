#include "commands.hpp"

User *searchUser(std::string nickname, std::map<int, User *> users)
{
	for (std::map<int, User *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return (it->second);
	}
	#ifdef DEBUG
		std::cerr << "@octavegraf replace" << std::endl;
	#endif
	return (NULL);
}

int dispatchCommand(t_msg *msg, Server &server)
{
	const std::string commandsList[] = {"CAP", "PASS", "NICK", "USER", "PRIVMSG"};
	for (int i = 0; i < 10; i++)
	{
		if (msg->command == commandsList[i])
		{
			switch (i)
			{
				case 0:
					return (cap(msg, server));
				case 1:
					return (pass(msg, server));
				case 2:
					return (nick(msg, server));
				case 3:
					return (user(msg, server, 0));
				case 4:
					return (privmsg(msg, server));
				default:
					break;
			}
		}
	}
	return (server.respond(msg->nickname, ERR_UNKNOWNCOMMAND(server.getHostname(), msg->nickname, msg->command)));
}

int cap(t_msg *msg, Server &server)
{
	return (server.respond(msg->nickname,  CAP(server.getHostname(), msg->nickname)));
}

int pass(t_msg *msg, Server &server)
{
	if (msg->params.size() < 1)
		return (server.respond(msg->nickname, ERR_PASSWDMISMATCH(server.getHostname(), msg->nickname)));
	else if (msg->params[0] != server.getPassword())
		return (server.respond(msg->nickname, ERR_PASSWDMISMATCH(server.getHostname(), msg->nickname)));
	else
		return (0);
}

int nick(t_msg *msg, Server &server)
{
	std::map<int, User *> users = server.getUsers();
	if (searchUser(msg->params[0], users))
		return (server.respond(msg->nickname, ERR_NICKNAMEINUSE(server.getHostname(), msg->nickname, msg->params[0])));
	else
	{
		std::string oldNickname = msg->nickname;
		User *user = searchUser(oldNickname, users);
		if (user)
		{
			user->setNickname(msg->params[0]);
			std::string response = NICK(oldNickname, user->getUsername(), user->getRealName(), msg->params[0]);
			return (server.respond(msg->nickname, response));
		}
		else
			return (server.respond(msg->nickname, ERR_NOTREGISTERED(server.getHostname(), msg->nickname)));
	}
}

int user(t_msg *msg, Server &server, int sfd)
{
	if (msg->params.size() < 4)
		return (server.respond(msg->nickname, ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command)));

	std::map<int, User *> users = server.getUsers();
	User *user = searchUser(msg->nickname, users);
	if (user)
	{
		return (server.respond(msg->nickname, ERR_ALREADYREGISTRED(server.getHostname(), msg->nickname)));
	}
	else
	{
		User *newUser = new User(sfd);
		newUser->setNickname(msg->nickname);
		newUser->setUsername(msg->params[0]);
		newUser->setRealname(msg->params[3]);
		if (!server.addUser(newUser))
		{
			server.respond(msg->nickname, RPL_WELCOME(server.getHostname(), msg->nickname, newUser->getUsername(), newUser->getRealName()));
			return (0);
		}
		else
		{
			delete newUser;
			return (server.respond(msg->nickname, ERR_ERRONEUSNICKNAME(server.getHostname(), msg->nickname, msg->params[0])));
		}
	}
}

int privmsg(t_msg *msg, Server &server)
{
	if (msg->params.size() < 2)
		return (server.respond(msg->nickname, ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command)));
	User *target = searchUser(msg->params[0], server.getUsers());
	if (!target)
		return (server.respond(msg->nickname, ERR_NOSUCHNICK(server.getHostname(), msg->nickname, msg->params[0])));
	std::string message = msg->params[1];
	return (server.respond(target, CLIENT_ID(msg->nickname, msg->username, msg->hostname) + " PRIVMSG " + target->getNickname() + " :" + message));
}

int pingpong(t_msg *msg, Server &server)
{
	(void)msg;
	(void)server;
	// IDK if its for ping @user or test the server. Dont know how to implement it. @octavegraf
	return (1);
}

int mode(t_msg *msg, Server &server)
{
	(void)msg;
	(void)server;
	// @octavegraf TODO
	return (1);
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