#include "Server.hpp"

Server::Server(std::string password, int port) : password(password), port(port), \
	name("In Real unControl - An ft_irc server")
{
	this->users = new std::vector<User *>();
	this->channels = new std::vector<User *>();
	this->pfds = new std::vector<pollfd>();
	std::cout << "Initializating server..." << std::endl;
}

Server::~Server() {
	delete this->users;
	delete this->channels;
	for (std::vector<pollfd>::iterator it = pfds.rbegin(); it != pfds.rend(); it++) {
		close((*it).fd);
	}
	delete this->pfds;
}

int	Server::initServer() {
	struct addrinfo	hints;
	struct addrinfo	*ai;
	struct addrinfo	*p;
	int	server_fd;
	int	opt = 1;
	int res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((res = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
		std::cerr << "getaddrinfo error " << gai_strerror(res) << std::endl;
		return (-1);
	}
	for (p = ai; p != NULL; p = p->ai_next) {
		server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (server_fd < 0) {
			continue;
		}
		setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
		if (bind(server_fd, p->ai_addr, p->ai_addrlen) < 0) {
			close(server_fd);
			continue;
		}
		break;
	}
	freeaddrinfo(ai);
	if (p == NULL) {
		return (-1);
	}
	if (listen(server_fd, 10) == -1) {
		return (-1);
	}
	this->server_socket = server_fd;
	return this->server_socket;
}

int	Server::runServer() {
	struct sockaddr_in	client_addr;
	socklen_t			addr_len;
	int					cli_fd;
	char				buf[BUF_SIZE + 1];

	// We add the server socket fd to a vector of pollfd.
	add_socket_fd_to_poll(pfds, this->server_socket);

	std::cout << "Server started" << std::endl;

	int poll_count = poll(&pfds[0], pfds.size(), -1);

	if (!exit_val && poll_count < 0) {
		std::cerr << "poll error" << std::endl;
		return (-1);
	} else if (!exit_val) {
		std::cout << "Number of detected connection:" << pfds.size() - 1 << std::endl;
		if (pfds[0].revents & POLLIN) {
			addr_len = sizeof(cli_addr);
			cli_fd = accept(server_fd, (struct sockaddr *)&cli_addr, &addr_len);
			add_socket_fd_to_poll(pfds, cli_fd);
			std::cout << "New connection accepted from " << inet_ntoa(cli_addr.sin_addr) << std::endl;
		}
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
