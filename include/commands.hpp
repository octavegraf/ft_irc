#pragma once

#include <vector>

#include "errors.hpp"
#include "message.hpp"
#include "Server.hpp"
#include "User.hpp"

class Server;

// Utils
User * searchUser(std::string nickname, std::vector<User *> users);
int dispatchCommand(t_msg *msg, Server &server);

// Server Scope
int cap(t_msg *msg, Server &server);
int pass(t_msg *msg, Server &server);
int user(t_msg *msg, Server &server, int sfd);
int privmsg(t_msg *msg, Server &server);
int pingpong(t_msg *msg, Server &server);

// Channel Scope
int mode(t_msg *msg, Server &server);

// User Scope
int nick(t_msg *msg, Server &server);