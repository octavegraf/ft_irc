#pragma once

#include "errors.hpp"
#include "message.hpp"
#include "Server.hpp"
#include "User.hpp"

// Server Scope
void cap(t_msg *msg, Server &server);
void pass(t_msg *msg, Server &server);
void user(t_msg *msg, Server &server);
void privmsg(t_msg *msg, Server &server);
void pingpong(t_msg *msg, Server &server);

// Channel Scope
void join(t_msg *msg, Server &server);
void part(t_msg *msg, Server &server);
void kick(t_msg *msg, Server &server);
void invite(t_msg *msg, Server &server);

// User Scope
void nick(t_msg *msg, Server &server);
