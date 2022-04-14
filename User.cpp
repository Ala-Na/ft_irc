/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anadege <anadege@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 09:41:08 by cboutier          #+#    #+#             */
/*   Updated: 2022/04/14 01:49:06 by anadege          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

using namespace irc;

User::User(int fd, struct sockaddr_in address)
: _fd(fd)
{
	std::string	msg = "Please wait while we are connecting you...";
	send(fd, &msg, msg.size(), MSG_DONTWAIT);
	_address = address;
	userModes.set_a(false);		// user is flagged as away;
	userModes.set_i(false);		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
	userModes.set_w(false);		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
	userModes.set_r(false);		// restricted user connection;
	userModes.set_o(false);		// operator flag;
	userModes.set_O(false);
	// userModes.a = false;		// user is flagged as away;
	// userModes.i = false;		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
	// userModes.w = false;		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
	// userModes.r = false;		// restricted user connection;
	// userModes.o = false;		// operator flag;
	// userModes.O = false;
	_nickname = "";
	this->_status = PASS;
}

User::User(User const &src)
{
	*this = src;
}

User &User::operator=(User const &src)
{
	_nickname = src._nickname;
	_username = src._username;
	_real_name = src._real_name;
	_hostname = src._hostname;
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

void	User::sendMessage(int fd, std::string msg)
{
	send(fd, &msg, msg.size(), MSG_DONTWAIT);
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
	if (usr.userModes.get_a())
	{
		sendMessage(usr._fd, this->getAwayMessage());
		// send(usr._fd, this->getAwayMessage(), this->getAwayMessage().size())
		std::string reply = getNickname() + " :" + getAwayMessage();
	}
	sendMessage(this->_fd, msg);
	// send(this->_fd, &msg, msg.size());
}

void	User::notice(std::string msg)
{
	sendMessage(this->_fd, msg);
	// send(this->_fd, &msg, msg.size());
}

void	User::wallops(std::string msg) // pov de la pax qui recoit le msg, usr est la pax qui veut lui envoyer un msg
{
	if(userModes.get_w() == true)
		sendMessage(this->_fd, msg);
	// send(this->_fd, &msg, msg.size());
}

void	User::away(std::string msg)
{
	if (userModes.get_a())
	{
		userModes.set_a(false);
		std::string reply = "You are no longer marked as being away";
	}
	else
	{
		userModes.set_a(true);
		std::string reply = "You have been marked as being away";
		setAwayMessage(msg);
		send(this->_fd, &msg, msg.size(), MSG_DONTWAIT);
		// // numeric reply sent to client
		// std::string reply2 = getNickname() + " :" + msg;
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
	send(usr._fd, &combined, combined.size(), MSG_DONTWAIT);
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
	// for(int i = 0; i < param.size(); i++)
	// 	std::cout << i << ": " << param[i] << '\n';
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

