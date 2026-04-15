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
			close(this->_pollfds[i].fd);
		}
	}
	for (std::map<int, User *>::iterator it=this->_users.begin(); it != this->_users.end(); it++)
		delete it->second;
	// close(this->_listenSfd);
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
{
	for (int i = 1; i < CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd == sfd)
		{
			this->_pollfds[i].fd = -1;
			this->_pollfds[i].events = 0;
			this->_pollfds[i].revents = 0;
			if (i == this->_lastPollfd)
			{
				while(this->_lastPollfd >= 0 && this->_pollfds[this->_lastPollfd].fd == -1)
					this->_lastPollfd -= 1;
			}
			return ;
		}
	}
	std::cerr << "Error: could not find client sfd in pollfds to remove" << std::endl;
}

void	Server::removeUser(int sfd)
{
	#ifdef DEBUG
		std::map<int, User *>::iterator debugIt = this->_users.find(sfd);	
		if(debugIt != this->_users.end() && debugIt->second)
		{
			std::cerr << "Removing user: " << sfd << 
			" nick: " << debugIt->second->getNickname() <<
			" user: " << debugIt->second->getUsername() << std::endl;
		}
		else
			std::cerr << "Removing user with sfd: " << sfd << std::endl;
	#endif
	std::map<int, User *>::iterator it = this->_users.find(sfd);
	if (it != this->_users.end())
	{
		delete it->second;
		_users.erase(it);
		_nbUsers -= 1;
	}
	removePollfd(sfd);
	close(sfd);

	// _users.erase(sfd);
	// this->removePollfd(sfd);
	// close(sfd);
	// this->_nbUsers -= 1;
}

// static void	fill_getaddrinfo_hints(struct addrinfo *hints, int ai_flags, int ai_family, int ai_socktype, int ai_protocol)
// {}

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
}

void	Server::getProtocolConnexionInfo(struct addrinfo **info, const char *port, int ai_flags, int ai_family, int ai_socktype, const char *protocol)
{
	(void)protocol;

	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_flags = ai_flags;
	hints.ai_family = ai_family;
	hints.ai_socktype = ai_socktype;
	int ret = getaddrinfo(NULL, port, &hints, info);
	if (ret != 0)
	{
		std::cerr << "Error: getaddrinfo failed: " << gai_strerror(ret) << std::endl;
		throw std::exception();
	}
}

int	Server::bindPort(struct addrinfo *info)
{
	for (struct addrinfo *ptr = info; ptr != NULL; ptr = ptr->ai_next)
	{
		int sfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sfd == -1)
			continue ;
		int yes = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		{
			close(sfd);
			std::cerr << "Error: setsockopt failed" << std::endl;
			continue;
		}
		int flags = fcntl(sfd, F_GETFL, 0);
		if (flags == -1 || fcntl(sfd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			std::cerr << "Error: failed to set socket to non-blocking" << std::endl;
			close(sfd);
			continue ;
		}
		if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == 0)
			return (sfd);
		close(sfd);
	}
	std::cerr << "Error: failed to bind to any address" << std::endl;
	throw std::exception();
}

int Server::getListenSfd(const char *port)
{
	struct addrinfo *info = NULL;

	getProtocolConnexionInfo(&info, port, AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, "tcp");
	int sfd = bindPort(info);
	freeaddrinfo(info);

	if (listen(sfd, SOMAXCONN) == -1)
	{
		std::cerr << "Error: listen failed" << std::endl;
		close(sfd);
		throw std::exception();
	}
	return (sfd);
}

void	Server::addPollfd(int client_sfd)
{
	for (int i = 1; i < CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd == -1)
		{
			this->_pollfds[i].fd = client_sfd;
			this->_pollfds[i].events = POLLIN;
			this->_pollfds[i].revents = 0;
			if (i > this->_lastPollfd)
				this->_lastPollfd = i;
			return ;
		}
	}
	std::cerr << "Error: too many clients connected, cannot add new client" << std::endl;
	close(client_sfd);
}

void	Server::acceptNewConnections(void)
{
	while(true)
	{
		int client_sfd = accept(this->_listenSfd, NULL, NULL);
		if (client_sfd == -1)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break ;
			std::cerr << "Error: accept failed" << std::endl;
			throw std::exception();
		}
		#ifdef DEBUG
			std::cerr << "ACCEPT client fd=" << client_sfd << std::endl;
		#endif
		//limite nb clients
		if (this->_nbUsers >= CLIENT_LIMIT - 1)
		{
			std::cerr << "Error: too many clients connected, rejecting new client" << std::endl;
			close(client_sfd);
			continue ;
		}
		// mettre client non bloquant
		int flags = fcntl(client_sfd, F_GETFL, 0);
		if (flags == -1 || fcntl(client_sfd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			std::cerr << "Error: failed to set client socket to non-blocking" << std::endl;
			close(client_sfd);
			continue ;
		}
		this->addUser(client_sfd);
		this->addPollfd(client_sfd);
	}
}

int	Server::fetchNewEvents(void)
{
	int ret = poll(this->_pollfds, this->_lastPollfd + 1, 100);
	if ( ret == - 1)
	{
		if (errno == EINTR)
			return (0); // ctrl c no errror
		std::cerr << "Error: poll failed" << std::endl;
		throw std::exception();
	}
	return (ret);
}
// 1 = ok 0 = client fermer -1 = erreur
int	Server::receive(int sfd, std::string& text)
{
	char buffer[BUFFER_SIZE];
	
	while(true)
	{
		int n = recv(sfd, buffer, BUFFER_SIZE, 0);
		if (n > 0)
			text.append(buffer, n);
		else if (n == 0)
			return (0);
		else
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				return (1) ;
			std::cerr << "Error: recv failed" << std::endl;
			return (-1);
		}
	}
}

void Server::disconnectUser(int client_sfd)
{
	std::map<int, User *>::iterator uit = this->_users.find(client_sfd);
	if (uit == this->_users.end() || uit->second == NULL)
	{
		removePollfd(client_sfd);
		close(client_sfd);
		return ;
	}
	User* user = uit->second;
	for (std::map<std::string, Channel>::iterator chan_it = this->_channels.begin(); chan_it != this->_channels.end();)
	{
			Channel& channel = chan_it->second;
			if (channel.getUsers().find(client_sfd) != channel.getUsers().end())
				channel.removeUser(*user);
			if (channel.getNbUsers() == 0)
				this->_channels.erase(chan_it++);
			else
				++chan_it;
		
	}
	this->removeUser(client_sfd);
}

void	Server::handleNewEvents(void)
{
	int	nb_events = this->fetchNewEvents();
	if (nb_events == 0)
		return ;
	for (int i = 0; i <= this->_lastPollfd; i++)
	{
		int fd = this->_pollfds[i].fd;
		if (fd == -1)
			continue ;
		short rev = this->_pollfds[i].revents;
		if(rev == 0)
			continue ;
		// erreur et deco
		if(rev & (POLLERR | POLLHUP | POLLNVAL))
		{
			if(fd != this->_listenSfd)
				this->disconnectUser(fd);
			_pollfds[i].revents = 0;
			continue ;
		}
		//nouvelle connexion sur le socket listensfd
		if (fd == this->_listenSfd && (rev & POLLIN))
		{
			this->acceptNewConnections();
			_pollfds[i].revents = 0;
			continue ;
		}
		//clien fd avec message en attente
		if (rev & POLLIN)
		{
			std::string	text;
			int r = this->receive(fd, text);
			std::map<int, User *>::iterator it = this->_users.find(fd);
			if (it == this->_users.end() || it->second == NULL)
			{
				disconnectUser(fd);
				_pollfds[i].revents = 0;
				continue ;
			}
			if (r <= 0)
			{
				disconnectUser(fd);
				_pollfds[i].revents = 0;
				continue ;
			}
			#ifdef DEBUG
			if(!text.empty())
				std::cerr << "Received message from fd " << fd << ": " << text << std::endl;
			#endif

			User* user = it->second;
			t_msg msg;
			msg.sfd = fd;
			int parse_return = parsing(user->getParseBuffer(), text.c_str(), &msg);
			while (parse_return == 0)
			{
				#ifdef DEBUG
					std::cerr << "CMD: " << msg.command << " from " << msg.nickname << "\n";
				#endif
				utils::dispatchCommand(&msg, *this);
				parse_return = parsing(user->getParseBuffer(), "", &msg);
			}
		}
		_pollfds[i].revents = 0;
	}
	/*
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
	*/
}

void	Server::printPollfds(void) const
{}

std::ostream& operator<<(std::ostream& os, const Server& server)
{
    os << "SERVER host=" << server.getHostname()
       << " users=" << server.getUsers().size()
       << " channels=" << server.getChannels().size() << std::endl;
    return os;
}
