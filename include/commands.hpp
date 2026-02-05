#pragma once

#include <vector>

#include "errors.hpp"
#include "message.hpp"
#include "Server.hpp"

// Server Scope
int cap(t_msg *msg);
int pass(t_msg *msg, const Server &server);
int user(t_msg *msg, const Server &server);

// Channel Scope

// User Scope
int nick(t_msg *msg, const Server &server);