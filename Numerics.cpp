#include "Numerics.hpp"

using namespace irc;

#define END "\r\n"

// TODO check bytes_send and maybe close connection if error ?
// TODO uncomment send and comment erase write
void	sendNumeric(int fd, std::string msg) {
	//int bytes_send = send(fd, msg.c_str(), msg.size(), 0);
	write(fd, msg.c_str(), msg.size());
}

// 001
std::string	RplWelcome (std::string server, std::string nick, std::string network, \
	std::string user, std::string host)
{
	return (" :Welcome to the " + network + " Network, " + nick + "!" + user + \
		"@" + host);
}
// 002
std::string	RplYourHost (std::string server, std::string version) {
	return (" :Your host is " + server + ", running version " + version);
}
// 003
std::string	RplCreated (std::string creation) {
	return (" :This server was created " + creation);
}
// 004
std::string	RplMyInfo (std::string server, std::string version, \
	std::string u_modes, std::string c_modes, std::string supp_c_modes)
{
	return (" " + server + " " + version + " " + u_modes + " " +\
		c_modes + " " + supp_c_modes);
}
// 005
std::string	RplBounce (std::string server, std::string port) {
	return (" : Try server " + server + ", port " + port);
}
// 020 - Not in RFC, but used in irssi
std::string	RplProcessConnection () {
	return (" :Please wait while we process your connection.");
}
// 301
std::string	RplAway (std::string nick, std::string dest_nick, std::string away_msg) {
	return (nick + " " + dest_nick + " :" + away_msg);
}
// 302
// WARNING Need to generate reply before
std::string	RplUserhost (std::string replies) {
	return (" :" + replies);
	// there can be multiple replies, which should be in format :
	// If operator without away message : nickname*=-hostname
	// If operator with away message : nickname*=+hostname
	// If non-operator without away message : nickname=-hostname
	// If non-operator with away message : nickname=+hostname
}


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
			msg += RplAway(nick, s_params[0], s_params[1]);
		case 302:
			msg += RplUserhost(s_params[0]);
			break;
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
	irc::numericReply(302, 1, "server", "nick", 1, "user=+/-host");
}
