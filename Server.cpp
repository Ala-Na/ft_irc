#include "Server.hpp"

using namespace irc;

// TODO check name validity
Server::Server(std::string password, const char* port) : password(password), port(port), \
	name("In Real unControl - An ft_irc server")
{
	std::cout << "Initializating server..." << std::endl;
	this->users = std::vector<User *>();
	this->channels = std::vector<Channel *>();
	this->pfds = std::vector<pollfd>();
	// TODO recuperate info from .conf file
}

Server::~Server() {
	std::cout << "Closing server..." << std::endl;
	for (std::vector<pollfd>::reverse_iterator it = pfds.rbegin(); it != pfds.rend(); it++) {
		close((*it).fd);
	}
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

	while (running == true) {
		int poll_count = poll(&pfds[0], pfds.size(), -1);
		if (running == true && poll_count < 0) {
			std::cerr << "Error: poll()" << std::endl;
			return (-1);
		} else if (running == true) {
			if (pfds[0].revents & POLLIN) {
				this->createUser();
			}
			this->receiveDatas();
		}
	}
	return 0;
}

void	Server::addSocketToPoll(int socket_fd) {
	fcntl(socket_fd, F_SETFL, O_NONBLOCK);
	this->pfds.push_back(pollfd());
	this->pfds.back().fd = socket_fd;
	this->pfds.back().events = POLLIN;
}

void	Server::deleteSocketFromPoll(std::vector<pollfd>::iterator& to_del) {
	std::cout << "Closing connection to client fd = " << (*to_del).fd << std::endl;

	int	position = to_del - this->pfds.begin();

	close((*to_del).fd);
	this->pfds.erase(to_del);
	std::vector<User *>::iterator it1 = this->users.begin();
	std::advance(it1, position);
	delete (*it1);
	this->users.erase(it1);
	std::vector<std::string>::iterator it2 = this->datas.begin();
	std::advance(it2, position);
	this->datas.erase(it2);

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
	this->users.push_back(new User(client_fd, client_addr));
	this->datas.push_back("");
}

void	Server::deleteUser (User* user) {
	for (std::vector<User *>::iterator it1 = this->users.begin(); \
			it1 != this->users.end(); it1++) {
		if ((*it1) == user) {
			// TODO Response with ERROR
			int position = it1 - this->users.begin();
			std::vector<std::string>::iterator it2 = this->datas.begin();
			std::advance(it2, position);
			this->datas.erase(it2);
			std::vector<pollfd>::iterator it3 = this->pfds.begin();
			std::advance(it3, position);
			close((*it3).fd);
			this->pfds.erase(it3);
			delete *it1;
		}
	}
}

int	Server::isServOp(User & user)
{
	unsigned long	i;

	i = 0;
	while (i < operators.size())
	{
		if (operators[i]->getNickname() == user.getNickname())
			return (1);
		i++;
	}
	return (0);
}

// TODO check parameters for channel creation
Channel*	Server::createChannel(std::string name) {
	this->channels.push_back(new Channel(this, name));
	return this->channels.back();
}
// TODO create channel method which could be called by channel

void	Server::receiveDatas() {
	char		buf[SERV_BUF_SIZE + 1];
	std::string	s_buf;

	for (std::vector<pollfd>::iterator it = pfds.begin() + 1; it != pfds.end(); it++) {
		if ((*it).revents & POLLIN) {
			ssize_t	bytes_recv = recv((*it).fd, &buf, SERV_BUF_SIZE, 0);
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
				s_buf = buf;
				this->datasExtraction(s_buf, it - pfds.begin() - 1);
			}
		}
	}
}

void	Server::datasExtraction(std::string& buf, int pos) {
	User *user = this->getSpecificUser(pos - 1);
	datas[pos].append(buf);
	size_t cmd_end = datas[pos].find("\r\n");
	while (cmd_end != std::string::npos) {
		std::string	content = datas[pos].substr(0, cmd_end);
		datas[pos].erase(0, cmd_end + 2);
		cmd_end = datas[pos].find("\r\n");
		Command* cmd = new Command(getServer(), user, content);
		cmd->parseCommand();
		delete cmd;
	}
}

Server&	Server::getServer() {
	return *this;
}

// Here, user_nb is from 0 to max - 1.
User*	Server::getSpecificUser(unsigned long user_nb) {
	if (user_nb < this->users.size())
		return this->users[user_nb];
	return NULL;
}

Channel*	Server::getChannelByName(std::string name) {
	for (std::vector<Channel *>::iterator it = this->channels.begin();
		it != this->channels.end(); it++)
	{
		// TODO
		// Check if (*it)->getname() == name;
		// If true, return channel;
		(void)name;
	}
	return NULL;
}

// User*	Server::getUserByName(std::string name) const {
// 	for (std::vector<User *>::iterator it = this->users.begin();
// 		it != this->users.end(); it++)
// 	{
// 		// TODO
// 		// Check if (*it)->getname() == name;
// 		// If true, return user;
// 	}
// 	return NULL;
// }

User *  Server::getUserByUsername(std::string name)
{
	unsigned long i;

	i = 0;
	while (i < users.size())
	{
		if (users[i]->getUsername() == name)
			return (users[i]);
		i++;
	}
	return (NULL);
}

User * Server::getUserByNick(std::string nick)
{
	unsigned long i;

	i = 0;
	while (i < users.size())
	{
		if (users[i]->getNickname() == nick)
			return (users[i]);
		i++;
	}
	return (NULL);
}

void	Server::checkPassword(User* user, std::string parameters) {
	// if user->isRegistered() == true {}
		// Send error ERR_ALREADYREGISTERED 462
	// }
	if (parameters.empty()) {
		// Send error ERR_NEEDMOREPARAMS 461
		this->deleteUser(user);
	} else if (parameters.compare(this->password) != 0) {
		// Send error ERR_PASSWDMISMATCH 464
		this->deleteUser(user);
	}
	// Modify user to take into account the PASS status and wait for NICK and USER.
}

void	Server::listChannels (User* user) {
	int fd = user->getFd();
	(void)fd;
	// Maybe send 321 and 323 from intList ?
	// Send RPL_liststart 321
	for (std::vector<Channel *>::iterator it = this->channels.begin(); it != this->channels.end(); it++) {
		//(*it).getChanName()
		// Response must be : '322 nickname channelname nb_user_in_channel: topic'
		// Maybe do call to function ?
	}
	// Send RPL_LISTEND 323
}

int	there_is_no_server(char c, std::string str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (0);
		i++;
	}
	return (1);
};

std::vector<std::string>	split_server(std::string text, std::string space_delimiter)
{
	std::vector<std::string> words;

	size_t pos = 0;
	while ((pos = text.find_first_of(space_delimiter)) != std::string::npos)
	{
		std::cout << "substr: " << text.substr(0) << std::endl;
		if (text[0] == ':')
		{
			words.push_back(text.substr(0));
			return (words);
		}
		words.push_back(text.substr(0, pos));
		int nbDelimiters = 0;
		while (there_is_no_server(text[pos + nbDelimiters], space_delimiter) == 0)
			nbDelimiters++;
		text.erase(0, pos + nbDelimiters);
	}
	words.push_back(text.substr(0));
	words.push_back("\0");
	return (words);
}

void	Server::getMotdFile(User* user, std::string parameters) {
	int fd = user->getFd();
	(void)fd;

	// TODO use split of channel_management branch
	if (!parameters.empty() && parameters.compare(this->name)) {
		// Send ERR_NOSUCHSERVER 402
		return ;
	} else if (motd.empty()) {
		// Send ERR_NOMOTD 422
		return ;
	}
	// Send RPL_MOTDSTART 375
	std::vector<std::string> lines = split_server(motd, "\n");
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
		// Send RPL_MOTD 372 with (*it) 
	}
	// Send RPL_ENDOFMOTD 376
}

void	Server::retrieveTime(User* user, std::string parameters) {
	int fd = user->getFd();
	(void)fd;

	if (!parameters.empty() && parameters.compare(this->name)) {
		// Send ERR_NOSUCHSERVER 402
		return ;
	}
	std::time_t time;
	std::time(&time);
	std::string s_time = std::asctime(std::localtime(&time));
	// Send RPL_TIME 391 with time as parameter
}

void	Server::retrieveVersion(User* user, std::string parameters) {
	int fd = user->getFd();
	(void)fd;

	if (!parameters.empty() && parameters.compare(this->name)) {
		// Send ERR_NOSUCHSERVER 402
		return ;
	}
	// Send RPL 351 - client this->version this->name :comments but without comment
}

void	Server::getAdmin(User* user, std::string parameters) {
	int fd = user->getFd();
	(void)fd;

	if (!parameters.empty() && parameters.compare(this->name)) {
		// Send ERR_NOSUCHSERVER 402
		return ;
	}
	// Send RPL_ADMINME 256	
	// Send RPL_ADMINLOC1 257 with this->adminloc1
	// Send RPL_ADMINLOC2 258 with this->adminloc2	
	// Send RPL_ADMINEMAIL 259 with this->adminemail
}

void	Server::sendError (User* user, std::string parameter) {
	int fd = user->getFd();

	parameter.insert(0, "ERROR :");
	ssize_t bytes_send = send(fd, &parameter, parameter.size(), 0);
	if (bytes_send == -1) {
		std::cout << "Error: send()" << std::endl;
		return ;
	}
}

void	Server::sendPong (User* user, std::string parameter) {
	int fd = user->getFd();

	parameter.insert(0, " :");
	parameter.insert(0, this->name);
	parameter.insert(0, " PONG ");
	parameter.insert(0, this->name);
	parameter.insert(0, " :");
	ssize_t bytes_send = send(fd, &parameter, parameter.size(), 0);
	if (bytes_send == -1) {
		std::cout << "Error: send()" << std::endl;
		// TODO delete user ?
		return ;
	}
}

std::string	Server::getPass()
{
	return (password);
}

std::string	*Server::getVersionAddr()
{
	return (&version);
}

std::string	Server::getMotd()
{
	return (motd);
}

std::vector<User *>	Server::getServOp()
{
	return (operators);
}

std::vector<User *>	Server::getServUsers()
{
	return (users);
}
