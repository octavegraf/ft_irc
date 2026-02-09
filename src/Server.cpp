#include "Server.hpp"

Server::Server(char const *port, int const listenSfd, std::string const hostname, std::string const password) : _port(atoi(port)), _listenSfd(listenSfd), _password(password)
{}

Server::Server(char const *port, int const listenSfd, std::string const hostname) : _port(atoi(port)), _listenSfd(listenSfd), _password(NULL)
{}

Server::~Server()
{}

std::string const Server::getPassword() {return (_password);}

std::vector<Channel &> Server::getChannels() { return (_channels); }

std::vector<User &> Server::getUsers() {return (_users);}

int Server::command(t_msg *msg)
{
	return (0);
}

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
}
