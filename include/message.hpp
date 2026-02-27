#pragma once

#include <vector>
#include "iostream"
#include <vector>

typedef struct	s_msg
{
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string command;
	std::vector<std::string> params;
	int sfd;
	s_msg() : nickname(""), username(""), hostname(""), command(""), params(), sfd(0) {}
} t_msg;

inline std::ostream& operator<<(std::ostream& os, const t_msg& msg)
{
	os << "\tnickame: " << msg.nickname << std::endl;
	os << "\tusername: " << msg.username << std::endl;
	os << "\thostname: " << msg.hostname << std::endl;
	os << "\tcommand: " << msg.command << std::endl;
	os << "\tparams:";
	for (size_t i = 0; i < msg.params.size(); ++i)
		os << " " << msg.params[i];
	return (os);
}

// Message structure : https://datatracker.ietf.org/doc/html/rfc1459#section-2.3.1
