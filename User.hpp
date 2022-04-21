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
			std::vector<Channel *>		_channels;
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

			User();
			User(const User &src);
			User &operator=(const User &src);

		public:
			User(int fd, std::string& hostname, struct sockaddr_in& address, Server *server);
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
			std::vector<Channel *>		getChannels();
			int							getNbOfChannels();
			UserStatus					getStatus();

			bool						get_a();
			bool						get_i();
			bool						get_w();
			bool						get_r();
			bool						get_o();

			std::string					getModesString();

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

			void		addChannel(Channel* chan);
			void		deleteChannel(Channel* chan);

			// Commands
			void		userCmd(std::vector<std::string>& params);
			void		nick(std::string nickname, bool send_msg);
			void		quit(std::vector<std::string> channels);
			void		privmsgToUser(User* dest, std::string msg);
			void		privmsgToChannel(Channel* channel, std::string msg);
			void		noticeToUser(User* dest, std::string msg);
			void		noticeToChannel(Channel* channel, std::string msg);
			void		notice(std::string msg);
			void		receiveWallops(std::string msg);
			int			away(std::string msg = "");
			void		quit(std::string reason);
			void		partChannel(Channel* channel, std::string reason);
			// void		part(std::vector<std::string> params);
			int			whois(User* who);
			void		kick(Channel* chan, std::string reason);
			void		mode(User* ope, std::string params);
			void		sendMode(User* ope, std::string mode_msg);

			bool		operator==(User const &rhs) const;
			bool		operator!=(User const &rhs) const;

	};
}

#endif
