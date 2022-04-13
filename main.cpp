#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "Command.hpp"
#include <signal.h>

using namespace irc;

bool	exit_val = false;

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
		std::cerr << "Missing arguments.\nMust be launch as following:\n./ircsrv <port> <password>" << std::endl;
		return 1;
	}

	// Check that port is a valid one (there's a particular range to it)
	if ((port = check_port_validity(argv[1])) == 0) {
		std::cerr << "Please, choose a valid port number (1-65535)." << std::endl;
		return 1;
	}

	signal(SIGINT, sig_handler_function);

	// TODO make verification for password
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
