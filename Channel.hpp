#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <cstring>
# include <cctype>
# include <cstdlib>
# include <sstream>
# include <vector>
# include "Server.hpp"
# include "User.hpp"
# include "Utilitary.hpp"
# include <sys/types.h>
# include <sys/socket.h>

namespace irc
{
	class Server;
	class User;

	class Channel
	{
		private :

			Server				*server;
			std::string			chan_name;
			std::string			chan_password;
			std::string			chan_topic;
			std::string			chan_mode;
			std::vector<User>	vec_chan_users;
			unsigned long		max_nb_users_in_chan;
			std::vector<User>	vec_chan_operators;		// @
			std::string			chan_creator;
			std::vector<User>	vec_chan_banned_users;
		
			Channel();
			Channel(Channel const & other);
			Channel & operator=(Channel const & other);

		public :

			Channel(Server *server_ptr, std::string name);
			~Channel();

			std::string 		getChanName(); 
			std::string 		getChanPassword();
			std::string 		getChanTopic();
			std::string 		getChanMode();
			std::vector<User>	getVecChanUsers();
			int					getNbUsersInChan();
			int					getMaxNbUsersInChan();
			std::vector<User> 	getChanOperators();
			std::string 		getChanCreator();
			std::vector<User>	getVecChanBannedUsers();
			std::string			getChanNameAndTopic();
			User *				getUserFromUsername(std::string username);

			void				setChanPassword(std::string password);
			int					isOperator(User* user);
			void				setChanTopic(std::string topic, User* user_who_changes);
			void				setChanMode(std::string mode);
			void				setVecChanUsers(std::vector<User> vec_users);
			void				setMaxNbUsersInChan(int nb);
			void				setChanOperators(std::vector<User> vec_operators);
			void				setChanCreator(std::string creator);
			void				setVecChanBannedUsers(std::vector<User> vec_banned_users);

			int					userIsInChanFromUsername(std::string username_to_search);
			int					userIsInChanFromNickname(std::string nickname_to_search);
			int					userIsBannedFromChan(std::string username_to_search);
			void				addMode(std::string mode);
			int					receivingAnInvitation(User* user_inviting, User* user_invited);
			int					listAllUsersInChan(User* user_asking);
			int					writeToAllChanUsers(std::string sentence_to_send);
			int					addUser(User & user_to_add);
			int					deleteUser(User & user_to_delete, std::string message);
			int					addOperator(User & operator_to_add);
			int					deleteOperator(User & operator_to_delete);
			int					addBannedUser(User & user_to_ban);
			int					deleteBannedUser(User & user_to_unban);
	};
}

#endif
