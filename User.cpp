/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anadege <anadege@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 09:41:08 by cboutier          #+#    #+#             */
/*   Updated: 2022/04/13 23:09:22 by anadege          ###   ########.fr       */
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
	userModes.a = false;		// user is flagged as away;
	userModes.i = false;		// marks a users as invisible; hides you if someone does a /WHO or /NAMES outside the channel
	userModes.w = false;		// user receives wallops; Used by IRC operators, WALLOPS is a command utilized to send messages on an IRC network. WALLOPS messages are for broadcasting network information and its status to following users.
	userModes.r = false;		// restricted user connection;
	userModes.o = false;		// operator flag;
	userModes.O = false;
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

// FLAGS BOOL
bool 	User::isAway()
{
	if (User::userModes.a)
		return (true);
	return (false);
}

bool 	User::isInvisible()
{
	if (User::userModes.i)
		return (true);
	return (false);
}

bool 	User::isWallops()
{
	if (User::userModes.w)
		return (true);
	return (false);
}

bool 	User::isRestricted()
{
	if (User::userModes.r)
		return (true);
	return (false);
}

bool 	User::isOperator()
{
	if (User::userModes.o)
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


// COMMANDS
void	User::nick(std::string nickname)
{
	setNickname(nickname);
}

void	User::privmsg(User usr, std::string msg) // pov de la pax qui recoit le msg, usr est la pax qui veut lui envoyer un msg
{
	if (usr.userModes.a)
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
	if(userModes.w == true)
		sendMessage(this->_fd, msg);
	// send(this->_fd, &msg, msg.size());
}

void	User::away(std::string msg)
{
	if (userModes.a)
	{
		userModes.a = false;
		std::string reply = "You are no longer marked as being away";
	}
	else
	{
		userModes.a = true;
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
				userModes.i = true;
			if (it[0][1] == 'w')
				userModes.w = true;
			if (it[0][1] == 'r')
				userModes.r = true;
		}
		else if (it[0][0] == '-')
		{
			if (it[0][1] == 'i')
				userModes.i = false;
			if (it[0][1] == 'w')
				userModes.w = false;
			if (it[0][1] == 'r')
				userModes.r = false;
			if (it[0][1] == 'o')
				userModes.o = false;
			if (it[0][1] == 'O')
				userModes.O = false;
		}
		it++;
	}
}

void	User::user_cmd(std::string params)
{
	std::vector<std::string>	param;

	param = irc::split(params, " ");
	if (param.size() != 4)
		// send ERR NEEDMOREPARAMS 461
	// TODO: Check if user is already registered, if yes, send ERR ALREADYREGISTERED
	_nickname = param[0];
	
	int mask = std::atoi(param[1].c_str());
	int bit = 0;
	bit = mask >> 2; // bit shifting to the right
	if (bit &1) // check if 2nd bit is set, returns a 1 in each bit position for which the corresponding bits of both operands are 1s
		userModes.w = true;
	bit = bit >> 1;
	if (bit &1)
		userModes.i = true;
	_real_name = param[3];

	// add ERR_ALREADY_REGISTERED
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

