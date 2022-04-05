#include "./Server/Server.hpp"

bool	running = true;

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
		running = false;
}

int	main (int argc, char **argv)
{
	short	port;

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

	// Launch server
	Server server = new Server(argv[2], argv[1]);

	if (server.initServer() == -1) {
		std::cerr << "Error while initializing server socket." << std::endl;
		return 1;
	}

	while (running == true) {
		if (server.runServer() == -1) {
			break;
		}
		// Execution server
	}

	delete server;

	return 0;
}
