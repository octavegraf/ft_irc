#include "Server.hpp"

Server::Server(char const *port, int const listenSfd, std::string const hostname, std::string const password) : _port(atoi(port)), _listenSfd(listenSfd), _password(password)
{}

Server::Server(char const *port, int const listenSfd, std::string const hostname) : _port(atoi(port)), _listenSfd(listenSfd), _password(NULL)
{}

Server::~Server()
{}

std::string const Server::getHostname() { return (_hostname);}

std::string const Server::getPassword() { return (_password); }

std::vector<Channel *> Server::getChannels() { return (_channels); }

std::vector<User *> Server::getUsers() { return (_users);}

int Server::respond(User *user, std::string message)
{
	#ifdef DEBUG
		std::cout << user->getNickname() << "->" << message << std::endl;
	#endif
	return (send(user->getFd(), message.c_str(), message.length(), 0));
}

int Server::respond(std::string nickname, std::string message)
{
	User *user = searchUser(nickname, _users);
	if (!user)
		return (-1);
	return (respond(user, message));
}

int Server::command(t_msg *msg)
{
	return (0);
}
