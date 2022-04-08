#ifndef NUMERICS_HPP
#define NUMERICS_HPP

#include <cstring>
#include <cstdarg>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

namespace irc {
	// No special need of class ?
	void	numericReply (int numeric_nb, int fd, std::string server, \
		std::string nick, int param_nb, ...);

};

#endif
