<<<<<<< HEAD
#include <cstdlib>
#include <csignal>
#include "./Server/Server.hpp"

using namespace irc;

bool	irc::running = true;
=======
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <vector>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "Command.hpp"

#define BUF_SIZE 1024

bool	exit_val = false;
>>>>>>> 75535ef (Add files via upload)

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
<<<<<<< HEAD
		irc::running = false;
	std::cout << std::endl;
=======
		exit_val = true;
}

int	init_server_socket(const char* port)
{
	// Structure for informations about host, necessary for server configuration
	// hints will be used to set the server socket configuration.
	struct addrinfo	hints;
	// ai will be filled with server socket informations thanks to getaddrinfo() function.
	struct addrinfo	*ai;
	// p is not really useful. But sometime, getaddrinfo() can return a linked list
	// of potential sockets instead of one. p is only a pointer on ai to go through
	// this linked list.
	struct addrinfo	*p;
	// the futur fd of the server socket.
	int	server_fd;
	// just a non-zero value necessary to setsockopt() function
	int	opt = 1;
	// value to stock getaddrinfo() result, in case there is an error
	int res;

	// Necessary step to be sure that hints is an empty addrinfo struct
	memset(&hints, 0, sizeof(hints));
	// We set the structure with network setup.
	// AF_INET means we're gonna use IPv4
	hints.ai_family = AF_INET;
	// There is two kind of socket (stream and datagram). Stream is the type used
	// for client/server communication.
	hints.ai_socktype = SOCK_STREAM;
	// AI_PASSIVE make the struct assign the localhost adress to socket structures.
	// Used when first parameter of getaddrinfo is NULL.
	// Here, because application intend to communicate on the same host than peers/
	hints.ai_flags = AI_PASSIVE;

	// getadrrinfo ) function to return information about hostname
	// Here, the port in given as an argument to obtain
	// linked list of struct addrinfo *ai results about it.
	// An error check (res != 0) is also performed.
	if ((res = getaddrinfo(NULL, port, &hints, &ai)) != 0)
	{
		std::cerr << "getaddrinfo error " << gai_strerror(res) << std::endl;
		return (-1);
	}

	// We explore each element of the linked list ai obtained before.
	for (p = ai; p != NULL; p = p->ai_next)
	{
		// We obtain a socket fd for each element of the linked list
		server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (server_fd < 0) // Error check, it'll betaken in condiseration inside the main
			continue;

		// We allows other socket to bind to the same port to avoid
		// "address already in use" error message
		setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

		// The socket fd as to be put in non blocking mode (some functions used on it
		// can stop the whole process if nothing happens). It's commented as
		// it's made in the add_socket_fd_to_poll function.
		//fcntl(server_fd, F_SETFL, O_NONBLOCK);

		// We associate the obtained socket fd to the corresponding address
		// information structure. The socket fd is then associated to a port
		// number.
		// Useful for server.
		if (bind(server_fd, p->ai_addr, p->ai_addrlen) < 0)
		{
			// If we can't bind, we close the socket fd to avoid fd leaks.
			close(server_fd);
			continue;
		}
		break;
	}

	// We no longer need the ai linked list of information which were allocated.
	// We free the memory with that function.
	freeaddrinfo(ai);

	// Another error check.
	if (p == NULL)
		return (-1);

	// We tell the socket to listen to connections, beacause, duh, that's what
	// a server does.
	// In most systemn second parameter corresponds to number of pending connection.
	if (listen(server_fd, 10) == -1)
		return (-1);

	// We return the socket fd if everything was successful.
	return server_fd;
}

// Add a socket fd to pollfd vector
void	add_socket_fd_to_poll(std::vector<pollfd>& pfds, int& new_fd)
{
	// Some function can block the whole process. For example, recv called on a
	// client socket can prevent it to be used to send data if nothing is received.
	// fcntl call permits to put it in non blocking mode with last parameter.
	fcntl(new_fd, F_SETFL, O_NONBLOCK);

	// Here, we just add the new fd corresponding either to server or client socket
	// at the end of the pollfd vector.
	// With .events = POLLIN, we say that we're looking for POLLIN event (data/connection received).
	pfds.push_back(pollfd());
	pfds.back().fd = new_fd;
	pfds.back().events = POLLIN;
}

// Delete a socket fd from pollfd vector
void	delete_socket_fd_from_poll(std::vector<pollfd>& pfds, std::vector<pollfd>::iterator place)
{
	pfds.erase(place);
}

void	interact_with_client(int server_fd)
{
	// A vector of struct pollfd, which will be used by poll() to check if any
	// socket stocked inside had a change of status.
	std::vector<pollfd>	pfds;

	// Structures and variables necessary to accept a new client socket through
	// the server socket and messages/data through it.
	struct sockaddr_in	cli_addr;
	socklen_t			addr_len;
	int					cli_fd;
	char				buf[BUF_SIZE + 1];

	// We add the server socket fd to a vector of pollfd.
	add_socket_fd_to_poll(pfds, server_fd);

	std::cout << "Server started" << std::endl;

	while (!exit_val) {
		// Here's the poll function, it permits to detect when a change of status
		// is done in all the fd stocked inside pfds.
		// For example, when data is send through a socket, a change of status
		// is made.
		// It's a utilary function, to avoid checking constantly sockets state.
		// There's others similar functions like select().
		int poll_count = poll(&pfds[0], pfds.size(), -1);
		if (!exit_val && poll_count < 0) { // Just an error check. Exit is made in this case.
			std::cerr << "poll error" << std::endl;
			exit(1);
		} else if (!exit_val) {
			std::cout << "Number of detected connection:" << pfds.size() - 1 << std::endl;
			// pfds[0] = Ffirst socket = server socket accept users when revents
			// is ready to read data thanks to POLLIN mask (= a connection is coming through server).
			if (pfds[0].revents & POLLIN) {
				addr_len = sizeof(cli_addr);
				// Here, we accept the pending connection and obtain a new fd for it.
				cli_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &addr_len);
				add_socket_fd_to_poll(pfds, cli_fd);
				std::cout << "New connection accepted from " << inet_ntoa(cli_addr.sin_addr) << std::endl;
			}
			// We check that all other sockets (= clients sockets), are receiving data or not.
			for (std::vector<pollfd>::iterator it = pfds.begin() + 1; it != pfds.end(); it++) {
				// POLLIN = data to receive through client socket fd.
				if ((*it).revents & POLLIN) {
					// To retrieve this datam we make a call to recv() function.
					ssize_t	bytes_recv = recv((*it).fd, &buf, BUF_SIZE, 0);
					// We check for recv() errors or end of client connection.
					if (bytes_recv <= 0) {
						if (bytes_recv == -1) { // Case of error with recv
							std::cerr << "recv error" << std::endl;
						} else { // Case where client connection closed
							std::cout << "Client " << (*it).fd << " exited" << std::endl;
						}
						// We close the corresponding fd to avoid fd leak
						close((*it).fd);
						// We delete the fd socket from the vector pfds, in order to
						// stop checking the fd status.
						delete_socket_fd_from_poll(pfds, it);
						break ;
					} else {
						// Here, we received data correctly. The buffer is filled with data,
						// we had a '\0' at the end of it.
						buf[bytes_recv] = 0;
						// Just displaying the message received.
						std::cout << "From client (fd = " << (*it).fd << "): " << buf << std::endl;
					}
				}
			}
		}
	}
	// If ctrl+C is done, we close all clients socket fd and clear pfds vector.
	for (std::vector<pollfd>::iterator it = pfds.begin() + 1; it != pfds.end(); it++)
		close((*it).fd);
	pfds.clear();
>>>>>>> 75535ef (Add files via upload)
}

int	main (int argc, char **argv)
{
<<<<<<< HEAD
	short		port;
	std::string	password;
=======
	short	port;
	(void)argv[2];
>>>>>>> 75535ef (Add files via upload)

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

<<<<<<< HEAD
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
=======
	// If ctrl+C is send, we change a global variable exit_val
	signal(SIGINT, sig_handler_function);

	// We initialize the first socket fd on given port.
	// Each socket fd will be a "copy" of this one.
	int server_fd = init_server_socket(argv[1]);
	if (server_fd == -1)
		return 1;

	// Loop to interact with client (accept connections, receive data)
	interact_with_client(server_fd);

	// We close sever socket fd to avoid fd leaks
	close(server_fd);

	std::cout << std::endl << " Closing server" << std::endl;

	return 0;
}
>>>>>>> 75535ef (Add files via upload)
