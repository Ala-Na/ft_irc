#include "Server.hpp"

#define MAX_USERS 10

using namespace irc;

Server::Server(std::string password, const char* port) : password(password), port(port) {
	std::cout << "Initializing server..." << std::endl;
	this->users = std::vector<User *>();
	this->channels = std::vector<Channel *>();
	this->pfds = std::vector<pollfd>();
	this->conf = std::map<std::string, std::string>();
	this->operators = std::vector<User *>();
}

Server::~Server() {
	std::cout << "Closing server..." << std::endl;
	for (std::vector<pollfd>::reverse_iterator it = pfds.rbegin(); it != pfds.rend(); it++) {
		close((*it).fd);
	}
	for (std::vector<User *>::reverse_iterator it = this->users.rbegin(); it != this->users.rend(); it++) {
		delete *it;
	}
	for (std::vector<Channel *>::reverse_iterator it = this->channels.rbegin(); it != this->channels.rend(); it++) {
		delete *it;
	}
}

int	Server::readConfFile() {
	std::ifstream	in;
	int				res = 0;
	std::string		line;
	std::string		key;
	std::string		value = "";
    size_t			found;

	in.open("./ft_irc.conf", std::ifstream::in);
	if (!in.good()) {
		std::cerr << "Can't open configuration file" << std::endl;
		std::cerr << "Please check that ./ft_irc.conf is present in Server folder" << std::endl; 
		return (-1);
	}
	while (!in.eof() && in.good()) {
        value = "";
		std::getline(in, line);
        if ((found = line.find_first_not_of(' ')) != std::string::npos) {
        	line.erase(0, found);
		}
        if (line.empty() || line[found] == '#') {
            continue;
        } else if ((found = line.find("=\"")) == std::string::npos) {
			std::cerr << "Should be under format key=\"value\"" << std::endl;
			res = -1;
			break;
		}
		key = line.substr(0, found);
		line.erase(0, found + 2);
		while ((found = line.find('"')) == std::string::npos) {
			value.append(line);
			std::getline(in, line);
			value.append(" ");
		}
		value += line.substr(0, found);
		line.erase(0, found + 1);
		if (!line.empty()) {
			std::cerr << "Error in configuration file" << std::endl;
			std::cerr << "key=\"value\" should be followed by a line return" << std::endl;
			res = -1;
			break;
		}
		this->conf.insert(std::make_pair(key, value));
	}
	if (!in.eof()) {
		std::cerr << "Error while reading configuration file" << std::endl;
		res = -1;
	}
	in.close();
	return res;
}

int	Server::checkConf() {
	if (this->conf.count("name") == 0 || this->conf.count("version") == 0
		|| this->conf.count("adminloc1") == 0 || this->conf.count("adminemail") == 0
		|| this->conf.count("creation") == 0) {
		std::cerr << "Missing at least one non-optionnal configuration parameter" << std::endl;
		return (-1);
	}
	if ((this->getName()).size() > 63) {
		std::cerr << "Server name should be under 63 characters (please, modify .conf file)" << std::endl;
		return (-1);
	}
	if (this->conf.count("op_pwd") == 0) {
		this->conf.insert(std::make_pair("op_pwd", "admin"));
	}
	return 0;
}

int	Server::initServer() {
	struct addrinfo	hints;
	struct addrinfo	*ai;
	struct addrinfo	*p;
	int	server_fd;
	int	opt = 1;
	int res;

	if (this->readConfFile() == -1 || this->checkConf() == -1) {
		return (-1);
	}

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
				// std::cout << "UUUUUUUUSER1: " << users[0]->getUsername() << std::endl;

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

void	Server::createUser() {
	struct sockaddr_in	client_addr;
	socklen_t			addr_len;
	int					client_fd;

	addr_len = sizeof(client_addr);
	client_fd = accept(this->server_socket, (struct sockaddr *)&client_addr, &addr_len);
	if (this->users.size() == MAX_USERS) {
		std::string	refused = "Too many users on server, your connection was refused";
		irc::sendString(client_fd, refused);
		std::cout << "A new connection was refused because there's already too many clients" << std::endl;
		close(client_fd);
		return ;
	}
	this->addSocketToPoll(client_fd);
	std::cout << "Accepting new connection from " << inet_ntoa(client_addr.sin_addr) << " on fd: " << client_fd << std::endl;
	this->users.push_back(new User(client_fd, client_addr, this));
	this->datas.push_back("");
}

void	Server::deleteUser(User* user) {
	for (std::vector<User *>::iterator it1 = this->users.begin(); \
			it1 != this->users.end(); it1++) {
		if ((*it1) == user) {
			std::cout << "Closing connection to client fd = " << (*it1)->getFd() << "\n" << std::endl;
			this->deleteUserFromChannels(user);
			this->deleteServOperator(user);
			int position = it1 - this->users.begin();
			std::vector<std::string>::iterator it2 = this->datas.begin();
			std::advance(it2, position);
			this->datas.erase(it2);
			std::vector<pollfd>::iterator it3 = this->pfds.begin();
			std::advance(it3, position + 1);
			close((*it3).fd);
			this->pfds.erase(it3);
			delete *it1;
			this->users.erase(it1);
			return ;
		}
	}
}

void	Server::deleteUserFromChannels(User* user) {
	unsigned long i = 0;
	while (i < channels.size())
	{
		if (channels[i]->userIsInChanFromUsername(user->getUsername()))
		{
			channels[i]->deleteUser(*user, "");
		}
		i++;
	}
	return ;
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

void	Server::receiveDatas() {
	char		buf[SERV_BUF_SIZE + 1];
	std::string	s_buf;

	for (std::vector<pollfd>::iterator it = pfds.begin() + 1; it != pfds.end(); ++it) {
		if ((*it).revents & POLLIN) {
			ssize_t	bytes_recv = recv((*it).fd, &buf, SERV_BUF_SIZE, 0);
			if (bytes_recv <= 0) {
				if (bytes_recv == -1) {
					std::cerr << "Error : recv()" << std::endl;
				} else {
					std::cout << "Client " << (*it).fd << " exited" << std::endl;
				}
				int	position = it - this->pfds.begin();
				std::vector<User *>::iterator it1 = this->users.begin();
				std::advance(it1, position - 1);
				this->deleteUser(*it1);
				break ;
			} else {
				buf[bytes_recv] = 0;
				std::cout << "From client (fd = " << (*it).fd << "): " << buf << std::endl;
				s_buf = buf;
				this->datasExtraction(s_buf, it - pfds.begin() - 1);
				break ;
			}
		}
	}
}

void	Server::datasExtraction(std::string& buf, size_t pos) {
	User *user = this->getSpecificUser(pos);
	size_t	nbr_us = datas.size();

	datas[pos].append(buf);
	size_t cmd_end = datas[pos].find("\r\n");
	while (datas.size() == nbr_us && cmd_end != std::string::npos) {
		std::string	content = datas[pos].substr(0, cmd_end);
		if (cmd_end + 2 >= datas[pos].size()) {
			datas[pos].clear();
			cmd_end = std::string::npos;
		} else {
			datas[pos].erase(0, cmd_end + 2);
			cmd_end = datas[pos].find("\r\n");
		}
		std::cout << "Creating new command with: " << content << std::endl;
		Command* cmd = new Command(getServer(), user, content);
		cmd->parseCommand();
		delete cmd;
	}
}

std::string	Server::getName()
{
	return ((this->conf.find("name"))->second);
}

Server&	Server::getServer() {
	return *this;
}

// Here, user_nb is from 0 to max - 1.
User*	Server::getSpecificUser(size_t user_nb) {
	// std::cout << "nb : " << user_nb << std::endl;
	// std::cout << "users size : " << users.size() << std::endl;
	if (user_nb < this->users.size())
		return this->users[user_nb];
	return NULL;
}

Channel*	Server::getChannelByName(std::string name) {
	for (std::vector<Channel *>::iterator it = this->channels.begin(); \
		it != this->channels.end(); it++)
	{
		// TODO
		// Check if (*it)->getname() == name;
		// If true, return channel;
		(void)name;
	}
	return NULL;
}

User*	Server::getUserByUsername(std::string name)
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

User*	Server::getUserByNick(std::string nick)
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
	std::vector<std::string>	params;

	if (user->isRegistered() == true) {
		irc::numericReply(462, user, params);
		return ;
	 } else if (parameters.empty()) {
		params.push_back("PASS");
		irc::numericReply(461, user, params);
		this->deleteUser(user);
		return ;
	} else if (parameters.compare(this->password) != 0) {
		irc::numericReply(464, user, params);
		this->deleteUser(user);
		return ;
	}
	user->setStatus(NICK);
}

void	Server::checkNick(User* user, std::string parameters) {
	std::vector<std::string>	params;

	if (user->getStatus() == PASS) {
		this->checkPassword(user, "");
		return ;
	} else if (parameters.empty()) {
		irc::numericReply(431, user, params);
		if (user->isRegistered() == false)
			this->deleteUser(user);
		return ;
	}
	params = irc::split(parameters, " ");
	if (this->getUserByNick(params[0]) != NULL) {
		irc::numericReply(433, user, params);
		if (user->isRegistered() == false)
			this->deleteUser(user);
		return ;
	} else if (params[0].size() > 8 ||
		(params[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789;[]\\`_^{}|") != std::string::npos) ||
		((params[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ;[]\\`_^{}|")) == 0))
	{
			irc::numericReply(432, user, params);
			if (user->isRegistered() == false)
				this->deleteUser(user);
			return ;
	}
	user->nick(params[0]);
	if (user->isRegistered() == false)
		user->setStatus(USER);
	// We don't set the server as restricted so no ERR_RESTRICTED 484
}

void	Server::checkUserCmd(User* user, std::string parameters) {
	std::vector<std::string>	params;

	if (user->isRegistered() == true) {
		irc::numericReply(462, user, params);
		return ;
	}
	params = irc::split(parameters, " ");
	if (params.size() < 4) {
		params.clear();
		params.push_back("USER");
		irc::numericReply(461, user, params);
		this->deleteUser(user);
		return ;
	}
	user->userCmd(params);
	user->setStatus(REGISTERED);
	this->welcomeUser(user);
}

void	Server::welcomeUser(User *user) {
	std::vector<std::string>	params;

	std::cout << "In welcome" << std::endl;
	params.push_back(user->getUsername());
	params.push_back(user->getHostname());
	irc::numericReply(1, user, params);
	params.clear();
	params.push_back(this->conf.find("version")->second);
	irc::numericReply(2, user, params);
	params.clear();
	params.push_back(this->conf.find("creation")->second);
	irc::numericReply(3, user, params);	
	params.push_back(this->conf.find("version")->second);
	params.push_back("aiwroO");
	// TODO modify following
	params.push_back("channel modes availables");
	irc::numericReply(4, user, params);
	this->getMotd(user, "");
}

void	Server::listChannels (User* user) {
	(void)user;
	// Maybe send 321 and 323 from intList ?
	// Send RPL_liststart 321
	for (std::vector<Channel *>::iterator it = this->channels.begin(); it != this->channels.end(); it++) {
		//(*it).getChanName()
		// Response must be : '322 nickname channelname nb_user_in_channel: topic'
		// Maybe do call to function ?
	}
	// Send RPL_LISTEND 323
}

void	Server::getMotd(User* user, std::string parameters) {
	std::vector<std::string> param = irc::split(parameters, " ");
	std::string motd = (this->conf.find("motd"))->second;

	if (!param[0].empty() && param[0].compare((this->conf.find("name"))->second)) {
		irc::numericReply(402, user, param);
		return ;
	} else if (motd.empty()) {
		irc::numericReply(422, user, param);
		return ;
	}
	irc::numericReply(375, user, param);
	param.clear();
	for (size_t i = 0; i < motd.size(); i += 80) {
    	param.push_back(motd.substr(i, 80));
		irc::numericReply(372, user, param);
		param.clear();	
	}
	irc::numericReply(376, user, param);
}

void	Server::retrieveTime(User* user, std::string parameters) {
	std::string name = this->getName();

	if (!parameters.empty() && parameters.compare(name)) {
		std::vector<std::string> params;
		params.push_back(name);
		irc::numericReply(402, user, params);
		return ;
	}
	std::time_t time;
	std::time(&time);
	std::string s_time = std::asctime(std::localtime(&time));
	std::vector<std::string> params;
	params.push_back(name);
	params.push_back(s_time);
	irc::numericReply(391, user, params);
}

void	Server::retrieveVersion(User* user, std::string parameters) {
	std::string name = this->getName();
	std::vector<std::string> params;

	if (!parameters.empty() && parameters.compare(name)) {
		params.push_back(name);
		irc::numericReply(402, user, params);
		return ;
	}
	params.push_back(this->conf.find("version")->second);
	params.push_back("");
	params.push_back("");
	irc::numericReply(351, user, params);
}

void	Server::getAdmin(User* user, std::string parameters) {
	std::string name = this->getName();
	std::vector<std::string> params;

	if (!parameters.empty() && parameters.compare(name)) {
		params.push_back(name);
		irc::numericReply(402, user, params);
		return ;
	}
	irc::numericReply(256, user, params);
	params.push_back(this->conf.find("adminloc1")->second);
	irc::numericReply(257, user, params);
	params.clear();
	params.push_back(this->conf.find("adminloc2")->second);
	irc::numericReply(258, user, params);
	params.clear();
	params.push_back(this->conf.find("adminemail")->second);
	irc::numericReply(259, user, params);
}

void	Server::sendError (User* user, std::string parameter) {
	int fd = user->getFd();

	parameter.insert(0, "ERROR :");
	int res = irc::sendString(fd, parameter);
	if (res == -1) {
		std::cout << "Error: send()" << std::endl;
		this->deleteUser(user);
	}
}

void	Server::sendPong (User* user, std::string parameter) {
	int	fd = user->getFd();
	std::string name = this->conf.find("name")->second;

	parameter.insert(0, " :");
	parameter.insert(0, name);
	parameter.insert(0, " PONG ");
	parameter.insert(0, name);
	parameter.insert(0, " :");
	int ret = irc::sendString(fd, parameter);
	if (ret == -1) {
		std::cout << "Error: send()" << std::endl;
		this->deleteUser(user);
	}
}

std::string	Server::getPass()
{
	return (password);
}

std::string	Server::getVersion()
{
	return (this->conf.find("version")->second);
}

std::vector<User *>	Server::getServOp()
{
	return (operators);
}

std::vector<User *>	Server::getServUsers()
{
	return (users);
}

void	Server::setServOperator(User*	user) {
	std::vector<User *>::iterator it = std::find(this->operators.begin(), this->operators.end(), user);

	if (it == this->operators.end()) {
		this->operators.push_back(user);
	}
	std::vector<std::string> params;
	irc::numericReply(381, user, params);

}

void	Server::deleteServOperator(User* user) {
	std::vector<User *>::iterator it = std::find(this->operators.begin(), this->operators.end(), user);

	if (it != operators.end()) {
		this->operators.erase(it);
	}		
}
