#ifndef USER_HPP

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <vector>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>

// TODO modify following path
#include "User.hpp"
#include "Channel.hpp"

// TODO maybe put in canonical form ?
class Server {
	private :
		std::vector<User *>		users;
		std::vector<Channel *>	channels; // NOTE must keep track of channels modes

		std::string				name = "In Real unControl - An ft_irc server"; // NOTE max 63 characters
		std::string				password;
		const char				*port;
		bool					running = true;

	public :
		Server (); // TODO maybe set in private ?
		Server (std::string password, int port);
		~Server ();


};

#endif
