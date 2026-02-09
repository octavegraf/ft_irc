#pragma once

#include <string>
#include <vector>
#include <climits>

#include "message.hpp"

t_msg *parsing(char *raw_msg, t_msg *msg);
bool complete_message(std::string raw_msg);
void msg_prefix(t_msg *msg, std::string complete_msg);
void msg_params(t_msg *msg, std::string complete_msg);