#include "Server.hpp"

using namespace irc;

// TODO check name validity
Server::Server(std::string password, int port) : password(password), port(port), \
	name("In Real unControl - An ft_irc server")
{
	std::cout << "Initializating server..." << std::endl;
	this->users = new std::vector<User *>();
	this->channels = new std::vector<User *>();
	this->pfds = new std::vector<pollfd>();
}

Server::~Server() {
	std::cout << "Closing server..." << std::endl;
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

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((res = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
		std::cerr << "Error : getaddrinfo() - " << gai_strerror(res) << std::endl;
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
	this->addSocketToPoll(this->server_socket);

	std::cout << "Server started" << std::endl;

	int poll_count = poll(&pfds[0], pfds.size(), -1);

	while (runnning == true) {
		if (running = true && poll_count < 0) {
			std::cerr << "Error: poll()" << std::endl;
			return (-1);
		} else if (running == true) {
			if (pfds[0].revents & POLLIN) {
				this->createUser();
			}
			this->receiveMessages();
		}
	}
}

void	Server::addSocketToPoll(int socket_fd) {
	fcntl(new_fd, F_SETFL, O_NONBLOCK);
	this->pfds.push_back(pollfd());
	this->pfds.back().fd = new_fd;
	this->pfds.back().events = POLLIN;
}

void	Server::deleteSocketFromPoll(std::vector<pollfd>::iterator& to_del) {
	std::cout << "Closing connection to client fd = " << (*to_del).fd << std::endl;

	int	position = to_del - this->pfds.begin();

	close((*to_del).fd);
	this->pfds.erase(place);
	delete (*(this->users[position]));
	this->users.erase(position);
	this->messages.erase(position);

}

void	Server::createUser() {
	// TODO create max number of user to avoid slow server
	struct sockaddr_in	client_addr;
	socklen_t			addr_len;
	int					client_fd;

	addr_len = sizeof(client_addr);
	client_fd = accept(this->server_socket, (struct sockaddr *)&client_addr, &addr_len);
	this->addSocketToPoll(client_fd);
	std::cout << "Accepting new connection from " << inet_ntoa(client_addr.sin_addr) << " on fd :" << client_fd << std::endl;
	this->users.push_back(new User(user_fd, address));
	this->messages.push_back("");
}

// TODO check parameters for channel creation
Channel*	Server::createChannel(std::string name) {
	this->channels.push_back(new Channel(name, this));
	return this->channels.back();
}
# TODO create channel method which could be called by channel

void	Server::receiveDatas() {
	char	buf[BUF_SIZE + 1];

	for (std::vector<pollfd>::iterator it = pfds.begin() + 1; it != pfds.end(); it++) {
		if ((*it).revents & POLLIN) {
			ssize_t	bytes_recv = recv((*it).fd, &buf, BUF_SIZE, 0);
			if (bytes_recv <= 0) {
				if (bytes_recv == -1) {
					std::cerr << "Error : recv()" << std::endl;
				} else {
					std::cout << "Client " << (*it).fd << " exited" << std::endl;
				}
				this->deleteSocketFromPoll(it);
				break ;
			} else {
				buf[bytes_recv] = 0;
				std::cout << "From client (fd = " << (*it).fd << "): " << buf << std::endl;
				this->datasExtraction(&buf, it - pfds.begin());
			}
}

void	Server::datasExtraction(const char* buf, int pos) {
	User *user = this->getSpecificUser(pos - 1);
	datas[pos].append(buf);
	size_t cmd_end = datas[pos].find("\r\n");
	while (cmd_end != string::npos) {
		std::string	content = datas[pos].substr(0, cmd_end);
		datas[pos].erase(0, cmd_end + 2);
		cmd_end = datas[pos].find("\r\n");
		Command cmd = new Command(this.getServer(), user, content);
		cmd.parseCommand();
		delete cmd;
	}
}

Server&	Server::getServer() const {
	return *this;
}

// Here, user_nb is from 0 to max - 1.
User*	Server::getSpecificUser(int user_nb) const {
	if (user_nb < this->users.size())
		return this->users[user_nb];
	return NULL;
}

Channel*	Server::getChannelByName(std::string name) const {
	for (std::vector<Channel *>::iterator it = this->channels.begin(); \
		it != this->channels.end(); it++)
	{
		// TODO
		// Check if (*it)->getname() == name;
		// If true, return channel;
	}
	return NULL;
}

