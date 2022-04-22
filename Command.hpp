#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstring>
#include <algorithm>
#include <vector>
#include <stdlib.h>

// TODO modify following
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Numerics.hpp"
#include "Utilitary.hpp"

namespace irc {

	class Server;
	class User;
	class Channel;

	class	Command {
		private :
			Server&		server;
			User*		user;
			std::string	content;

			std::string	prefix;
			std::string	param;


			Command ();
			Command (const Command& other);
			Command& operator= (const Command& other);

		public :
			Command (Server& server, User* user, std::string& content);
			Command (Server& server, std::string& content);
			~Command ();

			std::string	getWord();
			std::string	getParam();
			User		*getUser();

			void		parseCommand();
			void		goToExecution();

			// intermediate commands
			void		intPass();
			void		intNick();
			void		intUser();
			void		intOper();
			void		intJoin();
			void		intTopic();
			void		intMode();
			void		intPart();
			void		intNames();
			void		intList();
			void		intInvite();
			void		intKick();
			void		intPrivMsg();
			void		intNotice();
			void		intKill();
			void		intQuit();
			void		intWhoIs();
			void		intAway();
			void		intWallops();
			void		intUserhost();
			void		intSquit();
			void		intMotd();
			void		intSummon();
			void		intUsers();
			void		intPing();
	};
};

#endif
