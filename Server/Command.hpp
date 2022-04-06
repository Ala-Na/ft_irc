#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstring>

// TODO modify following
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

namespace irc {
	class Server;

	class	Command {
		private :
			Server&				server;
			User*				user;
			std::string	content;

			std::string prefix;


			Command ();

		public :
			Command (Server& server, User* user, std::string& content);
			~Command ();

			void	parseCommand();

	};
];

#endif
