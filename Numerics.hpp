#ifndef NUMERICS_HPP
#define NUMERICS_HPP

#include <cstring>
#include <cstdarg>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Server.hpp"
#include "User.hpp"

namespace irc {
	class Server;
	class User;

	int	numericReply (int num_nb, User* user, std::vector<std::string>& s_params);
	int	sendNumeric(int fd, std::string msg);

	std::string	RplWelcome (std::string nick, std::string user, std::string host);
	std::string	RplYourHost (std::string server, std::string version);
	std::string	RplCreated (std::string creation);
	std::string	RplMyInfo (std::string server, std::string version, std::string u_modes, std::string c_modes);
	std::string	RplUModeIs (std::string modes);
	std::string	RplAdminMe (std::string server);
	std::string	RplAdminLocOne (std::string adminloc1);
	std::string	RplAdminLocTwo (std::string adminloc2);
	std::string	RplAdminEmail (std::string adminemail);
	std::string	RplAway (std::string dest_nick, std::string away_msg);
	std::string	RplUserhost (std::string replies);
	std::string	RplIson (std::string nicks_presents);
	std::string	RplUnaway ();
	std::string	RplNowAway ();
	std::string	RplWhoIsUser (std::string w_nick, std::string w_user, std::string w_host, std::string w_real);
	std::string	RplWhoIsServer (std::string server, std::string server_nick, std::string infos);
	std::string	RplWhoIsOperator (std::string o_nick);
	std::string	RplEndOfWhoIs ();
	std::string	RplWhoIsChannels (std::string w_nick, std::string w_list_channels_and_op);
	std::string	RplList (std::string channel, std::string visible, std::string topic);
	std::string	RplListEnd ();
	std::string	RplChannelModeIs (std::string channel, std::string mode, std::string mode_params);
	std::string	RplNoTopic (std::string channel);
	std::string	RplTopic (std::string channel, std::string topic);
	std::string	RplInviting (std::string nick, std::string channel);
	std::string	RplInviteList (std::string channel, std::string invitemask);
	std::string	RplEndOfInviteList (std::string channel);
	std::string	RplExceptList (std::string channel, std::string exceptionmask);
	std::string	RplEndOfExceptList (std::string channel);
	std::string	RplVersion (std::string server, std::string version, std::string debug, std::string comments);
	std::string	RplNamReply (std::string type_chan, std::string channel, std::string nicks);
	std::string	RplEndOfNames (std::string channel);
	std::string	RplBanList (std::string channel, std::string mask);
	std::string	RplEndOfBanList (std::string channel);
	std::string	RplInfo (std::string infos);
	std::string	RplMotd (std::string motd_line);
	std::string	RplEndOfInfo ();
	std::string	RplMotdStart (std::string server);
	std::string	RplEndOfMotd ();
	std::string	RplYourOper ();
	std::string	RplTime (std::string server, std::string time);
	std::string	ErrNoSuchNick (std::string nick);
	std::string	ErrNoSuchServer (std::string server_name);
	std::string	ErrNoSuchChannel (std::string channel_name);
	std::string	ErrCannotSendToChan (std::string channel_name);
	std::string	ErrTooManyChannels (std::string channel_name);
	std::string	ErrNoOrigin ();
	std::string	ErrNoRecipient (std::string command);
	std::string	ErrNoTextToSend ();
	std::string	ErrNoTopLevel (std::string mask);
	std::string	ErrWildTopLevel (std::string mask);
	std::string	ErrBadMask (std::string mask);
	std::string	ErrUnknownCommand (std::string command);
	std::string	ErrNoMotd ();
	std::string	ErrNoAdminInfo (std::string server);
	std::string	ErrFileError (std::string file_op, std::string file);
	std::string	ErrNoNicknameGiven ();
	std::string	ErrErroneusNickname (std::string nick);
	std::string	ErrNicknameInUse (std::string nick);
	std::string	ErrUserNotInChannel (std::string nick, std::string channel);
	std::string	ErrNotOnChannel (std::string channel);
	std::string	ErrUserOnChannel (std::string user, std::string channel);
	std::string	ErrNoLogin (std::string user);
	std::string	ErrSummonDisabled ();
	std::string	ErrUsersDisabled ();
	std::string	ErrNotRegistered ();
	std::string	ErrNeedMoreParams (std::string command);
	std::string	ErrAlreadyRegistered ();
	std::string	ErrPasswordMismatch ();
	std::string	ErrKeySet (std::string channel);
	std::string	ErrChannelIsFull (std::string channel);
	std::string	ErrUnknownMode (std::string letter, std::string channel);
	std::string	ErrInviteOnlyChan (std::string channel);
	std::string	ErrBannedFromChan (std::string channel);
	std::string	ErrBadChannelKey (std::string channel);
	std::string	ErrBadChanMask (std::string channel);
	std::string	ErrNoChanModes (std::string channel);
	std::string	ErrBanListFull (std::string channel, std::string letter);
	std::string	ErrNoPrivileges ();
	std::string	ErrChanNoPrivsNeeded (std::string channel);
	std::string	ErrCantKillServer ();
	std::string	ErrRestricted ();
	std::string	ErrNoOperHost ();
	std::string	ErrUModeUnknownFlag ();
	std::string	ErrUsersDontMatch ();
};

#endif
