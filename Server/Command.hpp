#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstring>
#include <algorithm>

// TODO modify following
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

namespace irc {
	class Server;

	class	Command {
		private :
			Server&		server;
			User*		user;
			std::string	content;

			std::string	prefix;
			std::string	parameters;


			Command ();
			Command (const Command& other);
			Command&	operator= (const Command& other);

		public :
			Command (Server& server, User* user, std::string& content);
			~Command ();

			void		parseCommand();
			std::string	getWord();
			void		goToExecution();

	};
};

#endif
