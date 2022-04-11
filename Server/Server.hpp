#ifndef SERVER_HPP
#define SERVER_HPP

// Cpp librairy
#include <vector>
#include <iostream>
#include <cstring>
#include <ctime>
#include <map>
#include <fstream>
#include <string>

// C librairy with no cpp equivalent
#include <poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h> //inet_ntoa
#include <netinet/in.h> //inet_ntoa
#include <fcntl.h>
#include <stdlib.h>

// TODO modify following path
#include "User.hpp"
#include "Channel.hpp"
#include "Command.hpp"

#define BUF_SIZE 512 // Max size of IRC message (last 2 characters for carriage return - line feed)

namespace irc {

	extern bool	running;

	class	User;
	class	Channel;
	class	Command;

	class Server {
		private :
			std::vector<pollfd>			pfds;
			std::vector<User *>			users;
			std::vector<Channel *>		channels;
			std::vector<std::string>	datas;

			std::string		password;
			const char*		port;
			int				server_socket;

			// TODO .conf file and recuperate each corresponding data
			std::map<std::string, std::string>	conf;

			Server ();
			Server (const Server& other);
			Server&	operator= (const Server& other);

		public :
			Server (std::string password, const char* port);
			~Server ();

			int	readConfFile ();
			int	initServer ();
			int	runServer ();

			void	addSocketToPoll (int socket_fd);
			void	deleteSocketFromPoll (std::vector<pollfd>::iterator& to_del);

			void		createUser ();
			Channel*	createChannel (std::string name);
			void		receiveDatas ();
			void		datasExtraction (std::string& buf, int pos);

			Server&		getServer ();
			User*		getSpecificUser (int user_nb);
			Channel*	getChannelByName (std::string name);
	};
};

#endif
