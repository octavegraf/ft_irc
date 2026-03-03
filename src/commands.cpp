#include "commands.hpp"
#include "utils.hpp"
#include <climits>
#include <cstdlib>

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
		return;
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
	
	// If user already has a username (was set before NICK), complete registration
	if (!user->getUsername().empty() && !user->getCompleteInfo())
	{
		// Send welcome message
		utils::sendToUser(RPL_WELCOME(server.getHostname(), user->getNickname(), user->getUsername(), server.getHostname()), msg->sfd);
		
		// Send motd
		utils::sendToUser(RPL_MOTDSTART(server.getHostname(), user->getNickname()), msg->sfd);
		utils::sendToUser(RPL_MOTD(server.getHostname(), user->getNickname(), "-"), msg->sfd);
		utils::sendToUser(RPL_MOTD(server.getHostname(), user->getNickname(), "Welcome to IRC"), msg->sfd);
		utils::sendToUser(RPL_ENDOFMOTD(server.getHostname(), user->getNickname()), msg->sfd);
		
		// Mark user as registered
		user->completeInfo(true);
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
	// Store username and realname
	newUser->setUsername(msg->params[0]);
	newUser->setRealname(msg->params[3]);

	// If NICK is already set, complete registration immediately
	if (!newUser->getNickname().empty())
	{
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
	// Else: NICK hasn't been set yet, so we store username/realname and wait for NICK
	// Registration will be completed in NICK handler when it's set
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
		// Don't send message back to the sender
		if (it->first != msg->sfd)
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
		utils::sendToUser(ERR_CHANNELISFULL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	else if (join_result == 3)
	{
		utils::sendToUser(ERR_BADCHANNELKEY(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	else if (join_result == 4)
	{
		utils::sendToUser(ERR_INVITEONLYCHAN(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	else if (join_result != 0)
	{
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

void part(t_msg *msg, Server &server)
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

	// Get channel before leaving to broadcast PART notification
	const std::map<std::string, Channel>& channels = server.getChannels();
	std::map<std::string, Channel>::const_iterator ch_it = channels.find(channel_name);
	
	if (ch_it == channels.end())
	{
		// Channel doesn't exist
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// Get COPY of list of users BEFORE leaving the channel (in case channel gets deleted)
	const Channel &channel = ch_it->second;
	std::map<int, User *> users_in_channel_copy = channel.getUsers();
	
	// Check if user is in the channel
	if (users_in_channel_copy.find(msg->sfd) == users_in_channel_copy.end())
	{
		// User not in channel
		utils::sendToUser(ERR_NOTONCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// Try to leave the channel
	int part_result = server.leaveChannel(*user, channel_name);
	
	if (part_result != 0)
	{
		// Error occurred
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// Send PART notification to all users who were in the channel (including the departing user)
	for (std::map<int, User *>::const_iterator it = users_in_channel_copy.begin(); it != users_in_channel_copy.end(); ++it)
	{
		utils::sendToUser(PART(user->getNickname(), user->getUsername(), msg->hostname, channel_name, ""), it->second);
	}
}

void kick(t_msg *msg, Server &server)
{
	if (msg->params.size() < 2)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}
	// Check if sender is channel operator
	Channel *channel = utils::searchChannel(msg->params[0], server.getChannels());
	if (!channel)
	{
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, msg->params[0]), msg->sfd);
		return;
	}
	// Find sender by socket fd (more reliable than nickname which can be empty)
	std::map<int, User *>::const_iterator sender_it = server.getUsers().find(msg->sfd);
	if (sender_it == server.getUsers().end())
	{
		#ifdef DEBUG
			std::cerr << "[KICK] Sender not found in server.getUsers() for sfd: " << msg->sfd << std::endl;
		#endif
		return;
	}
	
	User *sender = sender_it->second;
	
	#ifdef DEBUG
		std::cerr << "[KICK] Sender: " << sender->getNickname() << " (sfd:" << msg->sfd << ")" << std::endl;
		std::cerr << "[KICK] isUser: " << channel->isUser(*sender) << std::endl;
		std::cerr << "[KICK] isOperator: " << channel->isOperator(*sender) << std::endl;
	#endif
	
	// Check if sender is in this channel AND is operator
	if (!channel->isUser(*sender) || !channel->isOperator(*sender))
	{
		utils::sendToUser(ERR_CHANOPRIVSNEEDED(server.getHostname(), msg->nickname, msg->params[0]), msg->sfd);
		return;
	}
	if (msg->params[0][0] != '#' && msg->params[0][0] != '&')
	{
		utils::sendToUser(ERR_BADCHANMASK(server.getHostname(), msg->nickname, msg->params[0]), msg->sfd);
		return;
	}
	
	User *target = utils::searchUser(msg->params[1], channel->getUsers());
	if (!target)
	{
		utils::sendToUser(ERR_NOSUCHNICK(server.getHostname(), msg->nickname, msg->params[1]), msg->sfd);
		return;
	}
	channel->removeUser(*target);
	utils::sendToUser(KICK(sender->getNickname(), sender->getUsername(), msg->hostname, msg->params[0], msg->params[1], (msg->params.size() > 2 ? msg->params[2] : "You have been kicked from the channel for no particular reason.")), target);
}

void invite(t_msg *msg, Server &server)
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
	
	std::string channel_name = msg->params[1];
	
	// Normalize channel name - add # if not present
	if (channel_name[0] != '#' && channel_name[0] != '&')
	{
		utils::sendToUser(ERR_BADCHANMASK(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	
	Channel *channel = utils::searchChannel(channel_name, server.getChannels());
	if (!channel)
	{
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	
	std::map<int, User *>::const_iterator sender_it = server.getUsers().find(msg->sfd);
	if (sender_it == server.getUsers().end())
	{
		return;
	}
	
	User *sender = sender_it->second;
	
	// Check if sender is in this channel
	if (!channel->isUser(*sender))
	{
		utils::sendToUser(ERR_NOTONCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}
	
	// Check if target is already in the channel
	if (channel->isUser(*target))
	{
		utils::sendToUser(ERR_USERONCHANNEL(server.getHostname(), msg->nickname, msg->params[0], channel_name), msg->sfd);
		return;
	}
	
	// Send INVITE notification to the target
	utils::sendToUser(INVITE(sender->getNickname(), sender->getUsername(), msg->hostname, target->getNickname(), channel_name), target);
	
	// Add target to whitelist if channel is invite-only
	if (channel->isWhitelisted())
		channel->addToWhitelist(*target);
	
	// Send RPL_INVITING to the sender
	utils::sendToUser(RPL_INVITING(server.getHostname(), msg->nickname, channel_name, target->getNickname()), msg->sfd);
}

static int mode_i(bool add_mode, Channel *channel, std::string &mode_result);
static int mode_t_(bool add_mode, Channel *channel, std::string &mode_result); // name different due to conflicts during compilation
static int mode_k(bool add_mode, Channel *channel, t_msg *msg, Server &server, size_t &param_index, std::string &mode_result, std::string &mode_params_result);
static int mode_o(bool add_mode, Channel *channel, t_msg *msg, Server &server, size_t &param_index, std::string &mode_result, std::string &mode_params_result);
static int mode_l(bool add_mode, Channel *channel, t_msg *msg, Server &server, size_t &param_index, std::string &mode_result, std::string &mode_params_result);

void mode(t_msg *msg, Server &server)
{
	if (msg->params.size() < 2)
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return;
	}

	std::string channel_name = msg->params[0];
	Channel *channel = utils::searchChannel(channel_name, server.getChannels());
	if (!channel)
	{
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	std::map<int, User *>::const_iterator sender_it = server.getUsers().find(msg->sfd);
	if (sender_it == server.getUsers().end())
		return;

	User *sender = sender_it->second;
	
	// Check if sender is in this channel and is operator
	if (!channel->isUser(*sender) || !channel->isOperator(*sender))
	{
		utils::sendToUser(ERR_CHANOPRIVSNEEDED(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	std::string modes = msg->params[1];
	bool add_mode;
	if (msg->params[1][0] == '+')
		add_mode = true;
	else if (msg->params[1][0] == '-')
		add_mode = false;
	else
	{
		utils::sendToUser(ERR_UNKNOWNMODE(server.getHostname(), msg->nickname, std::string(1, msg->params[1][0])), msg->sfd);
		return;
	}
	size_t param_index = 2;
	std::string mode_result = "";
	std::string mode_params_result = "";
	
	for (size_t i = 1; i < modes.length(); ++i) // skipping + or - at index 0
	{
		char mode = modes[i];
		std::string param = "";
			if (param_index < msg->params.size())
				param = msg->params[param_index];
			int result = 0;
			switch(mode)
			{
				case 'i':
					result = mode_i(add_mode, channel, mode_result);
					break;
				case 't':
					result = mode_t_(add_mode, channel, mode_result);
					break;
				case 'k':
					result = mode_k(add_mode, channel, msg, server, param_index, mode_result, mode_params_result);
					break;
				case 'o':
					result = mode_o(add_mode, channel, msg, server, param_index, mode_result, mode_params_result);
					break;
				case 'l':
					result = mode_l(add_mode, channel, msg, server, param_index, mode_result, mode_params_result);
					break;
				default:
					utils::sendToUser(ERR_UNKNOWNMODE(server.getHostname(), msg->nickname, std::string(1, mode)), msg->sfd);
					return;
			}
			if (result != 0)
				return;
	}
	
	// Send MODE notification to all users in the channel
	const std::map<int, User *>& users = channel->getUsers();
	for (std::map<int, User *>::const_iterator it = users.begin(); it != users.end(); ++it)
	{
		utils::sendToUser(MODE(sender->getNickname(), sender->getUsername(), msg->hostname, channel_name, mode_result, mode_params_result), it->second);
	}
}

static int mode_i(bool add_mode, Channel *channel, std::string &mode_result)
{
	channel->setWhitelisted(add_mode);
	
	// If removing invite-only mode, clear the whitelist
	if (!add_mode)
		channel->clearWhitelist();
	
	mode_result += 'i';
	return 0;
}

static int mode_t_(bool add_mode, Channel *channel, std::string &mode_result)
{
	channel->setTopicRestricted(add_mode);
	mode_result += 't';
	return 0;
}

static int mode_k(bool add_mode, Channel *channel, t_msg *msg, Server &server, size_t &param_index, std::string &mode_result, std::string &mode_params_result)
{
	if (add_mode)
	{
		if (param_index >= msg->params.size())
		{
			utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
			return -1;
		}
		channel->setPassword(msg->params[param_index]);
		if (!mode_params_result.empty())
			mode_params_result += " ";
		mode_params_result += msg->params[param_index];
		++param_index;
		mode_result += 'k';
	}
	else
	{
		channel->setPassword("");
		mode_result += 'k';
	}
	return 0;
}

static int mode_o(bool add_mode, Channel *channel, t_msg *msg, Server &server, size_t &param_index, std::string &mode_result, std::string &mode_params_result)
{
	if (param_index >= msg->params.size())
	{
		utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
		return -1;
	}
	
	User *target = utils::searchUser(msg->params[param_index], channel->getUsers());
	if (!target)
	{
		utils::sendToUser(ERR_USERNOTINCHANNEL(server.getHostname(), msg->nickname, msg->params[param_index], msg->params[0]), msg->sfd);
		++param_index;
		return -1;
	}
	
	if (add_mode)
		channel->addOP(*target);
	else
		channel->removeOP(*target);
	
	if (!mode_params_result.empty())
		mode_params_result += " ";
	mode_params_result += msg->params[param_index];
	++param_index;
	mode_result += 'o';
	return 0;
}

static int mode_l(bool add_mode, Channel *channel, t_msg *msg, Server &server, size_t &param_index, std::string &mode_result, std::string &mode_params_result)
{
	if (add_mode)
	{
		if (param_index >= msg->params.size())
		{
			utils::sendToUser(ERR_NEEDMOREPARAMS(server.getHostname(), msg->nickname, msg->command), msg->sfd);
			return -1;
		}
		
		std::string limit_str = msg->params[param_index];
		unsigned int limit = std::atoi(limit_str.c_str());
		
		if (limit > 0)
		{
			channel->setMaxUsers(limit);
			if (!mode_params_result.empty())
				mode_params_result += " ";
			mode_params_result += limit_str;
			mode_result += 'l';
		}
		++param_index;
	}
	else
	{
		channel->setMaxUsers(0);
		mode_result += 'l';
	}
	return 0;
}

void topic(t_msg *msg, Server &server)
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

	// Get channel
	Channel *channel = utils::searchChannel(channel_name, server.getChannels());
	if (!channel)
	{
		utils::sendToUser(ERR_NOSUCHCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// Check if user is in the channel
	if (!channel->isUser(*user))
	{
		utils::sendToUser(ERR_NOTONCHANNEL(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// If no topic argument provided, display current topic
	if (msg->params.size() == 1)
	{
		if (channel->getTopic().empty())
			utils::sendToUser(RPL_NOTOPIC(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		else
			utils::sendToUser(RPL_TOPIC(server.getHostname(), msg->nickname, channel_name, channel->getTopic()), msg->sfd);
		return;
	}

	// Check permissions for setting topic (only ops if mode +t is set)
	if (channel->isTopicRestricted() && !channel->isOperator(*user))
	{
		utils::sendToUser(ERR_CHANOPRIVSNEEDED(server.getHostname(), msg->nickname, channel_name), msg->sfd);
		return;
	}

	// Set the new topic
	std::string new_topic = msg->params[1];
	channel->setTopic(new_topic);

	// Broadcast TOPIC notification to all users in the channel
	const std::map<int, User *>& users = channel->getUsers();
	for (std::map<int, User *>::const_iterator it = users.begin(); it != users.end(); ++it)
	{
		utils::sendToUser(TOPIC(user->getNickname(), user->getUsername(), msg->hostname, channel_name, new_topic), it->second);
	}
}
