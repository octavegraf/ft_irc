#include "Server.hpp"

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
		sfd = socket(ptr->ai_family, ptr->ai_socktype | SOCK_NONBLOCK, ptr->ai_protocol);
		if (sfd == -1)
		{
			freeaddrinfo(res);
			std::cerr << "socket()" << std::endl;
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
	_port(atoi(port)), _password(password), _listenSfd(getListenSfd(port)), _pollfds(), _channels(), _users() 
{
	for (int i=0; i < CLIENT_LIMIT; i++)
	{
		this->_pollfds[i].fd = -1;
		this->_pollfds[i].events = 0;
		this->_pollfds[i].revents = 0;
	}

}

Server::Server(const char *port) :
	_port(atoi(port)), _password(""), _listenSfd(getListenSfd(port)), _pollfds(), _channels(), _users() 
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
}

const std::string&	Server::getPassword(void)
{
	return (_password);
}

const std::map<std::string, Channel>&	Server::getChannels(void)
{
	return (_channels);
}

const std::map<int, User *>&	Server::getUsers(void)
{
	return (_users);
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

/*
void	Server::acceptNewConnections(void)
{
	int	client_sfd;

	if (ptr->ai_protocol == 6) // remove TCP code hard-code
	{
		client_sfd = accept(sfd, NULL, NULL); // need to retrieve the client ip address?
		if (client_sfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // no pending incoming connexion request
			return ;
		// iterate until no more pending incoming connexion request?
//		print_sockaddr(&addr, len);
		if (fcntl(client_sfd, F_SETFL, O_NONBLOCK) == -1) // useful?
		{
			std::cerr << "fcntl()" << std::endl;
			throw std::exception();
		}	
	}
	// instantiate new User
	User	client(client_sfd);

	// add user to server's users list
	// add new user's fd to server's list of pollfds
	sfds[n].fd = client_sfd;
//		sfds[n].events = POLLIN | POLLPRI | POLLOUT;
	sfds[n].events = POLLIN | POLLOUT;
	sfds[n].revents = 0;
	n++;

}*/

void	Server::fetchNewEvents(void)
{

}

void	Server::handleNewEvents(void)
{

}


