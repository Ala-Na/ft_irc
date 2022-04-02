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

#define BUF_SIZE 1024

short	check_port_validity(const char* port)
{
	int	port_nb = std::atoi(port);
	if (port_nb < 1 || port_nb > 65535)
		return 0;
	return port_nb;
}

int	init_server_socket(const char* port)
{
	// Structure for informations about host name
	struct addrinfo	hints;
	struct addrinfo	*ai;
	struct addrinfo	*p;
	int	server_fd;
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
		std::cerr << "getaddrinfo error " << gai_strerror(res) << std::endl;
		return (-1);
	}

	// We explore each element of the linked list ai obtained before.
	for (p = ai; p != NULL; p = p->ai_next)
	{
		// We obtain a socket fd for each element of the linked list
		server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (server_fd < 0)
			continue;
		

		// We allows other socket to bind to the same port to avoid
		// "address already in use" error message
		setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

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
	fcntl(new_fd, F_SETFL, O_NONBLOCK);
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
	std::vector<pollfd>	pfds;
	struct sockaddr_in	cli_addr;
	socklen_t			addr_len;
	int					cli_fd;
	char				buf[BUF_SIZE + 1];

	add_socket_fd_to_poll(pfds, server_fd);

	std::cout << "Server started" << std::endl;

	// TODO take ctrl D in account with signal to stop execution
	while (1) {
		int poll_count = poll(&pfds[0], pfds.size(), -1);
		if (poll_count < 0) {
			std::cerr << "poll error" << std::endl;
			exit(1);
		} else {
			std::cout << "Number of detected connection:" << pfds.size() - 1 << std::endl;
			// First socket = server socket accept users when revents is ready to read data
			if (pfds[0].revents & POLLIN) {
				addr_len = sizeof(cli_addr);
				cli_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &addr_len);
				add_socket_fd_to_poll(pfds, cli_fd);
				std::cout << "New connection accepted from " << inet_ntoa(cli_addr.sin_addr) << std::endl;
			}
			for (std::vector<pollfd>::iterator it = pfds.begin() + 1; it != pfds.end(); it++) {
				if ((*it).revents & POLLIN) {
					// Others sockets = clients sockets, received data when their is some to receive
					ssize_t	bytes_recv = recv((*it).fd, &buf, BUF_SIZE, 0);
					// Case of recv error or end of connection
					if (bytes_recv <= 0) {
						if (bytes_recv == -1) {
							std::cerr << "recv error" << std::endl;
						} else {
							std::cout << "Client " << (*it).fd << " exited" << std::endl;
						}
						close((*it).fd);
						delete_socket_fd_from_poll(pfds, it);
						break ;
					} else {
						buf[bytes_recv] = 0;
						std::cout << "From client (fd = " << (*it).fd << "): " << buf << std::endl;
					}
				}
			}
		}
	}
}

int	main (int argc, char **argv)
{
	short	port;
	(void)argv[2];

	// First, check than arguments are presents
	if (argc != 3) {
		std::cerr << "Missing arguments.\nMust be launch as following:\n./ircsrv <port> <password>" << std::endl;
		return 1;
	}

	// Check that port is a valid one 
	if ((port = check_port_validity(argv[1])) == 0) {
		std::cerr << "Please, choose a valid port number (1-65535)." << std::endl;
		return 1;
	}

	// We initialize the first socket fd on given port.
	// Each socket fd will be a "copy" of this one.
	int server_fd = init_server_socket(argv[1]);
	if (server_fd == -1)
		return 1;

	interact_with_client(server_fd);

	close(server_fd);

	return 0;
}
