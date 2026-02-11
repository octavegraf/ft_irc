#include "parsing.hpp"
#include <iostream>

int	parsing(const char *raw_msg, t_msg *msg)
{
	*msg = t_msg();
	static std::string buffer;
	#ifdef DEBUG
	std::cout << GREY << "Buffer: [" << buffer << "]" << RESET << std::endl;
	std::cout << GREY << "Raw message received: [" << raw_msg << "]" << RESET << std::endl;
	#endif
	buffer += raw_msg;
	if (!complete_message(raw_msg))
		return (1);
	else
	{
		size_t pos = buffer.find("\r\n"); // Search for the end of the message
		std::string complete_msg = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);
		msg_prefix(msg, complete_msg); // Extract prefix
		msg->command = complete_msg.substr(0, complete_msg.find(' ')); // Extract command
		complete_msg.erase(0, complete_msg.find(' ') + 1);
		msg_params(msg, complete_msg); // Extract parameters
	}
	return (0);
}

bool complete_message(std::string raw_msg)
{
	return (raw_msg.find("\r\n") != std::string::npos ? true : false);
}

void msg_prefix(t_msg *msg, std::string complete_msg)
{
	if (complete_msg[0] == ':') // Verify if there's a prefix
	{
		std::string prefix = complete_msg.substr(1, complete_msg.find(' ') - 1);
		complete_msg.erase(0, complete_msg.find(' ') + 1);
		if (prefix.find('@') != std::string::npos) // We start from the end
		{
			msg->hostname = prefix.substr(prefix.find('@') + 1); // get from '@'
			prefix.erase(prefix.find('@'));
		}
		if (prefix.find('!') != std::string::npos)
		{
			msg->username = prefix.substr(prefix.find('!') + 1); // get from '!'
			prefix.erase(prefix.find('!'));
		}
		msg->nickname = prefix;
	}
}

void msg_params(t_msg *msg, std::string complete_msg)
{
	while (true)
	{
		if (complete_msg[0] == ':') // Check for trailing parameter
		{
			complete_msg.erase(0, 1);
			msg->params.push_back(complete_msg);
			break ;
		}
		msg->params.push_back(complete_msg.substr(0, complete_msg.find(' ')));
		complete_msg.erase(0, complete_msg.find(' ') + 1);
		if (complete_msg.empty())
			break ;
	}
}
