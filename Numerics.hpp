#ifndef NUMERICS_HPP
#define NUMERICS_HPP

#include <cstring>
#include <cstdarg>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
// TODO include Server.hpp and User.hpp

namespace irc {
	class Server;
	class User;

	void	numericReply (int num_nb, User* user, std::vector<std::string> s_params);

};

#endif