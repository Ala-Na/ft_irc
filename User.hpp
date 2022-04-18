/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anadege <anadege@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 11:14:35 by cboutier          #+#    #+#             */
/*   Updated: 2022/04/19 17:15:40 by anadege          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <vector>
# include "Channel.hpp"
# include "Server.hpp"
# include "Utilitary.hpp"
# include <sys/socket.h>

namespace irc
{
	enum UserStatus
	{
		PASS,
		NICK,
		USER,
		REGISTERED
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

			UserStatus					_status;

			bool						userModes_a;		// user is flagged as away;
			bool						userModes_i;		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
			bool						userModes_w;		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
			bool						userModes_r;		// restricted user connection;
			bool						userModes_o;		// operator flag;
			bool						userModes_O;		// local operator flag;


		// public: //? or private?
			/*
				The user MODE's are typically changes which affect either how the
				client is seen by others or what 'extra' messages the client is sent.

				A user MODE command MUST only be accepted if both the sender of the
				message and the nickname given as a parameter are both the same.
				If no other parameter is given, then the server will return the current
				settings for the nick.
			*/
			// struct user_Modes
			// {
			// 	/*
			// 		https://datatracker.ietf.org/doc/html/rfc2812#section-3.1.5

			// 		The flag 'a' SHALL NOT be toggled by the user using the MODE command,
			// 		instead use of the AWAY command is REQUIRED.

			// 		If a user attempts to make themselves an operator using the "+o" or
			// 		"+O" flag, the attempt SHOULD be ignored as users could bypass the
			// 		authentication mechanisms of the OPER command.  There is no
			// 		restriction, however, on anyone `deopping' themselves (using "-o" or
			// 		"-O").

			// 		On the other hand, if a user attempts to make themselves unrestricted
			// 		using the "-r" flag, the attempt SHOULD be ignored.  There is no
			// 		restriction, however, on anyone `deopping' themselves (using "+r").
			// 		This flag is typically set by the server upon connection for
			// 		administrative reasons.  While the restrictions imposed are left up
			// 		to the implementation, it is typical that a restricted user not be
			// 		allowed to change nicknames, nor make use of the channel operator
			// 		status on channels.

			// 		The flag 's' is obsolete but MAY still be used.
			// 	*/
			// 	private:

			// 	bool		a;		// user is flagged as away;
			// 	bool		i;		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
			// 	bool		w;		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
			// 	bool		r;		// restricted user connection;
			// 	bool		o;		// operator flag;
			// 	bool		O;		// local operator flag;
			// 	// bool s;			// marks a user for receipt of server notices.

			// 	public:
			// 	// UserModes Getters
			// 	bool	get_a();
			// 	bool	get_i();
			// 	bool	get_w();
			// 	bool	get_r();
			// 	bool	get_o();
			// 	bool	get_O();

			// 	// UserModes Setters
			// 	void	set_a(bool val);
			// 	void	set_i(bool val);
			// 	void	set_w(bool val);
			// 	void	set_r(bool val);
			// 	void	set_o(bool val);
			// 	void	set_O(bool val);
			// };

			// struct user_Modes	userModes;

		public:
			User();
			User(int fd, struct sockaddr_in address);
			User(int fd, struct sockaddr_in address, Server *server);
			User(const User &src);
			User &operator=(const User &src);
			~User();

			// Getters
			Server *					getServer();
			std::string					getNickname();
			std::string					getUsername();
			std::string					getRealName();
			std::string					getHostname();
			std::vector<std::string>	getParams();
			std::string					getAwayMessage();
			int							getFd();
			sockaddr_in					getAddr();
			std::vector<std::string>	getChannels();
			int							getNbOfChannels();
			UserStatus					getStatus();

			bool						get_a();
			bool						get_i();
			bool						get_w();
			bool						get_r();
			bool						get_o();
			bool						get_O();

				// UserModes Setters

			// flags bool
			// bool		isAway();
			// bool		isInvisible();
			// bool		isWallops();
			// bool		isRestricted();
			// bool		isOperator();
			bool		isRegistered();

			// Setters
			void		setNickname(std::string nickname);
			void		setUsername(std::string username);
			void		setRealName(std::string real_name);
			void		setHostname(std::string hostname);
			void		setAwayMessage(std::string msg);
			void		setParams(std::vector<std::string> params);
			void		setStatus(UserStatus status);

			void		set_i(bool val);
			void		set_a(bool val);
			void		set_w(bool val);
			void		set_r(bool val);
			void		set_o(bool val);
			void		set_O(bool val);

			void		addChannel(std::string const &chan);
			void		deleteChannel(std::string const &chan);

			// Commands
			void		userCmd(std::vector<std::string>& params);
			void		nick(std::string nickname);
			void		quit(std::vector<std::string> channels);
			void		privmsg(User * usr, std::string msg);
			void		notice(std::string msg);
			void		wallops(std::string msg);
			int			away(std::string msg = "");
			void		quit(void);
			void		part();
			// void		part(std::vector<std::string> params);
			void		whois(User usr);
			void		kick(std::string const &chan);
			void		mode(std::vector<std::string> params);

			bool		operator==(User const &rhs) const;
			bool		operator!=(User const &rhs) const;

	};
}

#endif
