
#include <signal.h>
#include <exception>
#include <iostream>

#include "errors.hpp"
#include "Server.hpp"

bool	interrupt(false);
int		setup_signal_action(struct sigaction *sigaction);

/*	for (int i = 0; !argv[argc - 1][i]; i++)
	{
		if (!isdigit(i))
		{
			std::cerr << ERR_NOT_DIGIT << std::endl;
			return (1);
		}
	}
	if (atoi(argv[1]) < 1 || atoi(argv[1]) > 65535)
	{
		std::cerr << ERR_INVALID_PORT << std::endl;
		return (1);
	}*/


int	main(int argc, char **argv)
{
	if (argc != 2 && argc != 3)
	{
		std::cerr << ERR_INVALID_ARGC << std::endl;
		return (1);
	}

	// check inputs

	// init server: fetch socket type + create a socket + bind the socket to the host and port
	try
	{
		if (argc == 3)
			Server server(argv[1], argv[2]);
		else
			Server server(argv[1]);

	}
	catch (std::exception e)
	{
		std::cerr << "Failed to initialize the server." << std::endl;
		//std::cerr << e.what() << std::endl;
		return (1);
	}
	// set up signal action
	struct sigaction	sigaction;
	setup_signal_action(&sigaction);

	// listen routine
	while (interrupt == false)
	{
		// accept connection requests: create new user + update server user list + store user fd in pollfd tab
		//server.acceptNewConnections();

		// poll the fds to fetch new events
		//server.fetchNewEvents();

		// iterate on the fds to find events to handle with dispatchCommand()
		//server.handleNewEvents();
	}
}
