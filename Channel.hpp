#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <cstring>
# include <cctype>
# include <cstdlib>
# include <sstream>
# include <vector>
# include <cstdio>
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
			std::vector<User *>	vec_chan_users;
			unsigned long		max_nb_users_in_chan;
			std::vector<User *>	vec_chan_operators;		// @
			std::vector<User *>	vec_chan_banned_users;
			std::vector<User *> vec_chan_invited_users;
		
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
			std::string			getChanModesParams(); 
			std::vector<User *>	getVecChanUsers();
			int					getNbUsersInChan();
			int					getMaxNbUsersInChan();
			std::vector<User *> getChanOperators();
			std::vector<User *>	getVecChanBannedUsers();
			std::vector<User *>	getVecChanInvitedUsers();
			std::string			getChanNameAndTopic();
			User*				getUserFromNickname(std::string nickname);

			void				setChanPassword(std::string password);
			int					isOperator(User* user);
			void				setChanTopic(std::string topic, User* user_who_changes);
			void				setChanMode(std::string mode);
			void				setVecChanUsers(std::vector<User *> vec_users);
			void				setMaxNbUsersInChan(int nb);
			void				setChanOperators(std::vector<User *> vec_operators);
			void				setVecChanBannedUsers(std::vector<User *> vec_banned_users);
			void				setVecChanInvitedUsers(std::vector<User *> vec_invited_users);

			bool						userIsInChanFromNickname(std::string nickname_to_search);
			bool						userIsInChan(User* user);
			bool						userIsBannedFromChan(User* user);
			bool						userIsInvitedInChan(User* user);
			bool						addMode(User* ope, std::string mode);
			bool						deleteMode(User* ope, std::string mode);
			int							receivingAnInvitation(User* user_inviting, User* user_invited);
			std::vector<std::string>	namesInfos();
			int							listAllUsersInChan(User* user_asking);
			int							writeToAllChanUsers(std::string sentence_to_send, User* to_avoid);
			int							addUser(User* user_to_add);
			int							deleteChanUser(User* user_to_delete, std::string message, bool kick = false);
			void						addOperator(User* operator_adding, std::string nick_target);
			void						deleteOperator(User* operator_deleting, std::string nick_target);
			int							addBannedUser(User* user_banning, User* user_to_ban);
			int							deleteBannedUser(User* user_unbanning, User* user_to_unban);
			void						listBannedUsers(User* asking);
			int							addInvitedUser(User* user_inviting, User* user_to_invite);
			int							deleteInvitedUser(User* user_uninviting, User* user_to_uninvite);
			void						listInvitedUsers(User* asking);
			void						addKey(User* ope, std::string key);
			void						deleteKey(User* ope);
			void						addLimit(User* ope, std::string nb);
			void						deleteLimit(User* ope);
			void						setModes(User* user, std::string modes);
	};
}

#endif
