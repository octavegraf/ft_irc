#include "Server.hpp"
#include "commands.hpp"
#include "utils.hpp"
#include <iomanip>
#include "colors.hpp"

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

int Server::addUser(User *user)
{
	#ifdef DEBUG
		std::cerr << BLUE;
		std::cerr << "Adding user: " << user->getNickname() << std::endl;
		std::cerr << RESET;
		std::cerr << "==========" << std::endl;
	#endif
	_users.insert(std::pair<int, User *>(user->getSfd(), user));
	return (0);
}

int Server::removeUser(User *user)
{
	#ifdef DEBUG
		std::cerr << BLUE;
		std::cerr << "Removing user: " << user->getNickname() << std::endl;
		std::cerr << RESET;
		std::cerr << "==========" << std::endl;
	#endif
	_users.erase(user->getSfd());
	return (0);
	// @octavegraf @rchanrenous
}

void Server::disconnectUser(int client_sfd)
{
	// Find and remove the user from the map
	std::map<int, User *>::iterator it = _users.find(client_sfd);
	if (it != _users.end())
	{
		delete it->second;
		_users.erase(it);
	}
	
	// Close the socket
	close(client_sfd);
	
	// Mark the pollfd slot as unused
	for (int i = 0; i < CLIENT_LIMIT; i++)
	{
		if (_pollfds[i].fd == client_sfd)
		{
			_pollfds[i].fd = -1;
			_pollfds[i].events = 0;
			_pollfds[i].revents = 0;
			break;
		}
	}
	
	_nbUsers -= 1;
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
	
	// Add user to channel first
	channel->addUser(const_cast<User *>(&user));
	
	// Make first user (creator) an operator
	if (is_new_channel)
	{
		channel->addOP(user);
		#ifdef DEBUG
		std::cerr << BLUE;
		std::cerr << "User " << user.getNickname() << " is now operator of channel " << channelName << std::endl;
		std::cerr << RESET;
		std::cerr << "==========" << std::endl;
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

static void	fill_getaddrinfo_hints(struct addrinfo *hints, int ai_flags, int ai_family, int ai_socktype, int ai_protocol)
{
	hints->ai_flags = ai_flags;
	hints->ai_family = ai_family;
	hints->ai_socktype = ai_socktype;
	hints->ai_protocol = ai_protocol;
	hints->ai_addrlen = 0;
	hints->ai_addr = NULL;
	hints->ai_canonname = NULL;
	hints->ai_next = NULL;
}

static int getListenSfd(const char *port)
{
	struct addrinfo	hints;
	struct addrinfo	*res;
	struct addrinfo	*ptr;
	int				val;
	int				sfd;
	fill_getaddrinfo_hints(&hints, 0, AF_UNSPEC, SOCK_STREAM, 6); // remove TCP code hard-code
	val = getaddrinfo("localhost", port, &hints, &res);
	if (val != 0)
	{
		std::cerr << "getaddrinfo()" << std::endl;
		throw std::exception();
	}
	ptr = res;
	// attempt socket connection: socket() + bind()
	while (ptr != NULL)
	{
		//addr_print_info(ptr);
		sfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sfd == -1)
		{
			freeaddrinfo(res);
			std::cerr << "socket()" << std::endl;
			throw std::exception();
		}
		// Set socket to non-blocking mode using fcntl (macOS compatible)
		int flags = fcntl(sfd, F_GETFL, 0);
		if (flags == -1 || fcntl(sfd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			freeaddrinfo(res);
			close(sfd);
			std::cerr << "fcntl() - setting non-blocking" << std::endl;
			throw std::exception();
		}
		if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == 0) // check errno in case of fail: assignment in progress etc.
			break ;
		close(sfd);
		ptr = ptr->ai_next;
	}
	if (ptr == NULL)
	{
		freeaddrinfo(res);
		std::cerr << "Unable to bind to port." << std::endl;
		throw std::exception();
	}
	if (ptr->ai_protocol == 6 && listen(sfd, CLIENT_LIMIT) != 0) // remove TCP code hard-code
	{
		freeaddrinfo(res);
		std::cerr << "listen()" << std::endl;
		throw std::exception();
	}
	freeaddrinfo(res); // save ai_protocol info? (listen() call)
	return (sfd);
}

Server::Server(const char *port, const char *password) :
	_port(atoi(port)), _hostname("localhost"), _password(password), _listenSfd(getListenSfd(port)), _pollfds(), _nbUsers(0), _channels(), _users() 
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		this->_pollfds[i].fd = -1;
		this->_pollfds[i].events = 0;
		this->_pollfds[i].revents = 0;
	}
}

Server::Server(const char *port) :
	_port(atoi(port)), _hostname("localhost"), _password(""), _listenSfd(getListenSfd(port)), _pollfds(), _nbUsers(0), _channels(), _users() 
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		this->_pollfds[i].fd = -1;
		this->_pollfds[i].events = 0;
		this->_pollfds[i].revents = 0;
	}
}

Server::~Server(void)
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd != -1)
		{
#ifdef DEBUG
			std::cerr << BLUE;
			std::cerr << "close fd: " << this->_pollfds[i].fd << std::endl;
			std::cerr << RESET;
			std::cerr << "==========" << std::endl;
#endif
			close(this->_pollfds[i].fd);
		}
	}
	for (std::map<int, User *>::iterator it=this->_users.begin(); it != this->_users.end(); it++)	
		delete it->second;
	close(this->_listenSfd);

}

/*
int Server::command(t_msg *msg)
{
	return (0);
}*/

/*
int dispatchCommand(t_msg *msg, Server &server)
{
	// I'll do it again, im gonna use the "command.cpp now" @rchanrenous
	const std::string serverScope[] = {"CAP", "USER"};
	const std::string channelScope[] = {""};
	const std::string userScope[] = {"NICK"};

	for (size_t i = 0; i < sizeof(serverScope) / sizeof(std::string); ++i)
	{
		if (msg->command == serverScope[i])
			return (server.command(msg));
	}
	for (size_t i = 0; i < sizeof(channelScope) / sizeof(std::string); ++i)
	{
		if (msg->command == channelScope[i])
			return (server.getChannel(channelScope[i]).command(msg));
	}
	for (size_t i = 0; i < sizeof(userScope) / sizeof(std::string); ++i)
	{
		if (msg->command == userScope[i])
			return (server.getUser(userScope[i]).command(msg));
	}
	std::cerr << "Unknown command: " << msg->command << "." << std::endl;
	return (1);
}*/

void	Server::addPollfd(int client_sfd)
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd == -1)
		{
			this->_pollfds[i].fd = client_sfd;
			this->_pollfds[i].events = POLLIN;
			this->_pollfds[i].revents = 0;
			return ;
		}
	}
}

void	Server::acceptNewConnections(void)
{
	int	client_sfd;

	client_sfd = accept(this->_listenSfd, NULL, NULL); // need to retrieve the client ip address?
	if (client_sfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // no pending incoming connexion request
		return ;
	// iterate until no more pending incoming connexion request?
	else if (client_sfd == -1)
	{
		std::cerr << "accept()" << std::endl;
		throw std::exception();
	}
	if (this->_nbUsers >= CLIENT_LIMIT)
	{
		std::cerr << "Connexion rejected: maximum user limit reached" << std::endl; 
		// send reject message to client?
		close(client_sfd);
		return ;
	}
	this->_nbUsers += 1;
	//print_sockaddr(&addr, len);
	if (fcntl(client_sfd, F_SETFL, O_NONBLOCK) == -1) // useful?
	{
		std::cerr << "fcntl()" << std::endl;
		throw std::exception();
	}	
#ifdef DEBUG
	std::cerr << RED;
	std::cerr << "Now listening on sfd: " << client_sfd << std::endl;
	std::cerr << RESET;
#endif
	// instantiate new User and add User to server's Users list
	this->_users[client_sfd] = new User(client_sfd);
#ifdef DEBUG
	std::cerr << BLUE;
	std::cerr << "Adding new user to Server: " << std::endl << *(this->_users[client_sfd]);
	std::cerr << RESET;
	std::cerr << "==========" << std::endl;
#endif
	// add new user's fd to server's list of pollfds
	this->addPollfd(client_sfd);
}

int	Server::fetchNewEvents(void)
{
	int	res_poll = poll(this->_pollfds, CLIENT_LIMIT, 10);
	if (res_poll == -1)
	{
		if (errno == EINTR)
			return (0);  // Signal interrupted poll, continue the loop
		std::cerr << "poll()" << std::endl;
		throw std::exception();
	}
	return (res_poll);
}

void	Server::handleNewEvents(void)
{
	int	nb_events = this->fetchNewEvents();
	int	handled = 0;
	char	buffer[BUFFER_SIZE];
	for (int i = 0; i < CLIENT_LIMIT && handled < nb_events; i++)
	{
		// pending incoming message
		if (this->_pollfds[i].fd != -1 && (this->_pollfds[i].revents & POLLIN) == POLLIN)
		{
			// get bytes
			std::string	text("");
			int	nbytes = recv(this->_pollfds[i].fd, buffer, BUFFER_SIZE, 0);
			if (nbytes == 0)
			{
				// Client disconnected
#ifdef DEBUG
				std::cerr << "Client disconnected (socket " << this->_pollfds[i].fd << ")" << std::endl;
#endif
				this->disconnectUser(this->_pollfds[i].fd);
				handled += 1;
				continue;
			}
			while (nbytes > 0)
			{
				text.append(buffer, nbytes);
				nbytes = recv(this->_pollfds[i].fd, buffer, BUFFER_SIZE, 0);
			}
			if (nbytes == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
			{
				throw std::exception();
			}
#ifdef DEBUG
			std::cerr << RED;
			std::cerr << "Received:" << std::endl;
			std::cerr << GREEN;
			std::cerr << "text: " << text << std::endl;
			std::cerr << "from user: " << std::endl << *(this->_users[this->_pollfds[i].fd]);
			std::cerr << RESET;
#endif
			// get msg
			t_msg msg;
			msg.sfd = _pollfds[i].fd;
			
			// Get user's parse buffer
			User* user = this->_users[_pollfds[i].fd];
			if (!user)
				continue;
			
			int parse_return = parsing(user->getParseBuffer(), text.c_str(), &msg);

			while (parse_return == 0)
			{
				#ifdef DEBUG
				std::cerr << GREEN;
				std::cerr << ">>>>>>>" << msg << std::endl;
				#endif
				// exec message
				utils::dispatchCommand(&msg, *this);
				handled += 1;
				parse_return = parsing(user->getParseBuffer(), "", &msg);
			}

			#ifdef DEBUG
			std::cerr << RESET;
			std::cerr << "==========" << std::endl;
			#endif
		}
	}
}

void	Server::printPollfds(void) const
{
	for (int i=0; i<CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd != -1)
		{
			std::cerr << "\t*fd:" << this->_pollfds[i].fd << std::endl;
			std::cerr << "\tevents: " << this->_pollfds[i].events << std::endl;
			std::cerr << "\trevents: " << this->_pollfds[i].revents << std::endl;
		}
	}
}

std::ostream&	operator<<(std::ostream& os, const Server& server)
{
	os << "=============" << std::endl;
	os << "SERVER:" << std::endl;
	os << "\tport:" << server._port << std::endl;
	os << "\thostname:" << server._hostname << std::endl;
	os << "\tpassword:" << server._password << std::endl;
	os << "\tlistenSfd:" << server._listenSfd << std::endl;
	os << "\tnbUsers:" << server._nbUsers << std::endl;
	os << "\tpollfds:" << std::endl;
	server.printPollfds();
	os << "\tusers:" << std::endl << server._users << std::endl;
	os << "\tchannels:" << std::endl << server._channels << std::endl;
	os << "=============" << std::endl;
	return (os);
}
