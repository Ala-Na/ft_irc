#include <iostream>
#include <stdlib.h>

short	check_port_validity(const char* port)
{
	int	port_nb = std::atoi(port);
	if (port_nb < 1 || port_nb > 65535)
		return 0;
	return port_nb;
}

int	init_socket(const char* port)
{
	// Structure for informations about host name
	struct addrinfo	hints;
	struct addrinfo	*ai;
	struct addrinfo	*p;
	int	sockfd;
	int	opt = 1;
	int res;

	// Necessary step to be sure that hints is an empty addrinfo struct
	memset(&hints, 0, sizeof(hints));
	// We set the structure with network setup.
	// AF_INET means we're gonna use IPv4
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	// AI_PASSIVE make the struct assign the localhost adress to socket structures.
	// Used when first parameter of getaddrinfo is NULL.
	// Here, because application intend to communicate on the same host than peers/
	hints.ai_flags = AI_PASSIVE;

	// getadrrinfo ) function to return information about hostname
	// Here, the port in given as an argument to obtain
	// linked list of struct addrinfo *ai results about it.
	if ((res = getaddrinfo(NULL, port, &hints, &ai)) != 0)
	{
		std::cerr << "getaddrinfo error " << gai_strerror(res); << std::endl;
		return (-1);
	}

	// We explore each element of the linked list ai obtained before.
	for (p = ai; p != NULL; p = p->ai_next)
	{
		// We obtain a socket fd for each element of the linked list
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd < 0)
			continue;
		

		// We allows other socket to bind to the same port to avoid
		// "address already in use" error message
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		// We associate the obtained socket fd to the corresponding address
		// information structure. The socket fd is then associated to a port
		// number.
		// Useful for server.
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0)
		{
			// If we can't bind, we close the socket fd to avoid fd leaks.
			close(sockfd);
			continue;
		}
		break;
	}

	// We no longer need the ai linked list of information which were allocated.
	// We free the memory with that function.
	freeaddrinfo(ai);
	
	if (p == NULL)
		return (-1);

	// We tell the socket to listen to connections, beacause, duh, that's what
	// a server does.
	// In most systemn second parameter corresponds to number of pending connection.
	if (listen(sockfd, 10) == -1)
		return (-1);
	
	// We return the socket fd if everything was successful.
	return sockfd;
}

// Add a socket fd to pollfd vector
void	add_socket_fd_to_poll(std::vector<pollfd> pfds, int new_fd)
{
	pfds.push_back(pollfd());
	pfds.back().fd = new_fd;
	pfds.back().events = POLLIN;
}

// Delete a socket fd from pollfd vector
void	delete_socket_fd_from_poll(std::vector<pollfd> pfds, int place)
{
	if (place < pfds.size())
		pfds.erase(pfds.begin() + place);
}

int	main (int argc, char **argv)
{
	short	port;
	(void)argv[2];
	
	// First, check than arguments are presents
	if (argc != 3) {
		std::cerr << "Missing arguments.\nMust be launch as following:\n./ircsrv <port> <password>\n" std::endl;
		return 1;
	}

	// Check that port is a valid one 
	if ((port = check_port_validity(argv[1])) == 0) {
		std::cerr << "Please, choose a valid port number (1-65535)." << std::endl;
		return 1;
	}

	// We declare a vector a pollfd struct
	std::vector<pollfd> pfds;
	int	sockfd;

	// We initialize the first socket fd on given port.
	// Each socket fd will be a "copy" of this one.
	sockfd = init_socket(argv[1]);
	if (sockfd == -1)
		return 1;
	
	pfds.push_back(pollfd());
	pfds[0].fd = sockfd;
	pfds[0].events = POLLIN;

	while (1)
	{
		int poll_count = poll(&pfds[0], pfds.size(), -1);
		if (poll_count == -1) {
			std::cerr << "Poll error" << std::endl;
			return 1;
		}
		for (int i = 0; i < pfds.size(); i++)
		{
			if (pfds[i].revents & POLLIN) {
				if (pfds[i].fd == sockfd) {

				}
			}
		}


	}

}
