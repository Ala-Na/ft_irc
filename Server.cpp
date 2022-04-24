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
	for (std::vector<User *>::reverse_iterator it = this->users.rbegin(); it != this->users.rend(); it++) {
		this->deleteUser(*it, "Closing server");
	}
	for (std::vector<pollfd>::reverse_iterator it = pfds.rbegin(); it != pfds.rend(); it++) {
		close((*it).fd);
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
	char				host[1024];
	char 				service[20];

	addr_len = sizeof(client_addr);
	client_fd = accept(this->server_socket, (struct sockaddr *)&client_addr, &addr_len);
	if (this->users.size() == MAX_USERS) {
		std::string	refused = "Too many users on server, your connection was refused\r\n";
		irc::sendString(client_fd, refused);
		// No need to close connection if error with sendString, as User is not created
		// and fd is closed anyway
		std::cout << "A new connection was refused because there's already too many clients" << std::endl;
		close(client_fd);
		return ;
	}
	if (getnameinfo((const sockaddr *)&client_addr, sizeof(client_addr), \
		host, sizeof(host), service, sizeof(service), NI_NOFQDN) ==  -1) {
			std::cerr << "Error: getnameinfo()" << std::endl;
			close(client_fd);
			return ;
	}
	std::string hostname(host);
	this->addSocketToPoll(client_fd);
	std::cout << "Accepting new connection from " << inet_ntoa(client_addr.sin_addr) << " on fd: " << client_fd << std::endl;
	this->users.push_back(new User(client_fd, hostname, client_addr, this));
	this->datas.push_back("");
}

void	Server::deleteUser(User* user, std::string reason) {
	for (std::vector<User *>::iterator it1 = this->users.begin(); \
			it1 != this->users.end(); it1++) {
		if ((*it1) == user) {
			std::cout << "Closing connection to client fd = " << (*it1)->getFd() << "\n" << std::endl;
			this->deleteUserFromChannels(user, reason);
			this->deleteServOperator(user);
			this->sendError(user, reason);
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

void	Server::deleteUserFromChannels(User* user, std::string reason) {
	std::vector<Channel *>	chans = this->channels;

	for (size_t i = 0; i < chans.size(); i++) {
		if (chans[i]->userIsInChan(user) == true) {
			chans[i]->deleteChanUser(user, "PART " + chans[i]->getChanName() + " :" + reason);
		}
	}
}

Channel*	Server::createChannel(std::string name) {
	srand(time(0));
	if (name.find_first_of(" :,\007") != std::string::npos || name.find_first_of("#") != 0 || name.length() > 50) {
		this->channels.push_back(new Channel(this, "#"));
		return this->channels.back();
	}
	if (name[0] == '!')	{
		int					random;
		std::string			name;
		std::string 		s;
		std::stringstream	out;
		int					i;

		i = 0;
		while (i < 5) {
			random = rand() % 10;
			out << random;
			s = out.str();
			name.insert(0, s);
			i++;
		}
	}
	this->channels.push_back(new Channel(this, name));
	return this->channels.back();
}

int	Server::deleteChannel (Channel* channel) {
	if (channel->getNbUsersInChan() != 0) {
		return 0;
	}
	for (std::vector<Channel *>::iterator it1 = this->channels.begin(); \
			it1 != this->channels.end(); it1++) {
		if ((*it1) == channel) {
			delete *it1;
			this->channels.erase(it1);
			return 1;
		}
	}
	return 0;
}

void	Server::deleteEmptyChannels () {
	std::vector<Channel *>::iterator it = this->channels.begin();

	while (it != this->channels.end()) {
		if (this->deleteChannel(*it) == 1) {
			it = this->channels.begin();
		} else {
			it++;
		}
	}	
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
				this->deleteUser(*it1, "Fatal error");
				break ;
			} else {
				buf[bytes_recv] = 0;
				std::cout << "\n\n-----------------\n\nFrom client (fd = " << (*it).fd << "): " << buf << std::endl;
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
		std::cout << "\nCreating new command with: " << content << std::endl;
		Command* cmd = new Command(getServer(), user, content);
		cmd->parseCommand();
		delete cmd;
	}
}

// Here, user_nb is from 0 to max - 1.
User*	Server::getSpecificUser(size_t user_nb) {
	if (user_nb < this->users.size())
		return this->users[user_nb];
	return NULL;
}

Channel*	Server::getChannelByName(std::string name) {
	unsigned long	i;
	std::string		curr_chan;

	i = 0;
	name = irc::trim(name, " ");
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	while (i < channels.size())
	{
		curr_chan = channels[i]->getChanName();
		transform(curr_chan.begin(), curr_chan.end(), curr_chan.begin(), ::toupper);
		// std::cout << "chan_name to find: " << name << " - chan_names in vec: " << curr_chan << std::endl;
		if (curr_chan == name)
			return (channels[i]);
		i++;
	}
	return (NULL);
}

User*	Server::getUserByUsername(std::string name)
{
	unsigned long i;

	i = 0;
	while (i < users.size()) {
		if (users[i]->getUsername() == name) {
			return (users[i]);
		}
		i++;
	}
	return (NULL);
}

User*	Server::getUserByNick(std::string nick)
{
	unsigned long i;

	i = 0;
	while (i < users.size()) {
		if (users[i]->getNickname() == nick)
			return (users[i]);
		i++;
	}
	return (NULL);
}

std::string	Server::getPass() {
	return (password);
}

std::string	Server::getOpPass() {
	return ((this->conf.find("op_pwd"))->second);
}

std::string	Server::getVersion() {
	return (this->conf.find("version")->second);
}

std::vector<User *>	Server::getServOp() {
	return (operators);
}

std::vector<User *>	Server::getServUsers() {
	return (users);
}

std::string	Server::getName() {
	return ((this->conf.find("name"))->second);
}

Server&	Server::getServer() {
	return *this;
}

std::string	Server::getInfos() {
	return ((this->conf.find("infos"))->second);
}

int	Server::getMaxChannelbyUser() const {
	return 10;
}

void	Server::checkPassword(User* user, std::string parameters) {
	std::vector<std::string>	params;

	if (user->isRegistered() == true) {
		if (irc::numericReply(462, user, params) == -1) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	 } else if (parameters.empty()) {
		params.push_back("PASS");
		irc::numericReply(461, user, params);
		this->deleteUser(user, "Fatal error");
		return ;
	} else if (parameters.compare(this->password) != 0) {
		irc::numericReply(464, user, params);
		this->deleteUser(user, "Fatal error");
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
		if (irc::numericReply(431, user, params) == -1 || user->isRegistered() == false) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	}
	params = irc::split(parameters, " ", 0);
	if (this->getUserByNick(params[0]) != NULL) {
		if (irc::numericReply(433, user, params) == -1 || user->isRegistered() == false) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	} else if (params[0].size() > 8 ||
		(params[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789;[]\\`_^{}|") != std::string::npos) ||
		((params[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ;[]\\`_^{}|")) == 0))
	{
		if (irc::numericReply(432, user, params) == -1 || user->isRegistered() == false) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	}
	if (user->isRegistered() == false) {
		user->nick(params[0], false);
		user->setStatus(USER);
	} else {
		user->nick(params[0], true);
	}
}

void	Server::checkUserCmd(User* user, std::string parameters) {
	std::vector<std::string>	params;

	if (user->isRegistered() == true) {
		if (irc::numericReply(462, user, params) == -1) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	}
	params = irc::split(parameters, " ", 0);
	if (params.size() < 4) {
		params.clear();
		params.push_back("USER");
		irc::numericReply(461, user, params);
		this->deleteUser(user, "Fatal error");
		return ;
	}
	user->userCmd(params);
	user->setStatus(REGISTERED);
	this->welcomeUser(user);
}

void	Server::welcomeUser(User *user) {
	std::vector<std::string>	params;

	params.push_back(user->getUsername());
	params.push_back(user->getHostname());
	if (irc::numericReply(1, user, params) == -1) {
		this->deleteUser(user, "Fatal error");
		return ;
	}
	params.clear();
	params.push_back(this->conf.find("version")->second);
	if (irc::numericReply(2, user, params) == -1) {
		this->deleteUser(user, "Fatal error");
		return ;
	}
	params.clear();
	params.push_back(this->conf.find("creation")->second);
	if (irc::numericReply(3, user, params) == -1) {
		this->deleteUser(user, "Fatal error");
		return ;
	}
	params.clear();
	params.push_back(this->conf.find("version")->second);
	params.push_back("aiwro");
	params.push_back("oitklbI");
	if (irc::numericReply(4, user, params) == -1) {
		this->deleteUser(user, "Fatal error");
		return ;
	}
	this->getMotd(user, "");
}

void	Server::listChannels (User* user) {
	std::vector<std::string>	params;
	int							visible_users;
	char						buff[3];

	for (std::vector<Channel *>::iterator it = this->channels.begin(); it != this->channels.end(); it++) {
		params.push_back((*it)->getChanName());
		visible_users = (*it)->getNbUsersInChan();
		sprintf(buff, "%d", visible_users);
		params.push_back(buff);
		params.push_back((*it)->getChanTopic());
		irc::numericReply(322, user, params);
		params.clear();
	}
}

void	Server::getMotd(User* user, std::string parameters) {
	std::vector<std::string> param = irc::split(parameters, " ", 0);
	std::string motd = (this->conf.find("motd"))->second;

	if (!param[0].empty() && param[0].compare((this->conf.find("name"))->second)) {
		if (irc::numericReply(402, user, param) == -1) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	} else if (motd.empty()) {
		if (irc::numericReply(422, user, param) == -1) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	}
	if (irc::numericReply(375, user, param) == -1) {
		this->deleteUser(user, "Fatal error");
		return ;
	}
	param.clear();
	for (size_t i = 0; i < motd.size(); i += 80) {
    	param.push_back(motd.substr(i, 80));
		if (irc::numericReply(372, user, param) == -1) {
			this->deleteUser(user, "Fatal error");
			return ;
		}
		param.clear();
	}
	if (irc::numericReply(376, user, param) == -1) {
		this->deleteUser(user, "Fatal error");
	}
}

void	Server::retrieveTime(User* user, std::string parameters) {
	std::string name = this->getName();

	if (!parameters.empty() && parameters.compare(name)) {
		std::vector<std::string> params;
		params.push_back(name);
		if (irc::numericReply(402, user, params) == -1) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	}
	std::time_t time;
	std::time(&time);
	std::string s_time = std::asctime(std::localtime(&time));
	std::vector<std::string> params;
	params.push_back(name);
	params.push_back(s_time);
	if (irc::numericReply(391, user, params) == -1) {
		this->deleteUser(user, "Fatal error");
	}
	return ;
}

void	Server::retrieveVersion(User* user, std::string parameters) {
	std::string name = this->getName();
	std::vector<std::string> params;

	if (!parameters.empty() && parameters.compare(name)) {
		params.push_back(name);
		if (irc::numericReply(402, user, params) == -1) {
			this->deleteUser(user, "Fatal error");
		}
		return ;
	}
	params.push_back(this->conf.find("version")->second);
	params.push_back("");
	params.push_back("");
	if (irc::numericReply(351, user, params) == -1) {
		this->deleteUser(user, "Fatal error");
	}
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
	parameter += "\r\n";
	irc::sendString(fd, parameter);
}

void	Server::sendPong (User* user, std::string server_name) {
	std::vector<std::string>	params;
	int							ret;
	std::string 				name = this->conf.find("name")->second;

	if (server_name.empty()) {
		ret = irc::numericReply(409, user, params);
	} else if (server_name != name) {
		params.push_back(server_name);
		ret = irc::numericReply(402, user, params);
	} else {
		server_name.insert(0, " :");
		server_name.insert(0, name);
		server_name.insert(0, " PONG ");
		server_name.insert(0, name);
		server_name.insert(0, ":");
		server_name += "\r\n";
		ret = irc::sendString(user->getFd(), server_name);
	}
	if (ret == -1) {
		this->deleteUser(user, "Fatal error");
	}
}

int	Server::setServOperator(User*	user) {
	std::vector<User *>::iterator it = std::find(this->operators.begin(), this->operators.end(), user);
	if (it == this->operators.end()) {
		this->operators.push_back(user);
		user->set_o(true);
	}
	std::vector<std::string> params;
	if (irc::numericReply(381, user, params) == -1) {
		return (-1);
	}
	return 0;

}

void	Server::deleteServOperator(User* user) {
	std::vector<User *>::iterator it = std::find(this->operators.begin(), this->operators.end(), user);
	if (it != operators.end()) {
		this->operators.erase(it);
	}
}

bool	Server::isServOperator(User *user) {
	std::vector<User *>::iterator it = std::find(this->operators.begin(), this->operators.end(), user);
	if (it != operators.end()) {
		return true;
	}
	return false;
}

void	Server::sendWallops(std::string msg) {
	std::string	pref_msg = ":" + this->getName();
	std::vector<User *>::iterator	it = this->users.begin();
	while (it != this->users.end())
	{
		std::string full_msg = pref_msg + " NOTICE " +(*it)->getNickname() + " :" + msg + "\r\n";
		if ((*it)->get_w()) {
			irc::sendString((*it)->getFd(), full_msg);
		}
		it++;
	}
}
