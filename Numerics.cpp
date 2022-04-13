#include "Numerics.hpp"
#include <cstdio>

using namespace irc;

#define END "\r\n"

void	irc::sendNumeric(int fd, std::string msg) {
	int bytes_send = send(fd, msg.c_str(), msg.size(), 0);
	if ((size_t)bytes_send < msg.size()) {
		std::cerr << "Error while sending message to fd = " << fd << std::endl;
		// TODO close connection
	}
}

/* COMMAND RESPONSES */

/* 001 */ std::string	irc::RplWelcome (std::string nick, std::string user, std::string host) { return (" :Welcome to the Internet Relay Network, " + nick + "!" + user + "@" + host); }
/* 002 */ std::string	irc::RplYourHost (std::string server, std::string version) { return (" :Your host is " + server + ", running version " + version);}
/* 003 */ std::string	irc::RplCreated (std::string creation) { return (" :This server was created " + creation); }
/* 004 */ std::string	irc::RplMyInfo (std::string server, std::string version, std::string u_modes, std::string c_modes) { return (" " + server + " " + version + " " + u_modes + " " + c_modes); }
// 005 - No need
// 200 to 212 - No need
// 219 - No need
/* 221 */ std::string	irc::RplUModeIs (std::string modes) { return (" " + modes); }
// 234 - No need
// 235 - No need
// 242 - No need
// 243 - No need
// 251 to 255 - No need
/* 256 */ std::string	irc::RplAdminMe (std::string server) { return (" " + server + " :Administrative info"); }
/* 257 */ std::string	irc::RplAdminLocOne (std::string adminloc1) { return (" :" + adminloc1); }
/* 258 */ std::string	irc::RplAdminLocTwo (std::string adminloc2) { return (" :" + adminloc2); }
/* 259 */ std::string	irc::RplAdminEmail (std::string adminemail) { return (" :" + adminemail); }
// 261 to 263 - No need
/* 301 */ std::string	irc::RplAway (std::string dest_nick, std::string away_msg) { return (" " + dest_nick + " :" + away_msg); }
/* 302 */ std::string	irc::RplUserhost (std::string replies) { return (" :" + replies); }
/* 303 */ std::string	irc::RplIson (std::string nicks_presents) { return (" :" + nicks_presents); }
/* 305 */ std::string	irc::RplUnaway () { return (" :You are no longer marked as being away"); }
/* 306 */ std::string	irc::RplNowAway () { return (" : You have been marked as being away"); }
/* 311 */ std::string	irc::RplWhoIsUser (std::string w_nick, std::string w_user, std::string w_host, std::string w_real) { return (" " + w_nick + " " + w_user + " " + w_host + " * :" + w_real); }
/* 312 */ std::string	irc::RplWhoIsServer (std::string server, std::string server_nick, std::string infos) { return (" " + server_nick + " " + server + " :" + infos); };
/* 313 */ std::string	irc::RplWhoIsOperator (std::string o_nick) { return (" " + o_nick + " :is an IRC operator"); }
/* 315 */ std::string	irc::RplEndOfWho (std::string name_searched) { return (" " + name_searched + " :End of WHO list"); }
// 317 - No need
/* 318 */ std::string	irc::RplEndOfWhoIs () { return (" :End of WHOIS list"); }
/* 319 */ std::string	irc::RplWhoIsChannels (std::string w_nick, std::string w_list_channels_and_op) { return (" " + w_nick + " :" + w_list_channels_and_op); } 
/* 322 */ std::string	irc::RplList (std::string channel, std::string visible, std::string topic) { return (" " + channel + " " + visible + " :" + topic); }
/* 323 */ std::string	irc::RplListEnd () { return (" :End of LIST"); }
/* 324 */ std::string	irc::RplChannelModeIs (std::string channel, std::string mode, std::string mode_params) { return (" " + channel + " " + mode + " " + mode_params); }
/* 331 */ std::string	irc::RplNoTopic (std::string channel) { return (" " + channel + " :No topic is set"); }	
/* 332 */ std::string	irc::RplTopic (std::string channel, std::string topic) { return (" " + channel + " :" + topic); }
/* 341 */ std::string	irc::RplInviting (std::string nick, std::string channel) { return (" " + channel + " " + nick); }
// 342 - No need
/* 346 */ std::string	irc::RplInviteList (std::string channel, std::string invitemask) { return (" " + channel + " " + invitemask); }
/* 347 */ std::string	irc::RplEndOfInviteList (std::string channel) { return (" " + channel + " :End of channel invite list"); }
/* 348 */ std::string	irc::RplExceptList (std::string channel, std::string exceptionmask) { return (" " + channel + " " + exceptionmask); }
/* 349 */ std::string	irc::RplEndOfExceptList (std::string channel) { return (" " + channel + " :End of channel invite list"); }
/* 351 */ std::string	irc::RplVersion (std::string server, std::string version, std::string debug, std::string comments) { return (" " + version + "." + debug + " " + server + " :" + comments); }
/* 352 */ std::string	irc::RplWhoReply (std::string server, std::string nick, std::string channel, std::string user, std::string host, std::string more, std::string hopcount, std::string real_name) { return (" " + channel + " " + user + " " + host + " " + server + " " + nick + " " + more + " :" + hopcount + " " +  real_name); }
// Explanation for 352 : http://chi.cs.uchicago.edu/chirc/assignment3.html
/* 353 */ std::string	irc::RplNamReply (std::string type_chan, std::string channel, std::string nicks) { return (" " + type_chan + " " + channel + " :" + nicks); }
// nicks must contains each nicks of the users presents in the channel with either nothing/@/+ as prefix (@ = operator of channel, +=authorization to speak in moderated channel)
// 364 - No need
// 365 - No need
/* 366 */ std::string	irc::RplEndOfNames (std::string channel) { return (" " + channel + " :End of NAMES list"); }
/* 367 */ std::string	irc::RplBanList (std::string channel, std::string mask) { return (" " + channel + " " + mask); }
/* 368 */ std::string	irc::RplEndOfBanList (std::string channel) { return (" " + channel + " :End of channel ban list"); }
/* 371 */ std::string	irc::RplInfo (std::string infos) { return (" :" + infos); }
/* 372 */ std::string	irc::RplMotd (std::string motd_line) { return (":- " + motd_line); }
// Note : Max 80 char for each motd line
/* 374 */ std::string	irc::RplEndOfInfo () { return (" :End of INFO list"); }
/* 375 */ std::string	irc::RplMotdStart (std::string server) { return (" :- " + server + "Message of the day -"); }
/* 376 */ std::string	irc::RplEndOfMotd () { return (" :End of MOTD command"); }
/* 381 */ std::string	irc::RplYourOper () { return ( " :You are now an IRC operator"); }
// 382 - No need
// 383 - No need
/* 391 */ std::string	irc::RplTime (std::string server, std::string time) { return (" " + server + " :" + time); }
// 392 to 395 - No need

/* ERROR REPLIES */
/* 401 */ std::string	irc::ErrNoSuchNick (std::string nick) { return (" " + nick + " :No suck nick/channel"); }
/* 402 */ std::string	irc::ErrNoSuchServer (std::string server_name) { return (" " + server_name + " :No such server"); }
/* 403 */ std::string	irc::ErrNoSuchChannel (std::string channel_name) { return (" " + channel_name + " :No such channel"); }
/* 404 */ std::string	irc::ErrCannotSendToChan (std::string channel_name) { return (" " + channel_name + " :No such channel"); }
/* 405 */ std::string	irc::ErrTooManyChannels (std::string channel_name) { return (" " + channel_name + " :You have joined too many channels"); }
// 406 - No need
// 407 - NEED MORE INFOS
// 408 - No nedd
/* 409 */ std::string	irc::ErrNoOrigin () { return (" :No origin specified"); }
/* 411 */ std::string	irc::ErrNoRecipient (std::string command) { return (" : No recicipent given (" + command + ")"); }
/* 412 */ std::string	irc::ErrNoTextToSend () { return (" : NO text to send"); }
/* 413 */ std::string	irc::ErrNoTopLevel (std::string mask) { return (" " + mask + " :No toplevel domain specified"); }
/* 414 */ std::string	irc::ErrWildTopLevel (std::string mask) { return (" " + mask + " :Wildcard in toplevel domain"); }
/* 415 */ std::string	irc::ErrBadMask (std::string mask) { return (" " + mask + " :Bad Server/host mask"); }
/* 421 */ std::string	irc::ErrUnknownCommand (std::string command) { return (" " +  command + "  :Unknown command"); }
/* 422 */ std::string	irc::ErrNoMotd () { return (" : MOTD File is missing"); }
/* 423 */ std::string	irc::ErrNoAdminInfo (std::string server) { return (" " + server + " : No administrative info available"); }
/* 424 */ std::string	irc::ErrFileError (std::string file_op, std::string file) { return (" : File error doing " + file_op + " on " + file); }
/* 431 */ std::string	irc::ErrNoNicknameGiven () { return ( " :No nickname given"); }
/* 432 */ std::string	irc::ErrErroneusNickname (std::string nick) { return (" " + nick + " :Erroneous nickname"); }
/* 433 */ std::string	irc::ErrNicknameInUse (std::string nick) { return (" " + nick + " :Nickname is already in use"); }
// 436 - No need
// 437 - No need
/* 441 */ std::string	irc::ErrUserNotInChannel (std::string nick, std::string channel) { return (" " + nick + " " + channel +" : They aren't on that channel"); }
/* 442 */ std::string	irc::ErrNotOnChannel (std::string channel) { return (" " + channel + " :You're not on that channel"); }
/* 443 */ std::string	irc::ErrUserOnChannel (std::string user, std::string channel) { return (" " + user + " " + channel +" :is already on channel"); }
/* 444 */ std::string	irc::ErrNoLogin (std::string user) { return (" " + user + " :Not logged in"); }
/* 445 */ std::string	irc::ErrSummonDisabled () { return (" :SUMMON has been disabled"); }
/* 446 */ std::string	irc::ErrUsersDisabled () { return (" :USERS has be disabled"); }
/* 451 */ std::string	irc::ErrNotRegistered () { return (" :You have not registered"); }
/* 461 */ std::string	irc::ErrNeedMoreParams (std::string command) { return (" " + command + " :Not enough parameters"); }
/* 462 */ std::string	irc::ErrAlreadyRegistered () { return (" :Unauthorized command (already registered)"); }
// 463 - No need
/* 464 */ std::string	irc::ErrPasswordMismatch () { return (" : Password incorrect"); }
// 465 - No need
// 466 - No need
/* 467 */ std::string	irc::ErrKeySet (std::string channel) { return (" " + channel + " :Channel key already set"); }
/* 471 */ std::string	irc::ErrChannelIsFull (std::string channel) { return (" " + channel + " :Cannot join channel (+l)"); }
/* 472 */ std::string	irc::ErrUnknownMode (std::string letter, std::string channel) { return (" " + letter + " :is unknown mode char to me for " + channel); }
/* 473 */ std::string	irc::ErrInviteOnlyChan (std::string channel) { return (" " + channel + " :Cannot join channel (+i)"); }
/* 474 */ std::string	irc::ErrBannedFromChan (std::string channel) { return (" " + channel + " :Cannot join channel (+b)"); }
/* 475 */ std::string	irc::ErrBadChannelKey (std::string channel) { return (" " + channel + " :Cannot join channel (+k)"); }
/* 476 */ std::string	irc::ErrBadChanMask (std::string channel) { return (" " + channel + " :Bad Channel Mask"); }
/* 477 */ std::string	irc::ErrNoChanModes (std::string channel) { return (" " + channel + " :Channel doesn't support mode"); }
/* 478 */ std::string	irc::ErrBanListFull (std::string channel, std::string letter) { return (" " + channel + " " + letter + " :Channel list is full"); }
/* 481 */ std::string	irc::ErrNoPrivileges () { return (" : Permission Denied- You're not an IRC operator"); }
/* 482 */ std::string	irc::ErrChanNoPrivsNeeded (std::string channel) { return (" " + channel + " :You're not channel operator"); }
/* 483 */ std::string	irc::ErrCantKillServer () { return (" :You can't kill a server"); }
/* 484 */ std::string	irc::ErrRestricted () { return (" :Your connection is restricted!"); }
// 485 - No need
/* 491 */ std::string	irc::ErrNoOperHost () { return (" :No O-lines for your host"); }
/* 501 */ std::string	irc::ErrUModeUnknownFlag () { return (" :Unknown MODE flag"); }
/* 502 */ std::string	irc::ErrUsersDontMatch () { return (" :Cannot change mode for tohers users"); }


void	irc::numericReply(int num_nb, User* user, std::vector<std::string>& s_params) {
	std::string					msg;
	char		 				s_num_nb[4];
	// TODO maybe modify following functions calls
	int							fd = user->getFd();
	std::string					nick = user->getNickname();
	std::string					server = user->getServer()->getName();

	if (nick.empty()) {
		nick += "*";
	}

	sprintf(s_num_nb, "%.3d", num_nb);
	msg = ":" + server + " " + s_num_nb + " " + nick;

	switch (num_nb) {
		case 1:
			msg += irc::RplWelcome(nick, s_params[0], s_params[1]);
			break;
		case 2:
			msg += irc::RplYourHost(server, s_params[0]);
			break;
		case 3:
			msg += irc::RplCreated(s_params[0]);
			break;
		case 4:
			msg += irc::RplMyInfo(server, s_params[0], s_params[1], s_params[2]);
			break;
		case 221:
			msg += irc::RplUModeIs(s_params[0]);
			break;
		case 256:
			msg += irc::RplAdminMe(server);
			break;
		case 257:
			msg += irc::RplAdminLocOne(s_params[0]);
			break;
		case 258:
			msg += irc::RplAdminLocTwo(s_params[0]);
			break;
		case 259:
			msg += irc::RplAdminEmail(s_params[0]);
			break;
		case 301:
			msg += irc::RplAway(s_params[0], s_params[1]);
			break;
		case 302:
			msg += irc::RplUserhost(s_params[0]);
			break;
		case 305:
			msg += irc::RplUnaway();
			break;
		case 306:
			msg += irc::RplNowAway();
			break;
		case 311:
			msg += irc::RplWhoIsUser(s_params[0], s_params[1], s_params[2], s_params[3]);
			break;
		case 312:
			msg += irc::RplWhoIsServer(server, s_params[0], s_params[1]);
			break;
		case 313:
			msg += irc::RplWhoIsOperator(s_params[0]);
			break;
		case 315:
			msg += irc::RplEndOfWho(s_params[0]);
			break;
		case 318:
			msg += irc::RplEndOfWhoIs();
			break;
		case 322:
			msg += irc::RplList(s_params[0], s_params[1], s_params[2]);
			break;
		case 323:
			msg += irc::RplListEnd();
			break;
		case 324:
			msg += irc::RplChannelModeIs(s_params[0], s_params[1], s_params[2]);
			break;
		case 331:
			msg += irc::RplNoTopic(s_params[0]);
			break;
		case 332:
			msg += irc::RplTopic(s_params[0], s_params[1]);
			break;
		case 341:
			msg += irc::RplInviting(nick, s_params[0]);
			break;
		case 346:
			msg += irc::RplInviteList(s_params[0], s_params[1]);
			break;
		case 347:
			msg += irc::RplEndOfInviteList(s_params[0]);
			break;
		case 348:
			msg += irc::RplExceptList(s_params[0], s_params[1]);
			break;
		case 349:
			msg += irc::RplEndOfExceptList(s_params[0]);
			break;
		case 351:
			msg += irc::RplVersion(server, s_params[0], s_params[1], s_params[2]);
			break;
		case 352:
			msg += irc::RplWhoReply(server, nick, s_params[0], s_params[1], s_params[2], s_params[3], s_params[4], s_params[5]);
			break;
		case 353:
			msg += irc::RplNamReply(s_params[0], s_params[1], s_params[2]);
			break;
		case 366:
			msg += irc::RplEndOfNames(s_params[0]);
			break;
		case 367:
			msg += irc::RplBanList(s_params[0], s_params[1]);
			break;
		case 368:
			msg += irc::RplEndOfBanList(s_params[0]);
			break;
		case 371:
			msg += irc::RplInfo(s_params[0]);
			break;
		case 372:
			msg += irc::RplMotd(s_params[0]);
			break;
		case 374:
			msg += irc::RplEndOfInfo();
			break;
		case 375:
			msg += irc::RplMotdStart(server);
			break;
		case 376:
			msg += irc::RplEndOfMotd();
			break;
		case 381:
			msg += irc::RplYourOper();
			break;
		case 391:
			msg += irc::RplTime(server, s_params[0]);
			break;
		case 401:
			msg += irc::ErrNoSuchNick(s_params[0]);
			break;
		case 402:
			msg += irc::ErrNoSuchServer(s_params[0]);
			break;
		case 403:
			msg += irc::ErrNoSuchChannel(s_params[0]);
			break;
		case 404:
			msg += irc::ErrCannotSendToChan(s_params[0]);
			break;
		case 405:
			msg += irc::ErrTooManyChannels(s_params[0]);
			break;
		case 409:
			msg += irc::ErrNoOrigin();
			break;
		case 411:
			msg += irc::ErrNoRecipient(s_params[0]);
			break;
		case 412:
			msg += irc::ErrNoTextToSend();
			break;
		case 413:
			msg += irc::ErrNoTopLevel(s_params[0]);
			break;
		case 414:
			msg += irc::ErrWildTopLevel(s_params[0]);
			break;
		case 415:
			msg += irc::ErrBadMask(s_params[0]);
			break;
		case 421:
			msg += irc::ErrUnknownCommand(s_params[0]);
			break;
		case 422:
			msg += irc::ErrNoMotd();
			break;
		case 423:
			msg += irc::ErrNoAdminInfo(server);
			break;
		case 424:
			msg += irc::ErrFileError(s_params[0], s_params[1]);
			break;
		case 431:
			msg += irc::ErrNoNicknameGiven();
			break;
		case 432:
			msg += irc::ErrErroneusNickname(s_params[0]);
			break;
		case 433:
			msg += irc::ErrNicknameInUse(s_params[0]);
			break;
		case 441:
			msg += irc::ErrUserNotInChannel(nick, s_params[0]);
			break;
		case 442:
			msg += irc::ErrNotOnChannel(s_params[0]);
			break;	
		case 443:
			msg += irc::ErrUserOnChannel(s_params[0], s_params[1]);
			break;
		case 444:
			msg += irc::ErrNoLogin(s_params[0]);
			break;
		case 445:
			msg += irc::ErrSummonDisabled();
			break;
		case 446:
			msg += irc::ErrUsersDisabled();
			break;
		case 451:
			msg += irc::ErrNotRegistered();
			break;
		case 461:
			msg += irc::ErrNeedMoreParams(s_params[0]);
			break;
		case 462:
			msg += irc::ErrAlreadyRegistered();
			break;
		case 464:
			msg += irc::ErrPasswordMismatch();
			break;
		case 467:
			msg += irc::ErrKeySet(s_params[0]);
			break;
		case 471:
			msg += irc::ErrChannelIsFull(s_params[0]);
			break;
		case 472:
			msg += irc::ErrUnknownMode(s_params[0], s_params[1]);
			break;
		case 473:
			msg += irc::ErrInviteOnlyChan(s_params[0]);
			break;
		case 474:
			msg += irc::ErrBannedFromChan(s_params[0]);
			break;
		case 475:
			msg += irc::ErrBadChannelKey(s_params[0]);
			break;
		case 476:
			msg += irc::ErrBadChanMask(s_params[0]);
			break;
		case 477:
			msg += irc::ErrNoChanModes(s_params[0]);
			break;
		case 478:
			msg += irc::ErrBanListFull(s_params[0], s_params[1]);
			break;
		case 481:
			msg += irc::ErrNoPrivileges();
			break;
		case 482:
			msg += irc::ErrChanNoPrivsNeeded(s_params[0]);
			break;
		case 483:
			msg += irc::ErrCantKillServer();
			break;
		case 484:
			msg += irc::ErrRestricted();
			break;
		case 491:
			msg += irc::ErrNoOperHost();
			break;
		case 501:
			msg += irc::ErrUModeUnknownFlag();
			break;
		case 502:
			msg += irc::ErrUsersDontMatch();
			break;
		default :
			msg += irc::ErrUnknownCommand(s_num_nb);
			break;
	}
	msg += END;
	irc::sendNumeric(fd, msg);
}
