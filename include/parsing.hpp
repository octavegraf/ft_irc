#pragma once

#include <string>
#include <climits>

#include "colors.hpp"
#include "message.hpp"

int	parsing(const char *raw_msg, t_msg *msg);
bool complete_message(std::string raw_msg);
void msg_prefix(t_msg *msg, std::string complete_msg);
void msg_params(t_msg *msg, std::string complete_msg);
