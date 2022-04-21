#include "User.hpp"

using namespace irc;

User::User(int fd, std::string& hostname, struct sockaddr_in& address, Server *server)
{
	_fd = fd;
	_server = server;
	_address = address;
	_hostname = hostname;
	userModes_a = false;		// user is flagged as away;
	userModes_i = false;		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
	userModes_w = false;		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
	userModes_r = false;		// restricted user connection;
	userModes_o = false;		// operator flag;
	_nickname = "";
	_username = ""; //  Par défaut oui mais le nom précis, il ne peut pas le deviner, faut que ça vienne du client !
	this->_status = PASS;
}

User::~User() {}

// GETTERS
Server *	User::getServer()
{
	return (_server);	
}

std::string	User::getNickname()
{
	return (_nickname);
}

std::string	User::getUsername()
{
	return (_username);
}

std::string	User::getRealName()
{
	return (_real_name);
}

std::string	User::getHostname()
{
	return (_hostname);
}

std::vector<std::string>	User::getParams()
{
	return (_params);
}

std::string User::getAwayMessage()
{
	return (_away_message);
}

int		User::getFd()
{
	return (_fd);
}

sockaddr_in	User::getAddr()
{
	return (_address);
}

std::vector<Channel *> User::getChannels()
{
	return (_channels);
}

int	User::getNbOfChannels()
{
	return (_channels.size());
}

UserStatus	User::getStatus() {
	return this->_status;
}

// USERMODES GETTERS
bool	User::get_a()
{
	return (userModes_a);
}

bool	User::get_i()
{
	return (userModes_i);
}

bool	User::get_w()
{
	return (userModes_w);
}

bool	User::get_r()
{
	return (userModes_r);
}

bool	User::get_o()
{
	return (userModes_o);
}

std::string	User::getModesString() {
	std::string	modes = "";
	if (this->get_a() == true) {
		modes += "a";
	}
	if (this->get_i() == true) {
		modes += "i";
	} 
	if (this->get_w() == true) {
		modes += "w";
	} 
	if (this->get_o() == true) {
		modes += "o";
	} 
	if (this->get_r() == true) {
		modes += "r";
	} 		
	if (!modes.empty()) {
		modes.insert(modes.begin(), '+');
	}
	return modes;
}

// Get status
bool	User::isRegistered()
{
	if (this->_status == REGISTERED)
		return (true);
	return (false);
}

// SETTERS
void	User::setNickname(std::string nickname)
{
	_nickname = nickname;
}

void	User::setUsername(std::string username)
{
	_username = username;
}

void	User::setRealName(std::string real_name)
{
	_real_name = real_name;
}

void	User::setHostname(std::string hostname)
{
	_hostname = hostname;
}

void	User::setAwayMessage(std::string msg)
{
	_away_message = msg;
}

void	User::setParams(std::vector<std::string> params)
{
	_params = params;
}

void	User::setStatus(UserStatus status) {
	this->_status = status;
}

// USERMODES SETTERS
void	User::set_a(bool val)
{
	userModes_a = val;
}

void	User::set_i(bool val)
{
	userModes_i = val;
}

void	User::set_w(bool val)
{
	userModes_w = val;
}

void	User::set_r(bool val)
{
	userModes_r = val;
}

void	User::set_o(bool val)
{
	userModes_o = val;
}

void	User::addChannel(Channel* chan)
{
	_channels.push_back(chan);
}

void	User::deleteChannel(Channel* chan)
{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it) == chan) {
			// std::cout << "IN DELETECHANNEL: " << chan->getChanName() << std::endl;
			_channels.erase(it);
			return ;
		}
	}
}

// COMMANDS
void	User::nick(std::string nickname, bool send_msg) {
	std::string	nick_msg;

	setNickname(nickname);
	if (send_msg == true) {
		nick_msg = ":" + this->_nickname + "!" + this->_username + "@" + this->_hostname;
		nick_msg += " NICK :" + nickname + "\r\n";
		if (irc::sendString(this->getFd(), nick_msg) == -1) {
			return (this->_server->deleteUser(this));
		}	
	}
}

void	User::privmsgToUser(User* dest, std::string msg) // pov de la pax qui recoit le msg, usr est la pax qui veut lui envoyer un msg
{
	if (!dest)
		return ;
	std::string	full_msg = ":" + this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname();
	full_msg += " PRIVMSG " + dest->getNickname() + " :" + msg + "\r\n";
	int ret = irc::sendString(dest->getFd(), full_msg);
	if (ret == -1) {
		return (this->_server->deleteUser(this));
	}
	if (dest->get_a())
	{
		std::vector<std::string>	params;
		params.push_back(dest->getNickname());
		params.push_back(dest->getAwayMessage());
		if (irc::numericReply(301, this, params) == -1) {
			return (this->_server->deleteUser(this));
		}
	}
}

void	User::privmsgToChannel(Channel* channel, std::string msg) {
	std::string	full_msg = ":" + this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname();
	full_msg += " PRIVMSG " + channel->getChanName() + " :" + msg + "\r\n";
	channel->writeToAllChanUsers(full_msg, this);
}

void	User::noticeToUser(User* dest, std::string msg) {
	std::string	full_msg = ":" + this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname();
	full_msg += " NOTICE " + dest->getNickname() + " :" + msg + "\r\n";
	int ret = irc::sendString(dest->getFd(), full_msg);
	if (ret == -1) {
		return (this->_server->deleteUser(this));
	}
}

void	User::noticeToChannel(Channel* channel, std::string msg) {
	std::string	full_msg = ":" + this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname();
	full_msg += " NOTICE " + channel->getChanName() + " :" + msg + "\r\n";
	channel->writeToAllChanUsers(full_msg, this);
}

void	User::receiveWallops(std::string msg) {
	std::string full_msg = ":" + this->_server->getName() + " WALLOPS :" + msg + "\r\n";
	if(get_w() == true) {
		irc::sendString(this->_fd, full_msg);
	}
}

int	User::away(std::string msg)
{
	int 					ret;
	std::vector<std::string> params;

	if (msg.empty()) {
		set_a(false);
		ret = irc::numericReply(305, this, params);
	} else {
		set_a(true);
		setAwayMessage(msg);
		params.push_back(msg);
		ret = irc::numericReply(301, this, params);
	}
	return ret;
}

void	User::quit(std::string reason) {
	std::vector<Channel *>	channels = this->_channels;

	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++) {
		this->partChannel(*it, reason);
	}
}

void	User::partChannel(Channel* channel, std::string reason) {
	channel->deleteUser(this, " PART " + channel->getChanName() + " :" + reason);
}

void	User::kick(Channel* chan, std::string reason)
{
	if (reason.empty()) {
		reason = "no reason";
	}
	chan->deleteUser(this, " KICK " + chan->getChanName() + _nickname + " :" + reason);
}

int	User::whois(User* who)
{
	std::vector<std::string>	params;

	params.push_back(who->getNickname());
	params.push_back(who->getUsername());
	params.push_back(who->getHostname());
	params.push_back(who->getRealName());
	if (irc::numericReply(311, this, params) == -1) {
		return (-1);
	}
	params.clear();
	params.push_back(who->getNickname());
	std::string channels;
	for (std::vector<Channel *>::iterator it = this->_channels.begin(); it != _channels.end(); it++) {
		if ((*it)->isOperator(this) == 1) {
			channels += "@";
		}
		channels += (*it)->getChanName() + " ";
	}
	params.push_back(channels);
	std::cout << "here 3" << std::endl;
	if (irc::numericReply(319, this, params) == -1) {
		return (-1);
	}
	params.clear();
	params.push_back(this->_server->getName());
	params.push_back(this->_server->getInfos());
	if (irc::numericReply(312, this, params) == -1) {
		return (-1);
	}
	params.clear();
	if (this->get_a() == true) {
		params.push_back(this->_nickname);
		params.push_back(this->_away_message);
		if (irc::numericReply(301, this, params) == -1) {
			return (-1);
		}	
		params.clear();	
	}
	if (this->_server->isServOperator(this) == true) {
		params.push_back(this->_nickname);
		if (irc::numericReply(313, this, params) == -1) {
			return (-1);
		}	
	}
	if (irc::numericReply(318, this, params) == -1) {
		return (-1);
	}	
	return 0;	
}

void	User::sendMode(User* ope, std::string mode_msg) {
	mode_msg += "\r\n";
	sendString(this->getFd(), mode_msg);
	if (this != ope) {
		sendString(ope->getFd(), mode_msg);
	}
}

// this = changing user
// ope = operator who change this user mode; can be operator or same user than this
void	User::mode(User* ope, std::string new_modes) {
	std::vector<std::string>	params;

	if (new_modes.empty()) {
		params.push_back(this->getModesString());
		if (irc::numericReply(221, ope, params) == -1) {
			return (ope->getServer()->deleteUser(ope));
		}
	}

	std::string::iterator	it = new_modes.begin();
	std::string				mode_msg = ":" + ope->getNickname() + " MODE " + this->getNickname() + " :";
	bool					plus = true;
	std::string				valid = "+-iwroa";

	while (it != new_modes.end()) {
		if (*it == '+') {
			plus = true;
		} else if (*it == 'i' && plus == true && get_i() == false) {
			set_i(true);
			sendMode(ope, mode_msg + "+i");
		} else if (*it == 'w' && plus == true && get_w() == false) {
			set_w(true);
			sendMode(ope, mode_msg + "+w");
		} else if (*it == 'r' && plus == true && get_r() == false) {
			set_r(true);
			sendMode(ope, mode_msg + "+r");
		} else if (*it == '-') {
			plus = false;
		} else if (*it == 'i' && plus == false && get_i() == true) {
			set_i(false);
			sendMode(ope, mode_msg + "-i");
		} else if (*it == 'w' && plus == false && get_w() == true) {
			set_w(false);
			sendMode(ope, mode_msg + "-w");
		} else if (*it == 'r' && plus == false && get_r() == true) {
			set_r(false);
			sendMode(ope, mode_msg + "-r");
		} else if (*it == 'o' && plus == false && get_o() == true) {
			set_o(false);
			sendMode(ope, mode_msg + "-o");
		} else if (valid.find_first_of(*it)) {
			params.push_back(std::string(*it, 1));
			irc::numericReply(472, ope, params);
			params.clear();
		}
		it++;
	}
}

void	User::userCmd(std::vector<std::string>& params)
{
	this->setUsername(params[0]);
	int mask = std::atoi(params[1].c_str());
	int bit = 0;
	bit = mask >> 2; // bit shifting to the right
	if (bit &1) // check if 2nd bit is set, returns a 1 in each bit position for which the corresponding bits of both operands are 1s
		set_w(true);
	bit = bit >> 1;
	if (bit &1)
		set_i(true);
	_real_name = params[3];
}

bool	User::operator==(User const &rhs) const
{
	if (this->_nickname == rhs._nickname
			&& this->_username == rhs._username && this->_real_name == rhs._real_name
			&& this->_hostname == rhs._hostname
			&& this->_fd == rhs._fd)
		return (true);
	return (false);
}

bool	User::operator!=(User const &rhs) const
{
	return (!(*this == rhs));
}

