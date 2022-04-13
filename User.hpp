/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cboutier <cboutier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 11:14:35 by cboutier          #+#    #+#             */
/*   Updated: 2022/04/11 14:54:25 by cboutier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <vector>
# include "Channel.hpp"
# include "Server.hpp"
# include <sys/socket.h>

namespace irc
{
	enum UserStatus
	{
		PASSWORD,
		REGISTERED,
		ONLINE,
		DELETED
	};

	class Channel;
	class Server;

	class User
	{
		private:
			Server *					_server;
			std::string					_nickname;
			std::string					_username; //max len 9
			std::string					_real_name;
			std::string					_hostname;
			std::vector<std::string>	_channels;
			std::vector<std::string>	_params;
			int							_fd;
			struct	sockaddr_in			_address;

			std::string					_away_message;
		/*
			The user MODE's are typically changes which affect either how the
			client is seen by others or what 'extra' messages the client is sent.

			A user MODE command MUST only be accepted if both the sender of the
			message and the nickname given as a parameter are both the same.
			If no other parameter is given, then the server will return the current
			settings for the nick.
		*/
		public: //? or private?
		struct user_Modes
		{
			/*
				https://datatracker.ietf.org/doc/html/rfc2812#section-3.1.5

				The flag 'a' SHALL NOT be toggled by the user using the MODE command,
				instead use of the AWAY command is REQUIRED.

				If a user attempts to make themselves an operator using the "+o" or
				"+O" flag, the attempt SHOULD be ignored as users could bypass the
				authentication mechanisms of the OPER command.  There is no
				restriction, however, on anyone `deopping' themselves (using "-o" or
				"-O").

				On the other hand, if a user attempts to make themselves unrestricted
				using the "-r" flag, the attempt SHOULD be ignored.  There is no
				restriction, however, on anyone `deopping' themselves (using "+r").
				This flag is typically set by the server upon connection for
				administrative reasons.  While the restrictions imposed are left up
				to the implementation, it is typical that a restricted user not be
				allowed to change nicknames, nor make use of the channel operator
				status on channels.

				The flag 's' is obsolete but MAY still be used.
			*/
			bool		a;		// user is flagged as away;
			bool		i;		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
			bool		w;		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
			bool		r;		// restricted user connection;
			bool		o;		// operator flag;
			bool		O;		// local operator flag;
			// bool s;			// marks a user for receipt of server notices.

		};


		struct user_Modes	userModes;

		// public:
			User();
			User(int fd, struct sockaddr_in address);
			~User();
			User(const User &src);
			User &operator=(const User &src);
			// Getters
			Server *	getServer();
			std::string getNickname();
			std::string getUsername();
			std::string getRealName();
			std::string getHostname();
			std::vector<std::string> getParams();
			std::string getAwayMessage();
			int			getFd();
			sockaddr_in	getAddr();
			std::vector<std::string> getChannels();

			int			getNbOfChannels();

			// flags bool
			bool	isAway();
			bool	isInvisible();
			bool	isWallops();
			bool	isRestricted();
			bool	isOperator();

			// Setters
			void	setNickname(std::string nickname);
			void	setUsername(std::string username);
			void	setRealName(std::string real_name);
			void	setHostname(std::string hostname);
			void	setAwayMessage(std::string msg);
			void	setParams(std::vector<std::string> params);

			void	sendMessage(int fd, std::string msg);

			void	addChannel(std::string const &chan);
			void	deleteChannel(std::string const &chan);

			// Commands
			void	user_cmd(std::string params);
			void	nick(std::string nickname);
			void	quit(std::vector<std::string> channels);
			void	privmsg(User usr, std::string msg);
			void	notice(std::string msg);
			void	wallops(std::string msg);
			void	away(std::string msg = "");
			void	quit(void);
			void	part();
			// void	part(std::vector<std::string> params);
			void	whois(User usr);
			void	kick(std::string const &chan);
			void	mode(std::vector<std::string> params);

			bool	operator==(User const &rhs) const;
			bool	operator!=(User const &rhs) const;

	};

	// template <class T>															// it == it2
	// bool	operator==(const T & lhs, const T & rhs)
	// {
	// 	if (lhs.getServer() == rhs.getServer() && lhs.getNickname() == rhs.getNickname()
	// 			&& lhs.getUsername() == rhs.getUsername() && lhs.getRealName() == rhs.getRealName()
	// 			&& lhs.getHostname() == rhs.getHostname() && lhs.getChannels() == rhs.getChannels()
	// 			&& lhs.getParams() == rhs.getParams() && lhs.getFd() == rhs.getFd()
	// 			&& lhs.getAddr() == rhs.getAddr() && lhs.getAwayMessage() == rhs.getAwayMessage())
	// 		return (1);
	// 	return (0);
	// };

	// // Second versions are used to compare an iterator (T1) with a const iterator (T2)
	// template <class T1, class T2>												// const it == it2 or it == const it2
	// bool	operator==(const T1 & lhs, const T2 & rhs)
	// {
	// 	if (lhs.getServer() == rhs.getServer() && lhs.getNickname() == rhs.getNickname()
	// 		&& lhs.getUsername() == rhs.getUsername() && lhs.getRealName() == rhs.getRealName()
	// 		&& lhs.getHostname() == rhs.getHostname() && lhs.getChannels() == rhs.getChannels()
	// 		&& lhs.getParams() == rhs.getParams() && lhs.getFd() == rhs.getFd()
	// 		&& lhs.getAddr() == rhs.getAddr() && lhs.getAwayMessage() == rhs.getAwayMessage())
	// 		return (1);
	// 	return (0);
	// };

	// template <class T>															// it != it2
	// bool	operator!=(const T & lhs, const T & rhs)
	// {
	// 	return (!(lhs == rhs));
	// };

	// template <class T1, class T2>												// const it != it2 or it != const it2
	// bool	operator!=(const T1 & lhs, const T2 & rhs)
	// {
	// 	return (!(lhs == rhs));
	// };
}

#endif