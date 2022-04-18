/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anadege <anadege@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 09:41:08 by cboutier          #+#    #+#             */
/*   Updated: 2022/04/18 16:33:42 by anadege          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

using namespace irc;

User::User()
{

}

User::User(int fd, std::string& hostname, struct sockaddr_in& address)
: _fd(fd)
{
	_address = address;
	_hostname = hostname;
	userModes.set_a(false);		// user is flagged as away;
	userModes.set_i(false);		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
	userModes.set_w(false);		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
	userModes.set_r(false);		// restricted user connection;
	userModes.set_o(false);		// operator flag;
	userModes.set_O(false);
	_nickname = "";
	_username = "";
	this->_status = PASS;
}


User::User(int fd, std::string& hostname, struct sockaddr_in& address, Server *server)
{
	_fd = fd;
	_server = server;
	_address = address;
	_hostname = hostname;
	// TODO recuperate hostname in address
	//_hostname = 
	userModes.set_a(false);		// user is flagged as away;
	userModes.set_i(false);		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
	userModes.set_w(false);		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
	userModes.set_r(false);		// restricted user connection;
	userModes.set_o(false);		// operator flag;
	userModes.set_O(false);
	_nickname = "";
	_username = ""; //  Par défaut oui mais le nom précis, il ne peut pas le deviner, faut que ça vienne du client !
	this->_status = PASS;
}

User::User(User const &src)
{
	*this = src;
}

User &User::operator=(User const &src)
{
	if (this != &src)
	{
		_server = src._server;
		_nickname = src._nickname;
		_username = src._username;
		_real_name = src._real_name;
		_hostname = src._hostname;
		_channels = src._channels;
		_params = src._params;
		_fd = src._fd;
		_address = src._address;
		_away_message = src._away_message;
		_status = src._status;
		userModes = src.userModes;
	}
	return (*this);
}

User::~User()
{}

// GETTERS
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

std::vector<std::string> User::getChannels()
{
	return (_channels);
}

sockaddr_in	User::getAddr()
{
	return (_address);
}

int	User::getNbOfChannels()
{
	std::vector<std::string>::iterator it = _channels.begin();
	int	counter = 0;
	while (it != _channels.end())
	{
		counter++;
		it++;
	}
	return (counter);
}

UserStatus	User::getStatus() {
	return this->_status;
}

// USERMODES GETTERS
bool	User::user_Modes::get_a()
{
	return (a);
}

bool	User::user_Modes::get_i()
{
	return (i);
}

bool	User::user_Modes::get_w()
{
	return (w);
}

bool	User::user_Modes::get_r()
{
	return (r);
}

bool	User::user_Modes::get_o()
{
	return (o);
}

bool	User::user_Modes::get_O()
{
	return (O);
}

// FLAGS BOOL
bool 	User::isAway()
{
	if (User::userModes.get_a())
		return (true);
	return (false);
}

bool 	User::isInvisible()
{
	if (User::userModes.get_i())
		return (true);
	return (false);
}

bool 	User::isWallops()
{
	if (User::userModes.get_w())
		return (true);
	return (false);
}

bool 	User::isRestricted()
{
	if (User::userModes.get_r())
		return (true);
	return (false);
}

bool 	User::isOperator()
{
	if (User::userModes.get_o())
		return (true);
	return (false);
}

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
void	User::user_Modes::set_a(bool val)
{
	a = val;
}

void	User::user_Modes::set_i(bool val)
{
	i = val;
}

void	User::user_Modes::set_w(bool val)
{
	w = val;
}

void	User::user_Modes::set_r(bool val)
{
	r = val;
}

void	User::user_Modes::set_o(bool val)
{
	o = val;
}

void	User::user_Modes::set_O(bool val)
{
	O = val;
}


// COMMANDS
void	User::nick(std::string nickname)
{
	setNickname(nickname);
}

void	User::privmsg(User usr, std::string msg) // pov de la pax qui recoit le msg, usr est la pax qui veut lui envoyer un msg
{
	int ret;
	
	if (usr.userModes.get_a())
	{
		ret = irc::sendString(usr._fd, this->getAwayMessage());
		if (ret == -1) {
			// TODO close connection
			return ;
		}
		std::string reply = getNickname() + " :" + getAwayMessage();
	}
	ret = irc::sendString(this->_fd, msg);
	if (ret == -1) {
		// TODO close connection
		return ;
	}

}

void	User::notice(std::string msg)
{
	int ret = irc::sendString(this->_fd, msg);
	if (ret == -1) {
		// TODO close connection
	}
}

void	User::wallops(std::string msg) // pov de la pax qui recoit le msg, usr est la pax qui veut lui envoyer un msg
{
	if(userModes.get_w() == true) {
		int ret = irc::sendString(this->_fd, msg);
		if (ret == -1) {
			// TODO close connection
		}
	}
}

void	User::away(std::string msg)
{
	int ret;
	std::vector<std::string> params;
	
	if (userModes.get_a())
	{
		userModes.set_a(false);
		ret = irc::numericReply(305, this, params);

	}
	else
	{
		userModes.set_a(true);
		setAwayMessage(msg);
		params.push_back(msg);
		ret = irc::numericReply(301, this, params);
	}
	if (ret == -1) {
		// TODO close connection
	}
}

void	User::addChannel(std::string const &chan)
{
	_channels.push_back(chan);
}

void	User::deleteChannel(std::string const &chan)
{
	std::vector<std::string>::iterator	it = std::find(_channels.begin(), _channels.end(), chan);
	if (it != _channels.end())
		_channels.erase(it);
}

Server *	User::getServer()
{
	return (_server);	
}

void	User::quit(void)
{
	// std::vector<std::string>::iterator it = _channels.begin();
	unsigned long int i = 0;
	while (i < _channels.size())
	{
		Channel *chan = getServer()->getChannelByName(_channels[i]);
		chan->deleteUser(*this, ""); // ??delete the user from the channel
		i++;
	}
}

// void	User::part(std::vector<std::string> params)
void	User::part()
{
	std::vector<std::string>::iterator it1 = _params.begin();
	while (it1 != _params.end())
	{
		std::vector<std::string>::iterator it2 = _channels.begin();
		while (*it2 != *it1 && it2 != _channels.end())
		{
			it2++;
		}
		if (*it1 == *it2)
		{
			Channel *chan = getServer()->getChannelByName(*it1);
			chan->deleteUser(*this, "");
		}
		it1++;
	}
}

void	User::kick(std::string const &chan)
{
	deleteChannel(chan);
}

void	User::whois(User usr)
{
	std::string username = "Username : ";
	std::string nickname = "Nickname : ";
	std::string real_name = "Real Name : ";
	std::string hostname = "Hostname : ";
	std::string combined = username + usr._username
							+ nickname + usr._nickname
							+ real_name + usr._real_name
							+ hostname + usr._hostname;
	// TODO use numericsReply for WHOIS
}

void	User::mode(std::vector<std::string> params)
{
	std::vector<std::string>::iterator it = params.begin();
	while (it != params.end())
	{
		if (it[0][0] == '+')
		{
			if (it[0][1] == 'i')
				userModes.set_i(true);
			if (it[0][1] == 'w')
				userModes.set_w(true);
			if (it[0][1] == 'r')
				userModes.set_r(true);
		}
		else if (it[0][0] == '-')
		{
			if (it[0][1] == 'i')
				userModes.set_i(false);
			if (it[0][1] == 'w')
				userModes.set_w(false);
			if (it[0][1] == 'r')
				userModes.set_r(false);
			if (it[0][1] == 'o')
				userModes.set_o(false);
			if (it[0][1] == 'O')
				userModes.set_O(false);
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
		userModes.set_w(true);
	bit = bit >> 1;
	if (bit &1)
		userModes.set_i(true);
	_real_name = params[3];
}

bool	User::operator==(User const &rhs) const
{
	if (this->_nickname == rhs._nickname
			&& this->_username == rhs._username && this->_real_name == rhs._real_name
			&& this->_hostname == rhs._hostname
			&& this->_fd == rhs._fd)
		return (1);
	return (0);
}

bool	User::operator!=(User const &rhs) const
{
	return (!(*this == rhs));
}

