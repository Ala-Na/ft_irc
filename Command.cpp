#include "Command.hpp"

using namespace irc;

std::string Command::getParam()
{
	return (param);
}

Command::Command (Server& server, User* user, std::string& content) : server(server), user(user), content(content) {
	this->prefix = "";
}

// TODO check if memory is allocated and need to be free
Command::~Command () {}

void	Command::parseCommand () {
	std::string	first_word = this->getWord();
	if (first_word.empty()) {
		this->prefix = this->content;
		this->content.clear();
	} else {
		this->prefix = first_word;
	}
	std::cout << this->prefix << std::endl;
	ft_toupper_str(this->prefix);
	this->param = this->content;
	std::cout << this->param << std::endl;
	this->goToExecution();

}

std::string	Command::getWord () {
	std::string word = "";

	size_t word_end = this->content.find(" ");
	if (word_end != std::string::npos) {
		word += this->content.substr(0, word_end);
		this->content.erase(0, word_end + 1);
	}
	return word;
}

void	Command::goToExecution () {
	// TODO delete unimplemented functions
	int const nbr_cmd = 29;
	void (Command::*pmf[nbr_cmd])() = {&Command::intOper, &Command::intJoin, \
		&Command::intTopic, &Command::intMode, &Command::intPart, \
		&Command::intNames, &Command::intList, &Command::intInvite, \
		&Command::intKick, &Command::intPrivMsg, &Command::intNotice, \
		&Command::intKill, &Command::intQuit, &Command::intNick, \
		&Command::intWhoIs, &Command::intAway, &Command::intWallops, \
		&Command::intUserhost, &Command::intPass, &Command::intUser, \
		&Command::intSquit, &Command::intMotd, &Command::intTime, \
		&Command::intVersion, &Command::intPing, &Command::intPong, \
		&Command::intError, &Command::intWho, &Command::intAdmin};
	std::string msg[nbr_cmd] = {"OPER", "JOIN", "TOPIC", "MODE", "PART", "NAMES", \
		"LIST", "INVITE", "KICK", "PRIVMSG", "NOTICE", "KILL", "QUIT", "NICK", \
		"WHOIS", "AWAY", "WALLOPS", "USERHOST", "PASS", "USER", "SQUIT", \
		"MOTD", "TIME", "VERSION", "PING", "PONG", "ERROR", "WHO", "ADMIN"};

	for (int i = 0; i < nbr_cmd; i++) {
		if (!this->prefix.compare(msg[i])) {
			(this->*pmf[i])();
			// this->*pmf[i]();
			return ;
		}
	};
	// TODO send error unknown code 421 if not found
}

// Intermediate Commands
void	Command::intUser()
{
	user->user_cmd(getParam());
}

void	Command::intNick()
{
	std::string param = getParam();
	// if (param.size() == 0)
		// return ERRNONICKNAMEGIVEN 431 ":No nickname given"
	// if (param.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890-_{}[]\\`|") != std::string::npos)
		// return ERRONEUSNICKNAME 432 "<nick> :Erroneus nickname"
	// TODO : check if the session is still unregistered
	// if unregistered, create new user object

	std::string nickname;
	size_t pos = param.find("!");
	if (pos != std::string::npos)
	{
		nickname = param.substr(1, pos - 1);
	}
	std::string	new_nickname;
	size_t	pos2 = param.find("NICK") + 5;
	new_nickname = param.substr(pos2);
	user->setNickname(new_nickname);
//    ERR_NICKNAMEINUSE
// 	ERR_NICKCOLLISION
//     ERR_UNAVAILRESOURCE
// 	ERR_RESTRICTED
}

// void	Command::intUserMode()
// {
// 	std::string param = getParam();
// 	std::vector<std::string>	params;

// 	// User	usr;
// 	params = irc::split(param, " ");
// 	// need to select the right user in the channel
// 	user->mode(params);
// 	/*
// 	221    RPL_UMODEIS
// 			"<user mode string>"

// 		- To answer a query about a client's own mode,
// 		RPL_UMODEIS is sent back.
// 	*/
// }

void	Command::intWhoIs()
{
	std::string param = getParam();

	if (param.size() == 0)
		// return ERR_NONICKNAMEGIVEN 431 ":No nickname given"
	// check if user exists in the channel
	if (user)
		user->whois(*user);
	// check numeric replies
}

void	Command::intUserhost()
{
	std::string param = getParam();
	std::vector<std::string> params;

	std::string	reply;
	params = irc::split(param, " ");
	// if (params.size() == 0)
		// return ERR_NEEDMOREPARAMS 461
	unsigned long		i = 0;
	while (i < params.size())
	{
		reply.append(user->getNickname());
		if (user->userModes.o)
			reply.append("*");
		reply.append("=");
		if (user->userModes.a)
			reply.append("-");
		else
			reply.append("+");
		reply.append(user->getHostname());
		if (params.size() > 1)
			reply.append(" ");
		// User::whois(usr);
		i++;
	}
}

void	Command::intAway()
{
	std::string param = getParam();
	user->away(param);
}

void	Command::intPrivMsg()
{
	std::string username;
	std::string nickname;
	std::string hostname;
	std::string server_str;
	std::string msg;

	std::string param = getParam();

	size_t position1 = param.find("@");
	size_t position2 = param.find("%");
	size_t dif = position1 < position2 ? position1 : position2;
	username = param.substr(0, dif);
	size_t pos1 = param.find("!");
	if (pos1 != std::string::npos)
	{
		size_t pos2 = param.find("@");
		size_t pos3 = param.find("%");
		if (pos3 != std::string::npos)
			username = param.substr(pos1 + 1,(pos3 - pos1) - 1);
		else
			username = param.substr(pos1 + 1,(pos2 - pos1) - 1);
		nickname = param.substr(0, pos1);
	}
	size_t pos4 = param.find("@");
	if (pos4 != std::string::npos)
	{
		size_t pos5 = param.find("%");
		if (pos5 != std::string::npos)
		{
			hostname = param.substr(pos5 + 1, pos4 - pos5 - 1);
			server_str = param.substr(pos4 + 1, param.find(" ") - pos4 - 1);
		}
		else
		{
			hostname = param.substr(pos4 + 1, param.find(" ") - pos4 - 1);
		}
	}
	size_t pos6 = param.find("%");
	if (pos6 != std::string::npos && pos4 == std::string::npos)
	{
		hostname = param.substr(pos6 + 1, param.find(" ") - pos6 - 1);
	}

	size_t pos7 = param.find(":");
	if (pos7 != std::string::npos)
		msg = param.substr(pos7 + 1);

	User	*recipient;
	if (username.size() > 0)
		recipient = server.getUserByUsername(username);
	user->privmsg(*recipient, msg);
	// recipient.setNickname(nickname);
	// recipient.setUsername(username);
	// recipient.setHostname(hostname);
}

void	Command::intNotice()
{
	std::string username;
	std::string nickname;
	std::string hostname;
	std::string server_str;
	std::string msg;

	std::string param = getParam();

	size_t position1 = param.find("@");
	size_t position2 = param.find("%");
	size_t dif = position1 < position2 ? position1 : position2;
	username = param.substr(0, dif);
	size_t pos1 = param.find("!");
	if (pos1 != std::string::npos)
	{
		size_t pos2 = param.find("@");
		size_t pos3 = param.find("%");
		if (pos3 != std::string::npos)
			username = param.substr(pos1 + 1,(pos3 - pos1) - 1);
		else
			username = param.substr(pos1 + 1,(pos2 - pos1) - 1);
		nickname = param.substr(0, pos1);
	}
	size_t pos4 = param.find("@");
	if (pos4 != std::string::npos)
	{
		size_t pos5 = param.find("%");
		if (pos5 != std::string::npos)
		{
			hostname = param.substr(pos5 + 1, pos4 - pos5 - 1);
			server_str = param.substr(pos4 + 1, param.find(" ") - pos4 - 1);
		}
		else
		{
			hostname = param.substr(pos4 + 1, param.find(" ") - pos4 - 1);
		}
	}
	size_t pos6 = param.find("%");
	if (pos6 != std::string::npos && pos4 == std::string::npos)
	{
		hostname = param.substr(pos6 + 1, param.find(" ") - pos6 - 1);
	}

	size_t pos7 = param.find(":");
	if (pos7 != std::string::npos)
		msg = param.substr(pos7 + 1);

	User	*recipient;
	if (username.size() > 0)
		recipient = server.getUserByUsername(username);
		// recipient = Channel::getUserFromUsername(username);
	user->notice(msg);
	recipient->setNickname(nickname);
	recipient->setUsername(username);
	recipient->setHostname(hostname);
}

void	Command::intWallops()
{
	std::vector<User *>	users = server.getServUsers();
	std::string			sentence = getParam();
	// if (users.size() < 1)
		// return ERR_NEEDMOREPARAMS 461
	std::vector<User *>::iterator	it = users.begin();
	while (it != users.end())
	{
		if ((*it)->userModes.w)
			send((*it)->getFd(), &sentence, sentence.size(), MSG_DONTWAIT);
		it++;
	}
}

//OLY
std::string left_trim(const std::string &s, std::string to_remove)
{
	size_t start = s.find_first_not_of(to_remove);
	if (start == std::string::npos)
		return (NULL);
	return (s.substr(start));
}

std::string right_trim(const std::string &s, std::string to_remove)
{
	size_t end = s.find_last_not_of(to_remove);
	if (end == std::string::npos)
		return (NULL);
	return (s.substr(0, end + 1));
}

std::string trim(const std::string &s, std::string to_remove)
{
	return (right_trim(left_trim(s, to_remove), to_remove));
}

User *Command::getUser()
{
	return (user);
}

void Command::intJoin()
{
	std::vector<std::string>	vec;
	std::vector<std::string>	vec_chan_names;
	std::vector<std::string>	vec_keys;
	std::string                 name;
	std::string                 key;
	unsigned long				i;
	Channel                     *chan_found;
	std::string                 message;
	ssize_t                         ret;

	vec = irc::split(param, " ");
	vec_chan_names = irc::split(vec[0], ",");
	vec_keys = irc::split(vec[1], ",");
	if (vec_chan_names.size() == 1 && vec_keys.size() == 0 && vec_chan_names[0] == "0")    // JOIN 0
	{
		intQuit();
		return ;
	}
	if (vec_chan_names.size() == 0)  // ERR_NEEDMOREPARAMS
	{
		message = "ERR_NEEDMOREPARAMS\n";
		ret = send(getUser()->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (vec_chan_names.size() < vec_keys.size())  // ERR_NEEDMOREPARAMS ?? // ERR_TOOMANYTARGETS ??
	{
		message = "ERR_NEEDMOREPARAMS // ERR_TOOMANYTARGETS\n"; // ?????
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (vec_chan_names.size() >= 10 || vec_chan_names.size() + user->getNbOfChannels() > 10)  // ERR_TOOMANYCHANNELS
	{
		message = "ERR_TOOMANYCHANNELS\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	i = 0;
	while (i < vec_chan_names.size())
	{
		name = "";
		key = "";
		chan_found = NULL;
		if (vec_chan_names[i].size() != 0)
			name = vec_chan_names[i];
		if (vec_keys[i].size() != 0)
			key = vec_keys[i];
		if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
			name.insert(0, "#");
		chan_found = server.getChannelByName(name);
		if (chan_found == NULL)         // ERR_NOSUCHCHANNEL ??
		{
			chan_found = server.createChannel(name);
			if (key.size() > 0)
				chan_found->setChanPassword(key);
		}
		if (chan_found->userIsBannedFromChan(user->getUsername()))    // ERR_BANNEDFROMCHAN
		{
			message = "ERR_BANNEDFROMCHAN\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			i++;
			continue ;              // proceed to the other requests to join
		}
		if (chan_found->getMaxNbUsersInChan() == chan_found->getNbUsersInChan())    // ERR_CHANNELISFULL
		{
			message = "ERR_CHANNELISFULL\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			i++;
			continue ;              // proceed to the other requests to join
		}
		if (irc::there_is_no('i', chan_found->getChanMode()) == 0)   // ERR_INVITEONLYCHAN
		{
			message = "ERR_INVITEONLYCHAN\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			i++;
			continue ;              // proceed to the other requests to join
		}
		if (vec_keys[i] != chan_found->getChanPassword())   // ERR_BADCHANNELKEY
		{
			message = "ERR_BADCHANNELKEY\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			i++;
			continue ;              // proceed to the other requests to join
		}
		else
		{
			chan_found->addUser(*user);
			message = chan_found->getChanNameAndTopic();    // RPL_TOPIC
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			chan_found->listAllUsersInChan(*user);           // RPL_NAMREPLY
		}
		i++;
	}
	return ;
}

// User *  Server::getUserByUsername(std::string name)
// {
// 	int i;

// 	i = 0;
// 	while (i < users.size())
// 	{
// 		if (users[i].getName() == name)
// 			return (&users[i]);
// 		i++;
// 	}
// 	return (NULL);
// }

// User * Server::getUserByNick(std::string nick)
// {
// 	int i;

// 	i = 0;
// 	while (i < users.size())
// 	{
// 		if (users[i].getNick() == nick)
// 			return (&users[i]);
// 		i++;
// 	}
// 	return (NULL);
// }

void Command::intInvite()
{
	std::vector<std::string>	vec;
	std::string                 nickname;
	std::string                 name;
	// int                         i;
	Channel                     *chan_found;
	std::string                 message;
	int                         ret;
	User *                      user_asked;

	vec = irc::split(param, " ");
	if (vec.size() < 2)  // ERR_NEEDMOREPARAMS
	{
		ret = send(user->getFd(), "ERR_NEEDMOREPARAMS\n", 18, MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	nickname = vec[0];
	name = vec[1];
	chan_found = NULL;
	if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
		name.insert(0, "#");
	chan_found = server.getChannelByName(name);
	user_asked = server.getUserByNick(nickname);
	if (user_asked == NULL) // ERR_NOSUCHNICK
	{
		message = "ERR_NOSUCHNICK\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (chan_found->userIsInChanFromUsername(user->getUsername()) == 0) // ERR_NOTONCHANNEL
	{
		message = "ERR_NOTONCHANNEL\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (irc::there_is_no('i', chan_found->getChanMode()) == 0 && chan_found->isOperator(*user) == 0)  // ERR_CHANOPRIVSNEEDED
	{
		message = "ERR_CHANOPRIVSNEEDED\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (chan_found->userIsInChanFromNickname(name)) // ERR_USERONCHANNEL
	{
		message = "ERR_USERONCHANNEL\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	chan_found->receivingAnInvitation(*user, *user_asked);    // RPL_INVITING
	// RPL_AWAY ??
	return ;
}

void Command::intOper()
{
	std::vector<std::string>	vec;
	std::string                 name;
	std::string                 key;
	// int                         i;
	// Channel                     *chan_found;
	std::string                 message;
	int                         ret;

	vec = irc::split(param, " ");
	if (vec.size() < 2)  // ERR_NEEDMOREPARAMS
	{
		ret = send(user->getFd(), "ERR_NEEDMOREPARAMS\n", 19, MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	name = vec[0];
	key = vec[1];
	if (key != server.getPass())    // ERR_PASSWDMISMATCH
	{
		ret = send(user->getFd(), "ERR_PASSWDMISMATCH\n", 19, MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (user->isOperator())  // RPL_YOUREOPER
	{
		ret = send(user->getFd(), "RPL_YOUREOPER\n", 14, MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	return ;
}

void Command::intPart()
{
	std::vector<std::string>	vec;
	std::vector<std::string>	vec_chan_names;
	std::string					name;
	std::string                 message;
	unsigned long				i;
	Channel                     *chan_found;
	int                         ret;

	vec = irc::split(param, ":");
	vec_chan_names = irc::split(vec[0], ",");
	if (vec_chan_names.size() == 0)  // ERR_NEEDMOREPARAMS
	{
		message = "ERR_NEEDMOREPARAMS\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	i = 0;
	while (i < vec_chan_names.size())
	{
		trim(vec_chan_names[i], " ");
		i++;
	}
	i = 0;
	while (i < vec_chan_names.size())
	{
		chan_found = NULL;
		name = vec_chan_names[i];
		if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
			name.insert(0, "#");
		chan_found = server.getChannelByName(name);
		if (chan_found == NULL)     // ERR_NOSUCHCHANNEL
		{
			message = "ERR_NOSUCHCHANNEL\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			i++;
			continue ;
		}
		if (chan_found->userIsInChanFromUsername(user->getUsername()) == 0) // ERR_NOTONCHANNEL
		{
			message = "ERR_NOTONCHANNEL\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			i++;
			continue ;
		}
		else
		{
			chan_found->writeToAllChanUsers(vec[1]);
		}
		i++;
	}
	return ;
}

void    Command::intQuit()
{
	std::string                 message;
	unsigned long				i;
	std::vector<std::string>    vec_chan_names;
	Channel                     *chan_found;
	std::string                 name;

	message = trim(param, ":");
	vec_chan_names = user->getChannels();
	i = 0;
	while (i < vec_chan_names.size())
	{
		chan_found = NULL;
		name = vec_chan_names[i];
		if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
			name.insert(0, "#");
		chan_found = server.getChannelByName(name);
		chan_found->writeToAllChanUsers(message);
		chan_found->deleteUser(*user, "");
		i++;
	}
}

void    Command::intNames()
{
	std::vector<std::string>	vec_chan_names;
	std::string                 name;
	unsigned long				i;
	Channel                     *chan_found;

	if (param.empty())
	{
		i = 0;
		vec_chan_names = user->getChannels();
		while (i < vec_chan_names.size())
		{
			chan_found = server.getChannelByName(vec_chan_names[i]);
			chan_found->listAllUsersInChan(*user);
			i++;
		}
	}
	else
	{
		vec_chan_names = irc::split(param, ",");
		i = 0;
		while (i < vec_chan_names.size())
		{
			name = "";
			chan_found = NULL;
			if (vec_chan_names[i].size() != 0)
				name = vec_chan_names[i];
			if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
				name.insert(0, "#");
			chan_found = server.getChannelByName(name);
			if (chan_found == NULL)
			{
				i++;
				continue ;
			}
			else
				chan_found->listAllUsersInChan(*user);
			i++;
		}
	}
}

void Command::intList()
{
	std::vector<std::string>	vec_chan_names;
	std::string                 name;
	unsigned long				i;
	Channel                     *chan_found;
	int                         ret;
	std::string					message;

	if (param.empty())
	{
		i = 0;
		vec_chan_names = user->getChannels();
		while (i < vec_chan_names.size())
		{
			chan_found = server.getChannelByName(vec_chan_names[i]);
			chan_found->getChanNameAndTopic();
			i++;
		}
	}
	else
	{
		vec_chan_names = irc::split(param, ",");
		i = 0;
		while (i < vec_chan_names.size())
		{
			name = "";
			chan_found = NULL;
			if (vec_chan_names[i].size() != 0)
				name = vec_chan_names[i];
			if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
				name.insert(0, "#");
			chan_found = server.getChannelByName(name);
			if (chan_found == NULL)
			{
				message = "Invalid channel name\n";
				ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
				if (ret == -1)
				{
					std::cerr << "Could not send message\n";
					return ;
				}
			}
			else
				chan_found->getChanNameAndTopic();
			i++;
		}
	}
	return ;
}

void Command::intKick()
{
	std::vector<std::string>	vec;
	std::vector<std::string>	vec_chan_names;
	std::vector<std::string>	vec_usernames;
	std::string                 message;
	std::string                 name;
	std::string                 user_str;
	unsigned long				i;
	Channel                     *chan_found;
	int                         ret;

	vec = irc::split(param, " ");
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		message = "ERR_NEEDMOREPARAMS\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	vec_chan_names = irc::split(vec[0], ",");
	vec_usernames = irc::split(vec[1], ",");
	if (vec[2].size() > 0)
		message = vec[2];
	if (vec_chan_names.size() == 0 ||
		vec_usernames.size() == 0 ||
		(vec_chan_names.size() > 1 && vec_chan_names.size() != vec_usernames.size()))    // ERR_NEEDMOREPARAMS
	{
		message = "ERR_NEEDMOREPARAMS\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)                              
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (vec_chan_names.size() > 1)
	{
		i = 0;
		while (i < vec_chan_names.size())
		{
			chan_found = NULL;
			name = vec_chan_names[i];
			user_str = vec_usernames[i];
			if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
				name.insert(0, "#");
			chan_found = server.getChannelByName(name);
			if (chan_found == NULL) // ERR_NOSUCHCHANNEL
			{
				ret = send(user->getFd(), "ERR_NOSUCHCHANNEL\n", 17, MSG_DONTWAIT);
				if (ret == -1)
				{
					std::cerr << "Could not send message\n";
					return ;
				}
			}
			if (chan_found->userIsInChanFromUsername(user_str) == 0)         // ERR_USERNOTINCHANNEL
			{
				ret = send(user->getFd(), "ERR_USERNOTINCHANNEL\n", 21, MSG_DONTWAIT);
				if (ret == -1)
				{
					std::cerr << "Could not send message\n";
					return ;
				}
				i++;
				continue ;
			}
			else
			{
				chan_found->deleteUser(*(chan_found->getUserFromUsername(user_str)), "");
			}
			i++;
		}
	}
	return ;
}

void Command::intTopic()
{
	std::vector<std::string>	vec;
	std::string 	            name;
	std::string                 new_topic;
	std::string                 message;
	// int                         i;
	Channel                     *chan_found;
	int                         ret;

	if (param.size() == 0)  // ERR_NEEDMOREPARAMS
	{
		message = "ERR_NEEDMOREPARAMS";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)                              
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (param[param.size() - 1] == ':')
		param += " ";
	vec = irc::split(param, ":");
	name = vec[0];
	if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
		name.insert(0, "#");
	chan_found = server.getChannelByName(name);
	if (chan_found == NULL)
		return ;
	if (chan_found->userIsInChanFromUsername(user->getUsername()) == 0)  // ERR_NOTONCHANNEL
	{
		message = "ERR_NOTONCHANNEL";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)                              
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (vec.size() > 1 && vec[1] != " ")
		new_topic = vec[1];
	else
		new_topic = "";
	if (irc::there_is_no(':', param))    // RPL_TOPIC
	{
		message = chan_found->getChanTopic();
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)                              
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	if (irc::there_is_no('t', chan_found->getChanMode()) == 0 && chan_found->isOperator(*user) == 0) // ERR_CHANOPRIVSNEEDED
	{
		message = "ERR_CHANOPRIVSNEEDED";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)                              
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	else
	{
		chan_found->setChanTopic(new_topic, *user);
		if (new_topic.size() == 0)      // RPL_NOTOPIC
		{
			message = "RPL_NOTOPIC";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)                              
			{
				std::cerr << "Could not send message\n";
				return ;
			}
		}
	}
	return ;
}

void Command::intVersion()
{
	int ret;

	ret = send(user->getFd(), server.getVersionAddr(), sizeof(*(server.getVersionAddr())), MSG_DONTWAIT);  // RPL_VERSION
	if (ret == -1)
	{
		std::cerr << "Could not send message\n";
		return ;
	}
	return ;
}

void Command::intMotd()
{
	int ret;
	std::string motd = server.getMotd();

	if (motd.size() == 0)
	{
		ret = send(user->getFd(), "", 0, MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
	}
	ret = send(user->getFd(), &motd, motd.size(), MSG_DONTWAIT);  // RPL_MOTDSTART RPL_MOTD RPL_ENDOFMOTD
	if (ret == -1)
	{
		std::cerr << "Could not send message\n";
		return ;
	}
	return ;
}

int	Command::isServerOperator(User & user)
{
	unsigned long	i;

	i = 0;
	std::vector<User *>			operators = server.getServOp();

	while (i < operators.size())
	{
		if (operators[i]->getNickname() == user.getNickname())
			return (1);
		i++;
	}
	return (0);
}

void Command::intKill()
{
	std::vector<std::string>	vec;
	std::string             	nickname;
	std::string             	comment;
	std::string                 message;
	int                         ret;
	User *                      user_to_kill;
	std::vector<User *>::iterator	found;

	vec = irc::split(param, " ");
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		message = "ERR_NEEDMOREPARAMS\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	nickname = vec[0];
	comment = vec[1];
	if (server.isServOp(*user) == 0)  // ERR_NOPRIVILEGES
	{
		message = "ERR_NOPRIVILEGES\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	user_to_kill = server.getUserByNick(nickname);
	if (user_to_kill == NULL)  // ERR_NOSUCHNICK
	{
		message = "ERR_NOSUCHNICK\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	std::vector<User *>	users = server.getServUsers();
	found = find(users.begin(), users.end(), user_to_kill);
	if (found == users.end())
	{
		std::cerr << "Cannot delete user: not found\n";
		return ;
	}
	users.erase(found);
	// close socket ??
	return ;
}	

int 	HasInvalidMode(std::string letters)
{	
	int i;

	i = 0;
	while (letters[i])
	{
		if (letters[i] != '0' && letters[i] != 'o' && letters[i] != 'i' &&
			letters[i] != 't' && letters[i] != 'k' && letters[i] != 'l')
			return (1);
		i++;
	}
	return (0);
}	

void	 Command::intMode()
{	
	std::vector<std::string>	vec;
	std::string             	name;
	std::string             	mode;
	std::string                 letters;
	std::string             	arg;
	std::string                 message;
	int                         ret;
	Channel *                   chan_found;
	User *                      user_found;

	vec = irc::split(param, " ");
	user->mode(vec);
	/*
	221    RPL_UMODEIS
			"<user mode string>"

		- To answer a query about a client's own mode,
		RPL_UMODEIS is sent back.
	*/
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		message = "ERR_NEEDMOREPARAMS\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	name = vec[0];
	if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
		name.insert(0, "#");
	chan_found = server.getChannelByName(name);
	mode = vec[1];
	if (mode[0] != '-' && mode[0] != '+')
		name.insert(0, "+");
	letters = trim(name, "-+");
	if (HasInvalidMode(letters))     // ERR_UNKNOWNMODE
	{
		message = "ERR_UNKNOWNMODE\n";
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
	arg = "";
	if (vec.size() > 2)
		arg = vec[2];
	chan_found->addMode(letters);
	if (irc::there_is_no('k', letters) == 0 && vec.size() == 3)
	{
		if (mode[0] == '-' && arg == chan_found->getChanPassword())
			chan_found->setChanPassword("");
		else if (mode[0] == '-' && arg != chan_found->getChanPassword())    // ERR_KEYSET
		{
			message = "ERR_KEYSET\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			return ;
		}
		else if (mode[0] == '+')
			chan_found->setChanPassword(arg);
	}
	if (irc::there_is_no('l', letters) == 0)
	{
		if (mode[0] == '-' && vec.size() == 2)
			chan_found->setMaxNbUsersInChan(100);
		else if (mode[0] == '+')
			chan_found->setMaxNbUsersInChan(std::atoi(arg.c_str()));
	}
	if (irc::there_is_no('o', letters) == 0 && vec.size() == 3)
	{
		user_found = server.getUserByUsername(arg);
		if (user_found == NULL)         // ERR_USERNOTINCHANNEL
		{
			message = "ERR_USERNOTINCHANNEL\n";
			ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			return ;
		}
		if (mode[0] == '-')
			chan_found->deleteOperator(*user_found);
		else if (mode[0] == '+')
			chan_found->addOperator(*user_found);
	}
	if (irc::there_is_no('O', letters) == 0 && vec.size() == 2)
	{
		message = chan_found->getChanCreator();
		ret = send(user->getFd(), &message, message.size(), MSG_DONTWAIT);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
}

void		Command::intPass()
{

}

void		Command::intSquit()
{

}

void		Command::intTime()
{

}

void		Command::intPing()
{

}

void		Command::intPong()
{

}

void		Command::intError()
{

}

void		Command::intWho()
{

}

void		Command::intAdmin()
{

}

