#include "parsing.hpp"
#include <iostream>

t_msg *parsing(char *raw_msg, t_msg *msg)
{
	msg->command.clear(); msg->params.clear(); msg->prefix.clear(); // Reset msg structure content
	if (!raw_msg || !msg)
		return (NULL);
	static std::string buffer;
	#ifdef DEBUG
	std::cout << "Buffer: [" << buffer << "]" << std::endl;
	std::cout << "Raw message received: [" << raw_msg << "]" << std::endl;
	#endif
	buffer += raw_msg;
	if (buffer.find("\r\n") == std::string::npos) // The message is not complete
	{
		#ifdef DEBUG
		std::cout << "Incomplete message, waiting for more data..." << std::endl;
		#endif
		return (NULL);
	}
	else
	{
		size_t pos = buffer.find("\r\n"); // Search for the end of the message
		std::string complete_msg = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);
		if (complete_msg[0] == ':') // Verify if there's a prefix
		{
			msg->prefix = complete_msg.substr(1, complete_msg.find(' ') - 1);
			complete_msg.erase(0, complete_msg.find(' ') + 1);
		}
		msg->command = complete_msg.substr(0, complete_msg.find(' '));
		complete_msg.erase(0, complete_msg.find(' ') + 1);
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
	return (msg);
}

// Message structure : https://datatracker.ietf.org/doc/html/rfc1459#section-2.3.1

// int main(void)
// {
// 	t_msg msg;
// 	char raw_msg[] = ":@rchanernou PRIVMSG #channel Hello, World! eeeeeuuh\r\n";
// 	t_msg *parsed_msg = parsing(raw_msg, &msg);
// 	if (parsed_msg)
// 	{
// 		// Successfully parsed the message
// 		std::cout << "Prefix: " << parsed_msg->prefix << std::endl;
// 		std::cout << "Command: " << parsed_msg->command << std::endl;
// 		std::cout << "Parameters:" << std::endl;
// 		for (size_t i = 0; i < parsed_msg->params.size(); ++i)
// 		{
// 			std::cout << "  Param " << i + 1 << ": " << parsed_msg->params[i] << std::endl;
// 		}
// 	}
// 	return 0;
// }
