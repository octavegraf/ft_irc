#pragma once

#include <string>
#include <vector>
#include <climits>

typedef struct	s_msg
{
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
}	t_msg;
