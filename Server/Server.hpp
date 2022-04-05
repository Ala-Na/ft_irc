#ifndef SERVER_HPP
#define SERVER_HPP

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

class	User;
class	Channel;

// TODO maybe put in canonical form ?
class Server {
	private :
		std::vector<pollfd>		pfds;
		std::vector<User *>		users;
		std::vector<Channel *>	channels; // NOTE must keep track of channels modes

		std::string				name; // NOTE max 63 characters
		std::string				password;
		const char*				port;

		int						server_socket;

		Server ();

	public :
		Server (std::string password, int port);
		~Server ();

		int	initServer ();





};

#endif
