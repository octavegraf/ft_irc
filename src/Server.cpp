#include "Server.hpp"
#include "commands.hpp"
#include "utils.hpp"
#include <iomanip>

Server::Server(const char *port, const char *password) : _port(atoi(port)), _hostname("localhost"), _password(password), _listenSfd(getListenSfd(port)), _pollfds(), _lastPollfd(-1), _nbUsers(0), _channels(), _users()
{
	initPollfds();
}

Server::Server(const char *port) : _port(atoi(port)), _hostname("localhost"), _password(""), _listenSfd(getListenSfd(port)), _pollfds(), _lastPollfd(-1), _nbUsers(0), _channels(), _users()
{
	initPollfds();
}

Server::~Server(void)
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd != -1)
		{
#ifdef DEBUG

#endif
			close(this->_pollfds[i].fd);
		}
	}
	for (std::map<int, User *>::iterator it=this->_users.begin(); it != this->_users.end(); it++)
		delete it->second;
	close(this->_listenSfd);
}

const std::string& Server::getHostname() const
{
	return (this->_hostname);
}

const std::string& Server::getPassword() const
{
	return (this->_password);
}

const std::map<std::string, Channel>& Server::getChannels() const
{
	return (this->_channels);
}

const std::map<int, User *>& Server::getUsers() const
{
	return (this->_users);
}

void	Server::addUser(int sfd)
{
	// instantiate new User and add User to server's Users list
	this->_users[sfd] = new User(sfd);
	this->_nbUsers += 1;
	if (this->_password.empty()) // If server has no password, mark user as authenticated immediately
		this->_users[sfd]->setPasswordAuthenticated(true);
}


void	Server::updateLastPollfd(void)
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd != -1)
			this->_lastPollfd = i;
	}
}

int Server::createChannel(const std::string& channelName)
{
	if (_channels.find(channelName) != _channels.end())
		return (-1); // Channel already exists
	Channel newChannel(channelName);
	_channels.insert(std::make_pair(channelName, newChannel));
	return (0);
}

int Server::deleteChannel(const std::string& channelName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
		return (-1); // Channel doesn't exist
	_channels.erase(it);
	return (0);
}

int Server::joinChannel(const User& user, const std::string& channelName, const std::string& password)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	Channel *channel = NULL;
	bool is_new_channel = false;

	// If channel doesn't exist, create it
	if (it == _channels.end())
	{
		if (createChannel(channelName) != 0)
			return (-1);
		it = _channels.find(channelName);
		is_new_channel = true;
	}
	
	channel = &(it->second);
	
	// Check if user is already in channel
	if (channel->getUsers().find(user.getSfd()) != channel->getUsers().end())
		return (1); // User already in channel
	
	// Check if channel is full
	if (channel->isFull())
		return (2); // Channel is full
	
	// Check if channel is password protected
	if (channel->isPasswordProtected() && password != channel->getPassword())
		return (3); // Wrong password
	
	// Check if channel is invite-only (whitelist mode)
	if (channel->isWhitelisted() && !channel->isWhitelist(user))
		return (4); // Invite-only
	
	// Add user to channel first
	channel->addUser(const_cast<User *>(&user));
	
	// Make first user (creator) an operator
	if (is_new_channel)
	{
		channel->addOP(user);
		#ifdef DEBUG

		#endif
	}
	return (0); // Success
}

int Server::leaveChannel(const User& user, const std::string& channelName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
		return (-1); // Channel doesn't exist
	
	Channel *channel = &(it->second);
	if (channel->removeUser(user) != 0)
		return (1); // User not in channel
	
	// Delete empty channel
	if (channel->getNbUsers() == 0)
	{
		_channels.erase(it);
	}
	return (0);
}
void	Server::removePollfd(int sfd)
{}

void	Server::removeUser(int sfd)
{
	#ifdef DEBUG
		std::cerr << BLUE;
		std::cerr << "Removing user: " << *(this->_users[sfd]) << std::endl;
		std::cerr << RESET;
		std::cerr << "==========" << std::endl;
	#endif
	_users.erase(sfd);
	this->removePollfd(sfd);
	close(sfd);
	this->_nbUsers -= 1;
}

static void	fill_getaddrinfo_hints(struct addrinfo *hints, int ai_flags, int ai_family, int ai_socktype, int ai_protocol)
{}

void	Server::initPollfds(void)
{
	for (int i = 0; i < CLIENT_LIMIT; i++)
	{
		_pollfds[i].fd = -1;
		_pollfds[i].events = 0;
		_pollfds[i].revents = 0;
	}
	_pollfds[0].fd = this->_listenSfd;
	_pollfds[0].events = POLLIN;
	this->_lastPollfd = 0;

	_lastPollfd = 0;
}

void	Server::getProtocolConnexionInfo(struct addrinfo **info, const char *port, int ai_flags, int ai_family, int ai_socktype, const char *protocol)
{}

int	Server::bindPort(struct addrinfo *info)
{}

int Server::getListenSfd(const char *port)
{}

void	Server::addPollfd(int client_sfd)
{}

void	Server::acceptNewConnections(void)
{}

int	Server::fetchNewEvents(void)
{
	int ret = poll(this->_pollfds, this->_lastPollfd + 1, 100);
	if ( ret == - 1)
	{
		std::cerr << "Error: poll failed" << std::endl;
		throw std::exception();
	}
	return (ret);
}

void	Server::receive(int sfd, std::string& text)
{}

void	Server::handleNewEvents(void)
{
	int	nb_events = this->fetchNewEvents();
	int	handled = 0;
	for (int i = 0; i <= this->_lastPollfd && handled < nb_events; i++)
	{
		// pending incoming message
		if (this->_pollfds[i].fd != -1 && (this->_pollfds[i].revents & POLLIN) == POLLIN)
		{
			// get bytes
			std::string	text("");
			this->receive(this->_pollfds[i].fd, text);
			#ifdef DEBUG

			#endif
			// get msg
			t_msg msg;
			msg.sfd = _pollfds[i].fd;
			
			// Get user's parse buffer
			User* user = this->_users[this->_pollfds[i].fd];
			
			int parse_return = parsing(user->getParseBuffer(), text.c_str(), &msg);

			while (parse_return == 0)
			{
				// exec message
				utils::dispatchCommand(&msg, *this);
				handled += 1;
				parse_return = parsing(user->getParseBuffer(), "", &msg);
			}
		}
	}
}

void	Server::printPollfds(void) const
{}

std::ostream&	operator<<(std::ostream& os, const Server& server)
{}
