#include "Numerics.hpp"
#include <cstdio>

using namespace irc;

#define END "\r\n"

void	sendNumeric(int fd, std::string msg) {
	int bytes_send = send(fd, msg.c_str(), msg.size(), 0);
	if (bytes_send < msg.size()) {
		std::cerr << "Error while sending message to fd = " << fd << std::endl;
		// TODO close connection
	}
}

/* COMMAND RESPONSES */

/* 001 */ std::string	RplWelcome (std::string nick, std::string user, std::string host) { return (" :Welcome to the Internet Relay Network, " + nick + "!" + user + "@" + host); }
/* 002 */ std::string	RplYourHost (std::string server, std::string version) { return (" :Your host is " + server + ", running version " + version);}
/* 003 */ std::string	RplCreated (std::string creation) { return (" :This server was created " + creation); }
/* 004 */ std::string	RplMyInfo (std::string server, std::string version, std::string u_modes, std::string c_modes) { return (" " + server + " " + version + " " + u_modes + " " + c_modes); }
// 005 - No need
// 200 to 212 - No need
// 219 - No need
/* 221 */ std::string	RplUModeIs (std::string modes) { return (" " + modes); }
// 234 - No need
// 235 - No need
// 242 - No need
// 243 - No need
// 251 to 255 - No need
/* 256 */ std::string	RplAdminMe (std::string server) { return (" " + server + " :Administrative info"); }
/* 257 */ std::string	RplAdminLocOne (std::string adminloc1) { return (" :" + adminloc1); }
/* 258 */ std::string	RplAdminLocTwo (std::string adminloc2) { return (" :" + adminloc2); }
/* 259 */ std::string	RplAdminEmail (std::string adminemail) { return (" :" + adminemail); }
// 261 to 263 - No need
/* 301 */ std::string	RplAway (std::string dest_nick, std::string away_msg) { return (" " + dest_nick + " :" + away_msg); }
/* 302 */ std::string	RplUserhost (std::string replies) { return (" :" + replies); }
/* 303 */ std::string	RplIson (std::string nicks_presents) { return (" :" + nicks_presents); }
/* 305 */ std::string	RplUnaway () { return (" :You are no longer marked as being away"); }
/* 306 */ std::string	RplNowAway () { return (" : You have been marked as being away"); }
/* 311 */ std::string	RplWhoIsUser (std::string w_nick, std::string w_user, std::string w_host, std::string w_real) { return (" " + w_nick + " " + w_user + " " + w_host + " * :" + w_real); }
/* 312 */ std::string	RplWhoIsServer (std::string server, std::string server_nick, std::string infos) { return (" " + server_nick + " " + server + " :" + infos); };
/* 313 */ std::string	RplWhoIsOperator (std::string o_nick) { return (" " + o_nick + " :is an IRC operator"); }
/* 315 */ std::string	RplEndOfWho (std::string name_searched) { return (" " + name_searched + " :End of WHO list"); }
// 317 - No need
/* 318 */ std::string	RplEndOfWhoIs () { return (" :End of WHOIS list"); }
/* 319 */ std::string	RplWhoIsChannels (std::string w_nick, std::string w_list_channels_and_op) { return (" " + w_nick + " :" + w_list_channels_and_op); } 
/* 322 */ std::string	RplList (std::string channel, std::string visible, std::string topic) { return (" " + channel + " " + visible + " :" + topic); }
/* 323 */ std::string	RplListEnd () { return (" :End of LIST"); }
/* 324 */ std::string	RplChannelModeIs (std::string channel, std::string mode, std::string mode_params) { return (" " + channel + " " + mode + " " + mode_params); }
/* 331 */ std::string	RplNoTopic (std::string channel) { return (" " + channel + " :No topic is set"); }	
/* 332 */ std::string	RplTopic (std::string channel, std::string topic) { return (" " + channel + " :" + topic); }
/* 341 */ std::string	RplInviting (std::string nick, std::string channel) { return (" " + channel + " " + nick); }
// 342 - No need
/* 346 */ std::string	RplInviteList (std::string channel, std::string invitemask) { return (" " + channel + " " + invitemask); }
/* 347 */ std::string	RplEndOfInviteList (std::string channel) { return (" " + channel + " :End of channel invite list"); }
/* 348 */ std::string	RplExceptList (std::string channel, std::string exceptionmask) { return (" " + channel + " " + exceptionmask); }
/* 349 */ std::string	RplEndOfExceptList (std::string channel) { return (" " + channel + " :End of channel invite list"); }
/* 351 */ std::string	RplVersion (std::string server, std::string version, std::string debug, std::string comments) { return (" " + version + "." + debug + " " + server + " :" + comments); }
/* 352 */ std::string	RplWhoReply (std::string server, std::string nick, std::string channel, std::string user, std::string host, std::string more, std::string hopcount, std::string real_name) { return (" " + channel + " " + user + " " + host + " " + server + " " + nick + " " + more + " :" + hopcount + " " +  real_name); }
// Explanation for 352 : http://chi.cs.uchicago.edu/chirc/assignment3.html
/* 353 */ std::string	RplNamReply (std::string type_chan, std::string channel, std::string nicks) { return (" " + type_chan + " " + channel + " :" + nicks); }
// nicks must contains each nicks of the users presents in the channel with either nothing/@/+ as prefix (@ = operator of channel, +=authorization to speak in moderated channel)
// 364 - No need
// 365 - No need
/* 366 */ std::string	RplEndOfNames (std::string channel) { return (" " + channel + " :Ed of NAMES list"); }
/* 367 */ std::string	RplBanList (std::string channel, std::string mask) { return (" " + channel + " " + mask); }
/* 368 */ std::string	RplEndOfBanList (std::string channel) { return (" " + channel + " :End of channel ban list"); }
/* 371 */ std::string	RplInfo (std::string infos) { return (" :" + infos); }
/* 372 */ std::string	RplMotd (std::string motd_line) { return (":- " + motd_line); }
// Note : Max 80 char for each motd line
/* 374 */ std::string	RplEndOfInfo () { return (" :End of INFO list"); }
/* 375 */ std::string	RplMotdStart (std::string server) { return (" :- " + server + "Massage of the day -"); }
/* 376 */ std::string	RplEndOfMotd () { return (" :End of MOTD command"); }
/* 381 */ std::string	RplYourOper () { return ( " :You are now an IRC operator"); }
// 382 - No need
// 383 - No need
/* 391 */ std::string	RplTime (std::string server, std::string time) { return (" " + server + " :" + time); }
// 392 to 395 - No need

/* ERROR REPLIES */
/* 401 */ std::string	ErrNoSuchNick (std::string nick) { return (" " + nick + " :No suck nick/channel"); }
/* 402 */ std::string	ErrNoSuchServer (std::string server_name) { return (" " + server_name + " :No such server"); }
/* 403 */ std::string	ErrNoSuchChannel (std::string channel_name) { return (" " + channel_name + " :No such channel"); }
/* 404 */ std::string	ErrCannotSendToChan (std::string channel_name) { return (" " + channel_name + " :No such channel"); }
/* 405 */ std::string	ErrTooManyChannels (std::string channel_name) { return (" " + channel_name + " :You have joined too many channels"); }
// 406 - No need
// 407 - NEED MORE INFOS
// 408 - No nedd
/* 409 */ std::string	ErrNoOrigin () { return (" :No origin specified"); }
/* 411 */ std::string	ErrNoRecipient (std::string command) { return (" : No recicipent given (" + command + ")"); }
/* 412 */ std::string	ErrNoTextToSend () { return (" : NO text to send"); }
/* 413 */ std::string	ErrNoTopLevel (std::string mask) { return (" " + mask + " :No toplevel domain specified"); }
/* 414 */ std::string	ErrWildTopLevel (std::string mask) { return (" " + mask + " :Wildcard in toplevel domain"); }
/* 415 */ std::string	ErrBadMask (std::string mask) { return (" " + mask + " :Bad Server/host mask"); }
/* 421 */ std::string	ErrUnknownCommand (std::string command) { return (" " +  command + "  :Unknown command"); }
/* 422 */ std::string	ErrNoMotd () { return (" : MOTD File is missing"); }
/* 423 */ std::string	ErrNoAdminInfo (std::string server) { return (" " + server + " : No administrative info available"); }
/* 424 */ std::string	ErrFileError (std::string file_op, std::string file) { return (" : File error doing " + file_op + " on " + file); }
/* 431 */ std::string	ErrNoNicknameGiven () { return ( " :No nickname given"); }
/* 432 */ std::string	ErrErroneusNickname (std::string nick) { return (" " + nick + " :Erroneous nickname"); }
/* 433 */ std::string	ErrNicknameInUse (std::string nick) { return (" " + nick + " :Nickname is already in use"); }
// 436 - No need
// 437 - No need
/* 441 */ std::string	ErrUserNotInChannel (std::string nick, std::string channel) { return (" " + nick + " " + channel +" : They aren't on that channel"); }
/* 442 */ std::string	ErrNotOnChannel (std::string channel) { return (" " + channel + " :You're not on that channel"); }
/* 443 */ std::string	ErrUserOnChannel (std::string user, std::string channel) { return (" " + user + " " + channel +" :is already on channel"); }
/* 444 */ std::string	ErrNoLogin (std::string user) { return (" " + user + " :Not logged in"); }
/* 445 */ std::string	ErrSummonDisabled () { return (" :SUMMON has been disabled"); }
/* 446 */ std::string	ErrUsersDisabled () { return (" :USERS has be disabled"); }
/* 451 */ std::string	ErrNotRegistered () { return (" :You have not registered"); }
/* 461 */ std::string	ErrNeedMoreParams (std::string command) { return (" " + command + " :Not enough parameters"); }
/* 462 */ std::string	ErrAlreadyRegistered () { return (" :Unauthorized command (already registered)"); }
// 463 - No need
/* 464 */ std::string	ErrPasswordMismatch () { return (" : Password incorrect"); }
// 465 - No need
// 466 - No need
/* 467 */ std::string	ErrKeySet (std::string channel) { return (" " + channel + " :Channel key already set"); }
/* 471 */ std::string	ErrChannelIsFull (std::string channel) { return (" " + channel + " :Cannot join channel (+l)"); }
/* 472 */ std::string	ErrUnknownMode (std::string letter, std::string channel) { return (" " + letter + " :is unknown mode char to me for " + channel); }
/* 473 */ std::string	ErrInviteOnlyChan (std::string channel) { return (" " + channel + " :Cannot join channel (+i)"); }
/* 474 */ std::string	ErrBannedFromChan (std::string channel) { return (" " + channel + " :Cannot join channel (+b)"); }
/* 475 */ std::string	ErrBadChannelKey (std::string channel) { return (" " + channel + " :Cannot join channel (+k)"); }
/* 476 */ std::string	ErrBadChanMask (std::string channel) { return (" " + channel + " :Bad Channel Mask"); }
/* 477 */ std::string	ErrNoChanModes (std::string channel) { return (" " + channel + " :Channel doesn't support mode"); }
/* 478 */ std::string	ErrBanListFull (std::string channel, std::string letter) { return (" " + channel + " " + letter + " :Channel list is full"); }
/* 481 */ std::string	ErrNoPrivileges () { return (" : Permission Denied- You're not an IRC operator"); }
/* 482 */ std::string	ErrChanNoPrivsNeeded (std::string channel) { return (" " + channel + " :You're not channel operator"); }
/* 483 */ std::string	ErrCantKillServer () { return (" :You can't kill a server"); }
/* 484 */ std::string	ErrRestricted () { return (" :Your connection is restricted!"); }
// 485 - No need
/* 491 */ std::string	ErrNoOperHost () { return (" :No O-lines for your host"); }
/* 501 */ std::string	ErrUModeUnknownFlag () { return (" :Unknown MODE flag"); }
/* 502 */ std::string	ErrUsersDontMatch () { return (" :Cannot change mode for tohers users"); }


// For user parameter :
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
			msg += RplWelcome(nick, s_params[0], s_params[1]);
			break;
		case 2:
			msg += RplYourHost(server, s_params[0]);
			break;
		case 3:
			msg += RplCreated(s_params[0]);
			break;
		case 4:
			msg += RplMyInfo(server, s_params[0], s_params[1], s_params[2]);
			break;
		case 221:
			msg += RplUModeIs(s_params[0]);
			break;
		case 256:
			msg += RplAdminMe(server);
			break;
		case 257:
			msg += RplAdminLocOne(s_params[0]);
			break;
		case 258:
			msg += RplAdminLocTwo(s_params[0]);
			break;
		case 259:
			msg += RplAdminEmail(s_params[0]);
			break;
		case 301:
			msg += RplAway(s_params[0], s_params[1]);
			break;
		case 302:
			msg += RplUserhost(s_params[0]);
			break;
		case 305:
			msg += RplUnaway();
			break;
		case 306:
			msg += RplNowAway();
			break;
		case 311:
			msg += RplWhoIsUser(s_params[0], s_params[1], s_params[2], s_params[3]);
			break;
		case 312:
			msg += RplWhoIsServer(server, s_params[0], s_params[1]);
			break;
		case 313:
			msg += RplWhoIsOperator(s_params[0]);
			break;
		case 315:
			msg += RplEndOfWho(s_params[0]);
			break;
		case 318:
			msg += RplEndOfWhoIs();
			break;
		case 322:
			msg += RplList(s_params[0], s_params[1], s_params[2]);
			break;
		case 323:
			msg += RplListEnd();
			break;
		case 324:
			msg += RplChannelModeIs(s_params[0], s_params[1], s_params[2]);
			break;
		case 331:
			msg += RplNoTopic(s_params[0]);
			break;
		case 332:
			msg += RplTopic(s_params[0], s_params[1]);
			break;
		case 341:
			msg += RplInviting(nick, s_params[0]);
			break;
		case 346:
			msg += RplInviteList(s_params[0], s_params[1]);
			break;
		case 347:
			msg += RplEndOfInviteList(s_params[0]);
			break;
		case 348:
			msg += RplExceptList(s_params[0], s_params[1]);
			break;
		case 349:
			msg += RplEndOfExceptList(s_params[0]);
			break;
		case 351:
			msg += RplVersion(server, s_params[0], s_params[1], s_params[2]);
			break;
		case 352:
			msg += RplWhoReply(server, nick, s_params[0], s_params[1], s_params[2], s_params[3], s_params[4], s_params[5]);
			break;
		case 353:
			msg += RplNamReply(s_params[0], s_params[1], s_params[2]);
			break;
		case 366:
			msg += RplEndOfNames(s_params[0]);
			break;
		case 367:
			msg += RplBanList(s_params[0], s_params[1]);
			break;
		case 368:
			msg += RplEndOfBanList(s_params[0]);
			break;
		case 371:
			msg += RplInfo(s_params[0]);
			break;
		case 372:
			msg += RplMotd(s_params[0]);
			break;
		case 374:
			msg += RplEndOfInfo();
			break;
		case 375:
			msg += RplMotdStart(server);
			break;
		case 376:
			msg += RplEndOfMotd();
			break;
		case 381:
			msg += RplYourOper();
			break;
		case 391:
			msg += RplTime(server, s_params[0]);
			break;
		case 401:
			msg += ErrNoSuchNick(s_params[0]);
			break;
		case 402:
			msg += ErrNoSuchServer(s_params[0]);
			break;
		case 403:
			msg += ErrNoSuchChannel(s_params[0]);
			break;
		case 404:
			msg += ErrCannotSendToChan(s_params[0]);
			break;
		case 405:
			msg += ErrTooManyChannels(s_params[0]);
			break;
		case 409:
			msg += ErrNoOrigin();
			break;
		case 411:
			msg += ErrNoRecipient(s_params[0]);
			break;
		case 412:
			msg += ErrNoTextToSend();
			break;
		case 413:
			msg += ErrNoTopLevel(s_params[0]);
			break;
		case 414:
			msg += ErrWildTopLevel(s_params[0]);
			break;
		case 415:
			msg += ErrBadMask(s_params[0]);
			break;
		case 421:
			msg += ErrUnknownCommand(s_params[0]);
			break;
		case 422:
			msg += ErrNoMotd();
			break;
		case 423:
			msg += ErrNoAdminInfo(server);
			break;
		case 424:
			msg += ErrFileError(s_params[0], s_params[1]);
			break;
		case 431:
			msg += ErrNoNicknameGiven();
			break;
		case 432:
			msg += ErrErroneusNickname(s_params[0]);
			break;
		case 433:
			msg += ErrNicknameInUse(s_params[0]);
			break;
		case 441:
			msg += ErrUserNotInChannel(nick, s_params[0]);
			break;
		case 442:
			msg += ErrNotOnChannel(s_params[0]);
			break;	
		case 443:
			msg += ErrUserOnChannel(s_params[0], s_params[1]);
			break;
		case 444:
			msg += ErrNoLogin(s_params[0]);
			break;
		case 445:
			msg += ErrSummonDisabled();
			break;
		case 446:
			msg += ErrUsersDisabled();
			break;
		case 451:
			msg += ErrNotRegistered();
			break;
		case 461:
			msg += ErrNeedMoreParams(s_params[0]);
			break;
		case 462:
			msg += ErrAlreadyRegistered();
			break;
		case 464:
			msg += ErrPasswordMismatch();
			break;
		case 467:
			msg += ErrKeySet(s_params[0]);
			break;
		case 471:
			msg += ErrChannelIsFull(s_params[0]);
			break;
		case 472:
			msg += ErrUnknownMode(s_params[0], s_params[1]);
			break;
		case 473:
			msg += ErrInviteOnlyChan(s_params[0]);
			break;
		case 474:
			msg += ErrBannedFromChan(s_params[0]);
			break;
		case 475:
			msg += ErrBadChannelKey(s_params[0]);
			break;
		case 476:
			msg += ErrBadChanMask(s_params[0]);
			break;
		case 477:
			msg += ErrNoChanModes(s_params[0]);
			break;
		case 478:
			msg += ErrBanListFull(s_params[0], s_params[1]);
			break;
		case 481:
			msg += ErrNoPrivileges();
			break;
		case 482:
			msg += ErrChanNoPrivsNeeded(s_params[0]);
			break;
		case 483:
			msg += ErrCantKillServer();
			break;
		case 484:
			msg += ErrRestricted();
			break;
		case 491:
			msg += ErrNoOperHost();
			break;
		case 501:
			msg += ErrUModeUnknownFlag();
			break;
		case 502:
			msg += ErrUsersDontMatch();
			break;
		default :
			msg += ErrUnknownCommand(s_num_nb);
			break;
	}
	msg += END;
	sendNumeric(fd, msg);
}
