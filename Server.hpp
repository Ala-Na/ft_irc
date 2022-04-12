#ifndef SERVER_HPP
#define SERVER_HPP

// Cpp library
#include <vector>
#include <iostream>
#include <cstring>
#include <ctime>

// C library with no cpp equivalent
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

#define SERV_BUF_SIZE 512 // Max size of IRC message (last 2 characters for carriage return - line feed)

namespace irc {

	extern bool	running;

	class	User;
	class	Channel;
	class	Command;

	class Server {
		private :
			std::vector<pollfd>			pfds;
			std::vector<User *>			users;
			std::vector<User *>			operators;
			std::vector<Channel *>		channels;
			std::vector<std::string>	datas;
			bool						running;

			std::string		password;
			const char*		port;
			int				server_socket;
			// TODO .conf file and recuperate each corresponding data
			std::string		name; // NOTE max 63 characters
			std::string		adminloc1; //RPL 257
			std::string 	adminloc2; //RPL 258
			std::string		adminemail; //RPL 259
			std::string		motd;
			std::string		version;


			Server ();
			Server (const Server& other);
			Server&	operator= (const Server& other);

		public :
			Server (std::string password, const char* port);
			~Server ();

			int			initServer ();
			int			runServer ();

			void		addSocketToPoll (int socket_fd);
			void		deleteSocketFromPoll (std::vector<pollfd>::iterator& to_del);

			void		createUser ();
			void		deleteUser (User* user);
			Channel*	createChannel (std::string name);
			void		receiveDatas ();
			void		datasExtraction (std::string& buf, int pos);

			void		checkPassword(User* user, std::string parameters);
			void		listChannels (User* user);
			void		getMotdFile(User* user, std::string parameters);
			void		retrieveTime (User* user, std::string parameters);
			void		retrieveVersion (User* user, std::string parameter);
			void		sendError (User* user, std::string parameter);
			void		sendPong (User* user, std::string parameter);
			void		getAdmin(User* user, std::string parameters);
			int			isServOp(User & user);

			Server&		getServer ();

			User*		getSpecificUser (unsigned long user_nb);
			Channel*	getChannelByName (std::string name);
			// User*		getUserByName (std::string name);

			User *		getUserByUsername(std::string name);
			User *		getUserByNick(std::string nick);
			std::string	getPass();
			std::string	*getVersionAddr();
			std::string	getMotd();
			std::vector<User *>	getServOp();
			std::vector<User *>	getServUsers();

			void	intWallops();

	};
};

#endif