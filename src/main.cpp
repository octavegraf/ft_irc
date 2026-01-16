#include "main.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << ERR_INVALID_ARGC << std::endl;
		return (1);
	}
	if (atoi(argv[1]) < 1 || atoi(argv[1]) > 65535)
	{
		std::cout << ERR_INVALID_PORT << std::endl;
		return (1);
	}
	return (0);
}