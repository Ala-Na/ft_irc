#include "Numerics.hpp"

using namespace irc;

#define END "\r\n"

// TODO check bytes_send and maybe close connection if error ?
// TODO uncomment send and comment erase write
void	sendNumeric(int fd, std::string msg) {
	//int bytes_send = send(fd, msg.c_str(), msg.size(), 0);
	write(fd, msg.c_str(), msg.size());
}

/* 001 */ std::string	RplWelcome (std::string server, std::string nick, std::string network, std::string user, std::string host) { return (" :Welcome to the " + network + " Network, " + nick + "!" + user + "@" + host); }
/* 002 */ std::string	RplYourHost (std::string server, std::string version) { return (" :Your host is " + server + ", running version " + version);}
/* 003 */ std::string	RplCreated (std::string creation) { return (" :This server was created " + creation); }
/* 004 */ std::string	RplMyInfo (std::string server, std::string version, std::string u_modes, std::string c_modes, std::string supp_c_modes) { return (" " + server + " " + version + " " + u_modes + " " + c_modes + " " + supp_c_modes); }
/* 005 */ std::string	RplBounce (std::string server, std::string port) { return (" : Try server " + server + ", port " + port); }
/* 020 - Not in RFC, but used in irssi */ std::string	RplProcessConnection () { return (" :Please wait while we process your connection."); }
/* 301 */ std::string	RplAway (std::string dest_nick, std::string away_msg) { return (" " + dest_nick + " :" + away_msg); }
/* 302 */ std::string	RplUserhost (std::string replies) { return (" :" + replies); }
/* 305 */ std::string	RplUnaway () { return (" :You are no longer marked as being away"); }
/* 306 */ std::string	RplNowAway () { return (" : You have been marked as being away"); }
/* 311 */ std::string	RplWhoIsUser (std::string w_nick, std::string w_user, std::string w_host, std::string w_real) { return (" " + w_nick + " " + w_user + " " + w_host + " * :" + w_real); }
/* 312 */ std::string	RplWhoIsServer (std::string server, std::string infos) { return (" " + server + " :" + infos); };
/* 313 */ std::string	RplWhoIsOperator (std::string o_nick) { return (" " + o_nick + " :is an IRC operator"); }
// 317 - No need
/* 318 */ std::string	RplEndOfWhoIs () { return (" :End of WHOIS list"); }
// 319 - No need
/* 322 */ std::string	RplList (std::string channel, std::string visible, std::string topic) { return (" " + channel + " " + visible + " :" + topic); }
/* 323 */ std::string	RplListEnd () { return (" :End of LIST"); }
/* 324 */

// Examples :
// 442 :openirc.snt.utwente.nl 442 lolilol #Cimitero :You're not on that channel
// 474 :openirc.snt.utwente.nl 474 lolilol #Cimitero :Cannot join channel (+b)


// TODO check that server_name is in (...) for function where it's needed
// For user parameter :
void	irc::numericReply(int num_nb, int fd, std::string server, \
	std::string nick, int param_nb, ...) {
	va_list						params;
	std::vector<std::string>	s_params;
	std::string					msg;
	char		 				s_num_nb[4];

	if (nick.empty()) {
		nick += "*";
	}

	sprintf(s_num_nb, "%.3d", num_nb);
	msg = ":" + server + " " + s_num_nb + " " + nick;

	va_start(params, param_nb);
	for	(int i = 0; i < param_nb; i++) {
		std::string curr = va_arg(params, char*);
		s_params.push_back(curr);
	}
	switch (num_nb) {
		// TODO make system for non obligatory arguments or missing arguments
		case 1:
			msg += RplWelcome(server, nick, s_params[0], s_params[1], s_params[2]);
			break;
		case 2:
			msg += RplYourHost(server, s_params[0]);
			break;
		case 3:
			msg += RplCreated(s_params[0]);
			break;
		case 4:
			msg += RplMyInfo(server, s_params[0], s_params[1], s_params[2], s_params[3]);
			break;
		case 5:
			msg += RplBounce(server, s_params[0]);
		case 20:
			msg += RplProcessConnection();
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
			msg += RplWhoIsServer(server, s_params[0]);
			break;
		case 313:
			msg += RplWhoIsOperator(s_params[0]);
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
		default :
			// TODO set error
			break;
	}
	msg += END;
	sendNumeric(fd, msg);

}

int main() {
	irc::numericReply(20, 1, "server", "*", 0);
	irc::numericReply(1, 1, "server", "nick", 3, "user", "network", "host");
	irc::numericReply(2, 1, "server", "nick", 1, "version");
	irc::numericReply(3, 1, "server", "nick", 1, "date");
	irc::numericReply(4, 1, "server", "nick", 4, "version", "mode user", "mode channel", "more");
	irc::numericReply(301, 1, "server", "nick", 2, "from_dest", "Is away");
	irc::numericReply(302, 1, "server", "nick", 1, "user=+/-host");
	irc::numericReply(305, 1, "server", "nick", 0);
	irc::numericReply(306, 1, "server", "nick", 0);
	irc::numericReply(311, 1, "server", "nick", 4, "nick2", "user", "host", "real_name");
	irc::numericReply(312, 1, "server", "nick", 1, "Infos about server");
	irc::numericReply(313, 1, "server", "nick", 1, "nick2");
	irc::numericReply(318, 1, "server", "nick", 0);
	irc::numericReply(322, 1, "server", "nick", 3, "channel", "visible", "topic");
	irc::numericReply(323, 1, "server", "nick", 0);
}
