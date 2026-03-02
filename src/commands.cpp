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
static void channmsg(t_msg *msg, Server &server);
void privmsg(t_msg *msg, Server &server)
{
	if (msg->params.size() < 2)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}
	if (msg->params[0][0] == '#' || msg->params[0][0] == '&') // Verify if target is a channel (starts with # or &)
	{
		channmsg(msg, server); // static 
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

static void channmsg(t_msg *msg, Server &server)
{
	Channel *target = utils::searchChannel(msg->params[0], server.getChannels());
	if (!target)
	{
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, msg->params[0]), msg->sfd);
		return;
	}
	
	// Get the sender user
	std::map<int, User *>::const_iterator sender_it = server.getUsers().find(msg->sfd);
	if (sender_it == server.getUsers().end())
		return;
	
	User *sender = sender_it->second;
	std::string message = msg->params[1];
	const std::map<int, User *>& users = target->getUsers();
	for (std::map<int, User *>::const_iterator it = users.begin(); it != users.end(); ++it)
	{
		utils::sendToUser(PRIVMSG(sender->getNickname(), sender->getUsername(), msg->hostname, msg->params[0], message), it->second);
	}
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

void join(t_msg *msg, Server &server)
{
	// Check if user is registered
	std::map<int, User *>::const_iterator user_it = server.getUsers().find(msg->sfd);
	if (user_it == server.getUsers().end() || !user_it->second->getCompleteInfo())
	{
		utils::sendToUser(ERR_NOTREGISTERED(server.getHostname(), msg->nickname), msg->sfd);
		return;
	}

	User *user = user_it->second;

	// Check if parameters are provided
	if (msg->params.size() < 1)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}

	std::string channel_name = msg->params[0];
	std::string password = (msg->params.size() > 1) ? msg->params[1] : "";

	// Check if channel name starts with # or &
	if (channel_name[0] != '#' && channel_name[0] != '&')
	{
		utils::sendToUser(ERR_BADCHANMASK(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// Try to join the channel
	int join_result = server.joinChannel(*user, channel_name, password);
	
	if (join_result == 1)
	{
		// User already in channel
		return;
	}
	else if (join_result == 2)
	{
		// Channel is full
		utils::sendToUser(ERR_CHANNELISFULL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	else if (join_result == 3)
	{
		// Wrong password
		utils::sendToUser(ERR_BADCHANNELKEY(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	else if (join_result != 0)
	{
		// Other error
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// Get the updated channel reference
	const std::map<std::string, Channel>& channels = server.getChannels();
	std::map<std::string, Channel>::const_iterator ch_it = channels.find(channel_name);
	if (ch_it == channels.end())
		return; // Should not happen

	const Channel &channel = ch_it->second;

	// Send JOIN notification to all users in the channel (including the new user)
	const std::map<int, User *>& users_in_channel = channel.getUsers();
	for (std::map<int, User *>::const_iterator it = users_in_channel.begin(); it != users_in_channel.end(); ++it)
	{
		utils::sendToUser(JOIN(user->getNickname(), user->getUsername(), msg->hostname, channel_name), it->second);
	}

	// Send channel topic to the new user
	if (!channel.getTopic().empty())
	{
		utils::sendToUser(RPL_TOPIC(server.getHostname(), msg->nickname, channel_name, channel.getTopic()), msg->sfd);
	}
	else
	{
		utils::sendToUser(RPL_NOTOPIC(server.getHostname(), msg->nickname, channel_name), msg->sfd);
	}

	// Send NAMES reply (list of users in the channel)
	std::string names_list = "";
	for (std::map<int, User *>::const_iterator it = users_in_channel.begin(); it != users_in_channel.end(); ++it)
	{
		if (!names_list.empty())
			names_list += " ";
		names_list += it->second->getNickname();
	}
	utils::sendToUser(RPL_NAMREPLY(server.getHostname(), msg->nickname, channel_name, names_list), msg->sfd);
	utils::sendToUser(RPL_ENDOFNAMES(server.getHostname(), msg->nickname, channel_name), msg->sfd);
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