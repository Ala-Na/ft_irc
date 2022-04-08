#include "Numerics.hpp"

using namespace irc;

#define END "\r\n"


// TODO check bytes_send and maybe close connection if error ?
// TODO uncomment send and comment erase write

// 001
void	RplWelcome (int fd, std::string server, std::string user, \
	std::string network, std::string nick, std::string host) {
	std::string msg = ":" + server + " 001 " + user + " :Welcome to the " + \
		network + " Network, " + nick;
	if (!host.empty()) {
		msg += "!" + user + "@" + host;
	}
	msg += END;
	//int bytes_send = send(fd, msg.c_str(), msg.size(), 0);
	write(fd, msg.c_str(), msg.size());
};

// 020 - Not in RFC, but used in irssi
void	RplProcessConnection (int fd, std::string server) {
	std::string msg = ":" + server + " 020 * :Please wait while we process your connection." + END;
	//int bytes_send = send(fd, msg.c_str(), msg.size(), 0);
	write(fd, msg.c_str(), msg.size());
}

// TODO check thta server_name is in ... for function where it's needed
void	irc::getNumericReply(int num_nb, int fd, int param_nb, ...) {
	va_list						params;
	std::vector<std::string>	s_params;

	va_start(params, param_nb);
	for	(int i = 0; i < param_nb; i++) {
		std::string curr = va_arg(params, char*);
		s_params.push_back(curr);
	}
	switch (num_nb) {
		case 1:
			RplWelcome (fd, s_params[0], s_params[1], s_params[2], s_params[3], s_params[4]);
			break ;
		case 20:
			RplProcessConnection (fd, s_params[0]);
			break ;
		default :
			// Error
			break ;
	}

}

int main() {
	irc::getNumericReply(20, 1, 3, "Coucou", "est-ce que", "ca marche ?");
}
