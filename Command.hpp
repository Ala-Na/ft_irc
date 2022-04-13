#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstring>
#include <algorithm>

// TODO modify following
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Numerics.hpp"

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
			void		intOper();
			void		intJoin();
			void		intTopic();
			void		intMode();
			void		intPart();
			void		intNames();
			void		intList();
			void		intInvite();
			void		intKick();
			void		intNotice();
			void		intKill();
			void		intQuit();
			void		intWallops();
			void		intPass();
			void		intSquit();
			void		intMotd();
			void		intError();
			void		intUser();
			void		intNick();
			void		intUserMode();
			void		intWhoIs();
			void		intUserhost();
			void		intAway();
			void		intPrivMsg();
			void		intChannelMode();
			
			int			isServerOperator(User & user);

	};
};

#endif