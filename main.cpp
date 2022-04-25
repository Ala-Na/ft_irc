#include <cstdlib>
#include <csignal>
#include "Server.hpp"

using namespace irc;

bool	irc::running = true;

bool	check_password(const char* password) {
	size_t	len = strlen(password);
	
	if (len > 10) {
		std::cerr << "Password is too long" << std::endl;
		return false;
	}
	for(size_t i = 0; i < len; i++) {
		if (!isalnum(password[i])) {
			std::cerr << "Password doesn't contains only alphanumerics characters" << std::endl;
			return false;
		}
	}
	return true;
}

short	check_port_validity(const char* port)
{
	int	port_nb = std::atoi(port);
	if (port_nb < 1 || port_nb > 65535)
		return 0;
	return port_nb;
}

void sig_handler_function(int signum)
{
	if (signum == SIGINT)
		irc::running = false;
	std::cout << std::endl;
}

int	main (int argc, char **argv)
{
	short		port;
	std::string	password;

	// First, check than arguments are presents
	if (argc != 3) {
		std::cerr << "Missing arguments.\nMust be launch as following:\n./ircserv <port> <password>" << std::endl;
		return 1;
	}

	// Check that port is a valid one (there's a particular range to it)
	if ((port = check_port_validity(argv[1])) == 0) {
		std::cerr << "Please, choose a valid port number (1-65535)." << std::endl;
		return 1;
	}

	signal(SIGINT, sig_handler_function);

	if (check_password(argv[2]) == false) {
		return 1;
	}
	password = argv[2];

	// Launch server
	Server* server = new Server(password, argv[1]);

	if (server->initServer() == -1) {
		std::cerr << "Error while initializing server socket." << std::endl;
		delete server;
		return 1;
	}

	if (server->runServer() == -1) {
		std::cerr << "Error while executing server." << std::endl;
		delete server;
		return 1;
	}

	delete server;

	std::cout << "Bye !" << std::endl;

	return 0;
}
