#include "main.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 2 && argc != 3)
	{
		std::cout << ERR_INVALID_ARGC << std::endl;
		return (1);
	}
	std::cout << argv[1];
	for (int i = 0; !argv[argc - 1][i]; i++)
	{
		if (!isdigit(i))
		{
			std::cout << ERR_NOT_DIGIT << std::endl;
			return (1);
		}
	}
	if (atoi(argv[1]) < 1 || atoi(argv[1]) > 65535)
	{
		std::cout << ERR_INVALID_PORT << std::endl;
		return (1);
	}
	std::string hostname = "host";
	unsigned int listenSfd = 0; // @rchanrenous need to change this
	if (argc == 2)
		Server server(argv[1], listenSfd, hostname);
	if (argc == 3)
		Server server(argv[1], listenSfd, hostname, argv[2]);
	return (0);
}