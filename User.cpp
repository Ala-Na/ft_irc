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
	userModes_O = false;
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

void	User::set_O(bool val)
{
	userModes_O = val;
}

void	User::addChannel(Channel* chan)
{
	_channels.push_back(chan);
}

void	User::deleteChannel(Channel* chan)
{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it) == chan) {
			_channels.erase(it);
			return ;
		}
	}
}

// COMMANDS
void	User::nick(std::string nickname)
{
	setNickname(nickname);
}

void	User::privmsgToUser(User* dest, std::string msg) // pov de la pax qui recoit le msg, usr est la pax qui veut lui envoyer un msg
{
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
	channel->writeToAllChanUsers(full_msg);
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
	channel->writeToAllChanUsers(full_msg);
}

void	User::receiveWallops(std::string msg) {
	std::string full_msg = ":" + this->_server->getName() + " WALLOPS :" + msg + "\r\n";
	if(get_w() == true) {
		irc::sendString(this->_fd, full_msg);
	}
}

int	User::away(std::string msg)
{
	int ret;
	std::vector<std::string> params;

	if (get_a())
	{
		set_a(false);
		ret = irc::numericReply(305, this, params);
	}
	else
	{
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

void	User::whois(User* who)
{
	std::vector<std::string>	params;

	params.push_back(who->getNickname());
	params.push_back(who->getUsername());
	params.push_back(who->getHostname());
	params.push_back(who->getRealName());
	if (irc::numericReply(311, this, params) == -1) {
		return (this->_server->deleteUser(this));
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
		return (this->_server->deleteUser(this));
	}
	params.clear();
	params.push_back(this->_server->getName());
	params.push_back(this->_server->getInfos());
	if (irc::numericReply(312, this, params) == -1) {
		return (this->_server->deleteUser(this));
	}
	params.clear();
	if (this->get_a() == true) {
		params.push_back(this->_nickname);
		params.push_back(this->_away_message);
		if (irc::numericReply(301, this, params) == -1) {
			return (this->_server->deleteUser(this));
		}	
		params.clear();	
	}
	if (this->_server->isServOperator(this) == true) {
		params.push_back(this->_nickname);
		if (irc::numericReply(313, this, params) == -1) {
			return (this->_server->deleteUser(this));
		}	
	}
	if (irc::numericReply(318, this, params) == -1) {
		return (this->_server->deleteUser(this));
	}		
}

//TODO continue check following
void	User::mode(std::vector<std::string> params)
{
	std::vector<std::string>::iterator it = params.begin();
	while (it != params.end())
	{
		if (it[0][0] == '+')
		{
			if (it[0][1] == 'i')
				set_i(true);
			if (it[0][1] == 'w')
				set_w(true);
			if (it[0][1] == 'r')
				set_r(true);
		}
		else if (it[0][0] == '-')
		{
			if (it[0][1] == 'i')
				set_i(false);
			if (it[0][1] == 'w')
				set_w(false);
			if (it[0][1] == 'r')
				set_r(false);
			if (it[0][1] == 'o')
				set_o(false);
			if (it[0][1] == 'O')
				set_O(false);
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

