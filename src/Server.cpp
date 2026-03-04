#include "Server.hpp"
#include "commands.hpp"
#include "utils.hpp"
#include <iomanip>
#include "colors.hpp"

Server::Server(const char *port, const char *password) :
	_port(atoi(port)), _hostname("localhost"), _password(password), _listenSfd(getListenSfd(port)), _pollfds(), _lastPollfd(-1), _nbUsers(0), _channels(), _users()
{
	initPollfds();
}

Server::Server(const char *port) :
	_port(atoi(port)), _hostname("localhost"), _password(""), _listenSfd(getListenSfd(port)), _pollfds(), _lastPollfd(-1), _nbUsers(0), _channels(), _users()
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
#ifdef DEBUG
	std::cerr << BLUE;
	std::cerr << "Adding new user to Server: " << std::endl << *(this->_users[sfd]);
	std::cerr << RESET;
	std::cerr << "==========" << std::endl;
#endif

}


void	Server::updateLastPollfd(void)
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		if (this->_pollfds[i].fd != -1)
			this->_lastPollfd = i;
	}
}

void	Server::removePollfd(int sfd)
{
	for (int i=0; i <= this->_lastPollfd; i++)
	{
		if (this->_pollfds[i].fd == sfd)
		{
			this->_pollfds[i].fd = -1;
			this->_pollfds[i].events = 0;
			this->_pollfds[i].revents = 0;
			if (i == this->_lastPollfd)
				this->updateLastPollfd();
			return ;
		}
	}
}

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

void	Server::initPollfds(void)
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		this->_pollfds[i].fd = -1;
		this->_pollfds[i].events = 0;
		this->_pollfds[i].revents = 0;
	}
}

void	Server::getProtocolConnexionInfo(struct addrinfo **info, const char *port, int ai_flags, int ai_family, int ai_socktype, const char *protocol)
{
	struct protoent	*proto = getprotobyname(protocol);
	if (proto == NULL)
	{
		std::cerr << "No such available protocol: " << protocol << std::endl;
		throw std::exception();
	}
	std::cout << "protocol ID: " << (int)(proto->p_proto) << std::endl;
	struct addrinfo	hints;
	fill_getaddrinfo_hints(&hints, ai_flags, ai_family, ai_socktype, proto->p_proto);
	if (getaddrinfo("localhost", port, &hints, info) != 0)
	{
		std::cerr << "getaddrinfo()" << std::endl;
		throw std::exception();
	}
}

int	Server::bindPort(struct addrinfo *info)
{
	struct addrinfo	*ptr = info;
	int	sfd;
	while (ptr != NULL)
	{
		//addr_print_info(ptr);
		sfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sfd == -1)
		{
			freeaddrinfo(info);
			std::cerr << "socket()" << std::endl;
			throw std::exception();
		}
		// Set socket to non-blocking mode using fcntl (macOS compatible)
		int flags = fcntl(sfd, F_GETFL, 0);
		if (flags == -1 || fcntl(sfd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			freeaddrinfo(info);
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
		freeaddrinfo(info);
		std::cerr << "Unable to bind to port." << std::endl;
		throw std::exception();
	}
	return (sfd);
}

int Server::getListenSfd(const char *port)
{
	struct addrinfo	*info = NULL;
	Server::getProtocolConnexionInfo(&info, port, 0, AF_UNSPEC, SOCK_STREAM, "TCP");

	// attempt socket connection: socket() + bind()
	int	sfd = Server::bindPort(info);
	freeaddrinfo(info);
	//if (ptr->ai_protocol == 6 && listen(sfd, CLIENT_LIMIT) != 0) // remove TCP code hard-code
	if (listen(sfd, CLIENT_LIMIT) != 0)
	{
		close(sfd);
		std::cerr << "listen()" << std::endl;
		throw std::exception();
	}
	// save ai_protocol info? (listen() call)
	return (sfd);
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
			if (i > this->_lastPollfd)
				this->_lastPollfd = i;
			return ;
		}
	}
}

void	Server::acceptNewConnections(void)
{
	int client_sfd = accept(this->_listenSfd, NULL, NULL); // need to retrieve the client ip address?
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
	// add User to server's Users list
	this->addUser(client_sfd);
	// add new user's fd to server's list of pollfds
	this->addPollfd(client_sfd);
}

int	Server::fetchNewEvents(void)
{
	int	res_poll = poll(this->_pollfds, this->_lastPollfd + 1, 0);
	if (res_poll == -1)
	{
		std::cerr << "poll()" << std::endl;
		throw std::exception();
	}
	return (res_poll);
}

void	Server::receive(int sfd, std::string& text)
{
	char	buffer[BUFFER_SIZE];
	int	nbytes = recv(sfd, buffer, BUFFER_SIZE, 0);
	while (nbytes > 0)
	{
		text.append(buffer, nbytes);
		nbytes = recv(sfd, buffer, BUFFER_SIZE, 0);
	}
	if (nbytes == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
	{
		std::cerr << "recv()" << std::endl;
		throw std::exception();
	}
}

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
			while (parsing(text.c_str(), &msg) == 0)
			{
#ifdef DEBUG
				std::cerr << GREEN;
				std::cerr << ">>>>>>>" << msg << std::endl;
#endif
				// exec message
				utils::dispatchCommand(&msg, *this);
				handled += 1;
				text = "";
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
	for (int i=0; i <= this->_lastPollfd; i++)
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
