#pragma once

#include "iostream"
#include <vector>

typedef struct	s_msg
{
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string command;
	std::vector<std::string> params;

	s_msg() : nickname(NULL), username(NULL), hostname(NULL), command(NULL), params(NULL) {}
}	t_msg;

// Message structure : https://datatracker.ietf.org/doc/html/rfc1459#section-2.3.1