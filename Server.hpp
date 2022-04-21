#ifndef SERVER_HPP
#define SERVER_HPP

// Cpp library
#include <vector>
#include <iostream>
#include <cstring>
#include <ctime>
#include <map>
#include <fstream>

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

			std::string					password;
			const char*					port;
			int							server_socket;

			std::map<std::string, std::string>	conf;


			Server ();
			Server (const Server& other);
			Server&	operator= (const Server& other);

		public :
			Server (std::string password, const char* port);
			~Server ();

			int							initServer ();
			int							readConfFile ();
			int							checkConf ();
			int							runServer ();

			void						addSocketToPoll (int socket_fd);

			void						createUser ();
			void						deleteUser (User* user);
			void						deleteUserFromChannels (User* user);
			Channel*					createChannel (std::string name);
			void						receiveDatas ();
			void						datasExtraction (std::string& buf, size_t pos);

			void						checkPassword (User* user, std::string parameters);
			void						checkNick (User* user, std::string parameters);
			void						checkUserCmd(User *user, std::string paramters);
			void						welcomeUser(User *user);

			void						listChannels (User* user);
			void						getMotd (User* user, std::string parameters);
			void						retrieveTime (User* user, std::string parameters);
			void						retrieveVersion (User* user, std::string parameter);
			void						sendError (User* user, std::string parameter);
			void						sendPong (User* user, std::string parameter);
			void						getAdmin (User* user, std::string parameters);
			int							isServOp (User & user);

			std::string					getName ();
			Server&						getServer ();
			std::string					getInfos();

			int							getMaxChannelbyUser() const;

			User*						getSpecificUser (size_t user_nb);
			Channel*					getChannelByName (std::string name);

			User *						getUserByUsername (std::string name);
			User *						getUserByNick (std::string nick);
			std::string					getPass ();
			std::string					getVersion ();
			std::vector<User *>			getServOp ();
			std::vector<User *>			getServUsers ();
			
			void						setServOperator (User* user);
			void						deleteServOperator (User* user);
			bool						isServOperator(User *user);

			void						sendWallops(std::string msg);

	};
};

#endif
