#include "Command.hpp"
#include "Numerics.hpp"

using namespace irc;

std::string Command::getParam()
{
	return (param);
}

Command::Command (Server& server, User* user, std::string& content) : server(server), user(user), content(content) {
	std::cout << "nick: " << user->getNickname() << std::endl;
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
	this->prefix = ft_toupper_str(this->prefix);
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
	// std::cout << "nick: " << user->getNickname() << std::endl;

	int const nbr_cmd = 25;
	void (Command::*pmf[nbr_cmd])() = {&Command::intPass, &Command::intNick, \
		&Command::intUser, &Command::intOper, \
		&Command::intJoin, &Command::intTopic, &Command::intMode, \
		&Command::intPart, &Command::intNames, &Command::intList, \
		&Command::intInvite, &Command::intKick, &Command::intPrivMsg, \
		&Command::intNotice, &Command::intKill, &Command::intQuit, \
		&Command::intWhoIs, &Command::intAway, &Command::intWallops, \
		&Command::intUserhost, &Command::intSquit, &Command::intMotd, \
		&Command::intError, &Command::intSummon, &Command::intUsers};
	std::string msg[nbr_cmd] = {"PASS", "NICK", "USER", "OPER", "JOIN", "TOPIC", "MODE", "PART", "NAMES", \
		"LIST", "INVITE", "KICK", "PRIVMSG", "NOTICE", "KILL", "QUIT", \
		"WHOIS", "AWAY", "WALLOPS", "USERHOST", "SQUIT", \
		"MOTD", "ERROR", "SUMMON", "USERS"};

	std::cout << this->prefix << std::endl;

	for (unsigned long i = 0; i < nbr_cmd; i++) {
		if (!this->prefix.compare(msg[i])) {
			if (i >= 3 && this->user->isRegistered() == false) {
				break;
			}
			(this->*pmf[i])();
			return ;
		}
	};
	// TODO send error unknown code 421 if not found
}

// Intermediate Commands
void	Command::intUser()
{
	this->server.checkUserCmd(this->user, this->param);
	// Made in server to make deletion of unregistered user easier
}

void	Command::intNick()
{
	std::string param = getParam();
	std::vector<std::string> arg;
	arg.push_back(param);

	this->server.checkNick(this->user, this->param);
	// Made in server to make deletion of unregistered user easier

	//std::string nickname;
	//size_t pos = param.find("!");
	//if (pos != std::string::npos)
	//{
	//	nickname = param.substr(1, pos - 1);
	//}
	//std::string	new_nickname;
	//size_t	pos2 = param.find("NICK") + 5;
	//new_nickname = param.substr(pos2);
	//user->setNickname(new_nickname);
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
	std::vector<std::string>	arg;
	unsigned long				i;
	arg.push_back(user->getNickname());
	arg.push_back(user->getUsername());
	arg.push_back(user->getHostname());
	arg.push_back(user->getRealName());

	// ERR_NOSUCHSERVER    RPL_WHOISSERVER


	if (param.size() == 0)
		irc::numericReply(431, user, arg);
		// return ERR_NONICKNAMEGIVEN 431 ":No nickname given"
	// check if user exists in the channel
	if (user)
		user->whois(*user);
	if (!user)
		irc::numericReply(401, user, arg); // NOSUCHNICK
	irc::numericReply(311, user, arg); // WHOISUSER
	arg.clear();
	arg.push_back(user->getNickname());
	// get channels
	std::vector<std::string> channels = user->getChannels();
	std::string    chan_names = "";
	i = 0;
	while (i < channels.size())
	{
		chan_names += channels[i];
		if (i < channels.size() - 1)
			chan_names += ", ";
		i++;
	}
	arg.push_back(chan_names);
	irc::numericReply(319, user, arg); // WHOISCHANNELS

	//whoisserver
	arg.clear();
	arg.push_back(user->getNickname());
	arg.push_back(user->getAwayMessage());
	arg.push_back(server.getName());
	irc::numericReply(312, user, arg); // RPL_WHOISSERVER

	if (user->userModes.get_a())
		irc::numericReply(301, user, arg); // RPL_AWAY
	if (user->userModes.get_o())
		irc::numericReply(313, user, arg); // RPL_WHOISOPERATOR
	irc::numericReply(318, user, arg); // RPL_ENDOFWHOIS
}

void	Command::intUserhost()
{
	std::string param = getParam();
	std::vector<std::string>	params;
	unsigned long				i;

	std::string	reply;
	params = irc::split(param, " ");
	if (params.size() == 0)	// ERR_NEEDMOREPARAMS
	{
		std::vector<std::string> para;
		params.push_back(prefix);
		irc::numericReply(461, user, para);
		return ;
	}
	i = 0;
	while (i < params.size())
	{
		reply.append(user->getNickname());
		if (user->userModes.get_o())
			reply.append("*");
		reply.append("=");
		if (user->userModes.get_a())
			reply.append("-");
		else
			reply.append("+");
		reply.append(user->getHostname());
		if (params.size() > 1)
			reply.append(" ");
		// User::whois(usr);
		i++;
	}
	std::vector<std::string> arg;
	arg.push_back(reply);
	irc::numericReply(302, user, arg);

}

void	Command::intAway()
{
	std::string param = getParam();
	user->away(param);
	std::vector<std::string> arg;
	if (user->userModes.get_a())
		irc::numericReply(306, user, arg); // NOWAWAY
	else
		irc::numericReply(305, user, arg); // UNAWAY

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
	std::vector<std::string> arg;
	if (msg.size() == 0)
		irc::numericReply(412, user, arg); // NOTEXTTOSEND
	arg.push_back(user->getNickname());
	if (username.size() == 0)
		irc::numericReply(411, user, arg); // NORECIPIENT
	arg.clear();
	arg.push_back("PRIVMSG"); //command
	User	*recipient = NULL;
	if (username.size() > 0)
		recipient = server.getUserByUsername(username);
	if (!recipient)
		irc::numericReply(401, user, arg); // NOSUCHNICK

	user->privmsg(*recipient, msg);
	arg.clear();
	arg.push_back(recipient->getNickname());
	arg.push_back(recipient->getAwayMessage());
	if (recipient->userModes.get_a())
		irc::numericReply(301, user, arg); // RPL_AWAY

	recipient->setNickname(nickname);
	recipient->setUsername(username);
	recipient->setHostname(hostname);

	//    ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
		// ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
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

	std::vector<std::string> arg;
	if (msg.size() == 0)
		irc::numericReply(412, user, arg); // NOTEXTTOSEND
	arg.push_back(user->getNickname());
	if (username.size() == 0)
		irc::numericReply(411, user, arg); // NORECIPIENT
	arg.clear();
	arg.push_back("PRIVMSG"); //command
	User	*recipient = NULL;
	if (username.size() > 0)
		recipient = server.getUserByUsername(username);
		// recipient = Channel::getUserFromUsername(username);
	if (!recipient)
		irc::numericReply(401, user, arg); // NOSUCHNICK
	user->notice(msg);
	arg.clear();
	arg.push_back(recipient->getNickname());
	arg.push_back(recipient->getAwayMessage());
	if (recipient->userModes.get_a())
		irc::numericReply(301, user, arg); // RPL_AWAY

	recipient->setNickname(nickname);
	recipient->setUsername(username);
	recipient->setHostname(hostname);
}

void	Command::intWallops()
{
	std::vector<User *>	users = server.getServUsers();
	std::string			sentence = getParam();
	if (users.size() < 1)	// ERR_NEEDMOREPARAMS
	{
		std::vector<std::string> params;
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	std::vector<User *>::iterator	it = users.begin();
	while (it != users.end())
	{
		if ((*it)->userModes.get_w())
			irc::sendString((*it)->getFd(), sentence);
		it++;
	}
}

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
	std::vector<std::string> 	params;
	std::string					chan_names;
	unsigned long				i;
	std::string                 name;
	std::string                 key;
	Channel                     *chan_found;
	std::string                 message;
	ssize_t                     ret;
	std::vector<User> 			users;
	std::string 				nicks;

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
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	if (vec_chan_names.size() < vec_keys.size())  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	if (vec_chan_names.size() >= 10 || vec_chan_names.size() + user->getNbOfChannels() > 10)  // ERR_TOOMANYCHANNELS
	{
		chan_names = "";
		i = 0;
		while (i < vec_chan_names.size())
		{
			chan_names += vec_chan_names[i];
			if (i < vec_chan_names.size() - 1)
				chan_names += ", ";
			i++;
		}
		params.push_back(chan_names);
		irc::numericReply(405, user, params);
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
		if (chan_found == NULL)
		{
			chan_found = server.createChannel(name);
			if (key.size() > 0)
				chan_found->setChanPassword(key);
		}
		if (chan_found->userIsBannedFromChan(user->getUsername()))    // ERR_BANNEDFROMCHAN
		{
			params.push_back(vec_chan_names[i]);
			irc::numericReply(474, user, params);
			i++;
			continue ;
		}
		if (chan_found->getMaxNbUsersInChan() == chan_found->getNbUsersInChan())    // ERR_CHANNELISFULL
		{
			params.push_back(vec_chan_names[i]);
			irc::numericReply(471, user, params);
			i++;
			continue ;
		}
		if (irc::there_is_no('i', chan_found->getChanMode()) == 0)   // ERR_INVITEONLYCHAN
		{
			params.push_back(vec_chan_names[i]);
			irc::numericReply(473, user, params);
			i++;
			continue ;
		}
		if (vec_keys[i] != chan_found->getChanPassword())   // ERR_BADCHANNELKEY
		{
			params.push_back(vec_chan_names[i]);
			irc::numericReply(475, user, params);
			i++;
			continue ;
		}
		else
		{
			message = chan_found->getChanName() + ": channel joined\n";
			ret = irc::sendString(user->getFd(), message);
			if (ret == -1)
			{
				std::cerr << "Could not send message\n";
				return ;
			}
			chan_found->addUser(*user);
			params.push_back(chan_found->getChanName());
			params.push_back(chan_found->getChanTopic());
			irc::numericReply(332, user, params);
			params.clear();
			params.push_back("=");
			params.push_back(chan_found->getChanName());
			users = chan_found->getVecChanUsers();
			nicks = "";
			i = 0;
			while (i < users.size())
			{
				nicks += users[i].getNickname();
				if (i < users.size() - 1)
					nicks += ", ";
				i++;
			}
			params.push_back(nicks);
			irc::numericReply(353, user, params);	// RPL_NAMREPLY           
		}
		i++;
	}
	return ;
}

void Command::intInvite()
{
	std::vector<std::string>	vec;
	std::string                 nickname;
	std::string                 chan_name;
	Channel                     *chan_found;
	// int                         ret;
	User *                      user_asked;
	std::vector<std::string> 	params;

	vec = irc::split(param, " ");
	if (vec.size() < 2)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	nickname = vec[0];
	chan_name = vec[1];
	chan_found = NULL;
	if (chan_name[0] != '&' && chan_name[0] != '#' && chan_name[0] != '+' && chan_name[0] !=  '!')
		chan_name.insert(0, "#");
	chan_found = server.getChannelByName(chan_name);
	user_asked = server.getUserByNick(nickname);
	if (user_asked == NULL) // ERR_NOSUCHNICK
	{
		params.push_back(nickname);
		irc::numericReply(401, user, params);
		return ;
	}
	if (chan_found->userIsInChanFromUsername(user->getUsername()) == 0) // ERR_NOTONCHANNEL
	{
		params.push_back(chan_name);
		irc::numericReply(442, user, params);
		return ;
	}
	if (irc::there_is_no('i', chan_found->getChanMode()) == 0 && chan_found->isOperator(*user) == 0)  // ERR_CHANOPRIVSNEEDED
	{
		params.push_back(chan_name);
		irc::numericReply(482, user, params);
		return ;
	}
	if (chan_found->userIsInChanFromNickname(nickname)) // ERR_USERONCHANNEL
	{
		params.push_back(nickname);
		params.push_back(chan_name);
		irc::numericReply(443, user, params);
		return ;
	}
	chan_found->receivingAnInvitation(*user, *user_asked);    // RPL_INVITING
	params.push_back(nickname);
	params.push_back(chan_name);
	irc::numericReply(341, user, params);
	if (user_asked->isAway())	// RPL_AWAY
	{
		params.push_back(nickname);
		params.push_back(user_asked->getAwayMessage());
		irc::numericReply(301, user, params);
	}
	return ;
}

void Command::intOper()
{
	std::vector<std::string>	vec;
	std::string                 name;
	std::string                 key;
	// int                         ret;
	std::vector<std::string> 	params;

	vec = irc::split(param, " ");
	if (vec.size() < 2)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	name = vec[0];
	key = vec[1];
	std::vector<std::string> arg;
	if (key != server.getPass())    // ERR_PASSWDMISMATCH
	{
		irc::numericReply(464, user, arg);
		return ;
	} else if (user->isOperator())  // RPL_YOUREOPER
	{
		irc::numericReply(481, user, arg);
		return ;
	}
	//this->server->setServerOperator(this->user);
	return ;
}

void Command::intPart()
{
	std::vector<std::string>	vec;
	std::vector<std::string>	vec_chan_names;
	std::string					name;
	unsigned long				i;
	Channel                     *chan_found;
	std::vector<std::string> 	params;

	vec = irc::split(param, ":");
	vec_chan_names = irc::split(vec[0], ",");
	if (vec_chan_names.size() == 0)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
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
			params.push_back(name);
			irc::numericReply(403, user, params);
			i++;
			continue ;
		}
		if (chan_found->userIsInChanFromUsername(user->getUsername()) == 0) // ERR_NOTONCHANNEL
		{
			params.push_back(name);
			irc::numericReply(442, user, params);
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
				return ;
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
	std::vector<std::string>	params;

	vec = irc::split(param, " ");
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	vec_chan_names = irc::split(vec[0], ",");
	vec_usernames = irc::split(vec[1], ",");
	if (vec[2].size() > 0)
		message = vec[2];
	else
		message = user->getNickname() + " has kicked you\n";
	if (vec_chan_names.size() == 0 ||
		vec_usernames.size() == 0 ||
		(vec_chan_names.size() > 1 && vec_chan_names.size() != vec_usernames.size()))    // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
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
				params.push_back(name);
				irc::numericReply(403, user, params);
			}
			if (chan_found->userIsInChanFromUsername(user_str) == 0)	// ERR_USERNOTINCHANNEL
			{
				params.push_back(user_str);
				params.push_back(name);
				irc::numericReply(441, user, params);
				i++;
				continue ;
			}
			else
			{
				chan_found->deleteUser(*(chan_found->getUserFromUsername(user_str)), message);
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
	Channel                     *chan_found;
	// int                         ret;
	std::vector<std::string>	params;

	if (param.size() == 0)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
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
		params.push_back(name);
		irc::numericReply(442, user, params);
		return ;
	}
	if (vec.size() > 1 && vec[1] != " ")
		new_topic = vec[1];
	else
		new_topic = "";
	if (irc::there_is_no(':', param))    // RPL_TOPIC
	{
		params.push_back(name);
		params.push_back(chan_found->getChanTopic());
		irc::numericReply(332, user, params);
		return ;
	}
	if (irc::there_is_no('t', chan_found->getChanMode()) == 0 && chan_found->isOperator(*user) == 0) // ERR_CHANOPRIVSNEEDED
	{
		params.push_back(name);
		irc::numericReply(482, user, params);
		return ;
	}
	else
	{
		chan_found->setChanTopic(new_topic, *user);
		if (new_topic.size() == 0)      // RPL_NOTOPIC
		{
			params.push_back(name);
			irc::numericReply(482, user, params);
		}
	}
	return ;
}

void Command::intMotd() {
	this->server.getMotd(this->user, param);
}

int	Command::isServerOperator(User & user)
{
	unsigned long	i;
	std::vector<User *>	operators;

	i = 0;
	operators = server.getServOp();

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
	std::vector<std::string>		vec;
	std::string             		nickname;
	std::string             		comment;
	// int                         	ret;
	User *                      	user_to_kill;
	std::vector<User *>::iterator	found;
	std::vector<std::string> 		params;

	vec = irc::split(param, " ");
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	nickname = vec[0];
	comment = vec[1];
	if (server.isServOp(*user) == 0)  // ERR_NOPRIVILEGES
	{
		irc::numericReply(481, user, params);
		return ;
	}
	user_to_kill = server.getUserByNick(nickname);
	if (user_to_kill == NULL)  // ERR_NOSUCHNICK
	{
		params.push_back(nickname);
		irc::numericReply(401, user, params);
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
	unsigned long	i;

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
	std::vector<std::string> 	params;

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
		params.push_back(prefix);
		irc::numericReply(461, user, params);
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
		params.push_back(letters);
		params.push_back(name);
		irc::numericReply(472, user, params);
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
			params.push_back(name);
			irc::numericReply(467, user, params);
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
			params.push_back(arg);
			params.push_back(name);
			irc::numericReply(441, user, params);
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
		ret = irc::sendString(user->getFd(), message);
		if (ret == -1)
		{
			std::cerr << "Could not send message\n";
			return ;
		}
		return ;
	}
}

void		Command::intPass() {
	this->server.checkPassword(user, param);
}

void		Command::intSquit()
{
	std::vector<std::string>	vec;
	std::string					serv_name;
	std::string					comment;
	std::vector<std::string>	params;

	vec = irc::split(param, " ");
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	std::vector<std::string> arg;
	if (server.isServOp(*user) == 0)  // ERR_NOPRIVILEGES
	{
		irc::numericReply(481, user, arg);
		return ;
	}
	serv_name = vec[0];
	comment = vec[1];
	if (serv_name != server.getName())  // ERR_NOSUCHSERVER
	{
		params.push_back(serv_name);
		irc::numericReply(402, user, params);
	}
	// close connection ?????
}

// The ERROR message is also used before terminating a client
//    connection.
/*
   When a server sends a received ERROR message to its operators, the
   message SHOULD be encapsulated inside a NOTICE message, indicating
   that the client was not responsible for the error.
*/
void		Command::intError()
{
	std::vector<std::string> msg;
	msg.push_back("The client was not responsible for the error.");
	user->setParams(msg);
	intNotice();
}

void	Command::intSummon() {
	std::vector<std::string>	params;

	irc::numericReply(445, user, params);	
}

void	Command::intUsers() {
	std::vector<std::string>	params;

	irc::numericReply(446, user, params);		
}
