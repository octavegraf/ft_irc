#include "commands.hpp"

int cap(t_msg *msg)
{
	if (msg->params[0] == "LS" && msg->params.size() == 1)
		return (0);
	std::cerr << ERR_INVALID_PARAMS << std::endl;
	return (1);
}

int pass(t_msg *msg, Server &server)
{
	if (msg->params[0] == server.getPassword()) // @octavegraf need to finish
}

int user(t_msg *msg, Server &server)
{}


/*	./a.out localhost 6666
	ai_flags: 0
	ai_family: 2
	ai_socktype: 1
	ai_protocol: 6
	ai_addrlen: 16
	ai_addr->sa_data: 26.10.127.0.0.1.0.0.0.0.0.0.0.0.
	ai_canonname: (null)
	sfd: 3, read_sfd: 4
	CAP LS
	PASS motdepasse
	NICK ocgraf
	USER ocgraf ocgraf localhost :Octave Graf
*/