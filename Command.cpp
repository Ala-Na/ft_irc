#include "Command.hpp"
#include "Numerics.hpp"

using namespace irc;

std::string Command::getParam() {
	return (param);
}

Command::Command (Server& server, User* user, std::string& content) : server(server), user(user), content(content) {
	this->prefix = "";
}

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
	int const nbr_cmd = 26;
	void (Command::*pmf[nbr_cmd])() = {&Command::intPass, &Command::intNick, \
		&Command::intUser, &Command::intOper, \
		&Command::intJoin, &Command::intTopic, &Command::intMode, \
		&Command::intPart, &Command::intNames, &Command::intList, \
		&Command::intInvite, &Command::intKick, &Command::intPrivMsg, \
		&Command::intNotice, &Command::intKill, &Command::intQuit, \
		&Command::intWhoIs, &Command::intAway, &Command::intWallops, \
		&Command::intUserhost, &Command::intSquit, &Command::intMotd, \
		&Command::intError, &Command::intSummon, &Command::intUsers, &Command::intPing};
	std::string msg[nbr_cmd] = {"PASS", "NICK", "USER", "OPER", "JOIN", "TOPIC", "MODE", "PART", "NAMES", \
		"LIST", "INVITE", "KICK", "PRIVMSG", "NOTICE", "KILL", "QUIT", \
		"WHOIS", "AWAY", "WALLOPS", "USERHOST", "SQUIT", \
		"MOTD", "ERROR", "SUMMON", "USERS", "PING"};
	
	// TODO delete following
	std::cout << this->prefix << std::endl;
	for (unsigned long i = 0; i < nbr_cmd; i++) {
		if (!this->prefix.compare(msg[i])) {
			if (i >= 3 && this->user->isRegistered() == false) {
				return;
			}
			(this->*pmf[i])();
			return ;
		}
	};
	std::vector<std::string> params;
	params.push_back(this->prefix);
	if (irc::numericReply(421, user, params) == -1) {
		this->server.deleteUser(this->user);
	}
}

// Intermediate Commands
void	Command::intUser() {
	this->server.checkUserCmd(this->user, this->param);
}

void	Command::intPing() {
	this->server.sendPong(this->user, this->param);
}

void	Command::intNick() {
	this->server.checkNick(this->user, this->param);
}


void	Command::intWhoIs() {
	std::vector<std::string>	arg;
	std::vector<std::string>	names;
	unsigned long				i;

	if (param.empty()) {
		if (irc::numericReply(431, this->user, arg) == -1) {
			this->server.deleteUser(this->user);
		}
		return ;
	}
	arg = irc::split(this->param, " ");
	if (arg.size() >= 2 && arg[0] != this->server.getName()) {
		if (irc::numericReply(402, this->user, arg) == -1) {
			this->server.deleteUser(this->user);
		}
		return ;
	}
	if (arg.size() >= 2) {
		names = irc::split(arg[1], ",");
	} else {
		names = irc::split(arg[0], ",");
	}
	for (int i = 0; i < names.size(); i++) {
		User* who = this->server.getUserByNick(names[i]);
		if (who == NULL) {
			arg.clear();
			arg.push_back(names[i]);
			if (irc::numericReply(401, user, arg) == -1) {
				this->server.deleteUser(this->user);
				return;
			}
			continue;
		}
		if (this->user->whois(who) == -1) {
			this->server.deleteUser(this->user);
			return ;
		}
	}
}

void	Command::intUserhost() {
	std::vector<std::string>	nicks;
	std::string					reply = "";

	if (param.empty()) {
		nicks.push_back(this->prefix);
		if (irc::numericReply(461, this->user, nicks) == -1) {
			this->server.deleteUser(this->user);
		}
		return ;
	}
	nicks = irc::split(param, " ");
	for (int i = 0; i < nicks.size(); i++) {
	{
		User* usr = this->server.getUserByNick(nicks[i]);
		if (usr != NULL) {
			reply.append(usr->getNickname());
			if (usr->get_o()) {}
				reply.append("*");
			}
			reply.append("=");
			if (usr->get_a()) {
				reply.append("-");
			} else {
				reply.append("+");
			}
			reply.append(usr->getHostname());
			if (i < nicks.size() - 1) {
				reply.append(" ");
			}
		}
	}
	nicks.clear();
	nicks.push_back(reply);
	if (irc::numericReply(302, user, nicks) == -1) {
		this->server.deleteUser(this->user);
	}
}

void	Command::intAway() {
	if (this->user->away(this->param) == -1) {
		this->server.deleteUser(this->user);
	}
}

// Simplified version of PrivMsg (only nickname or channel, no mask involved)
// Consequence : No ERR_WILDTOPLEVEL, ERR_NOTOPLEVEL or ERR_TOOMANYTARGETS
void	Command::intPrivMsg() {
	std::vector<std::string>	params;
	std::string					is_chan = "&#+!";

	params = irc::split(this->param, " ");
	if (params.empty()) {
		params.push_back(this->prefix);
		if (irc::numericReply(411, this->user, params) == -1) {
			this->server.deleteUser(this->user);
		}
		return ;
	} else if (params.size() < 2) {
		if (irc::numericReply(412, this->user, params) == -1) {
			this->server.deleteUser(this->user);
		}
		return ;	
	}
	if (is_chan.find_first_of(params[0]) == std::string::npos) {
		User*	dest = this->server.getUserByNick(params[0]);
		if (dest == NULL) {
			if (irc::numericReply(401, this->user, params) == -1) {
				this->server.deleteUser(this->user);
			}
			return;
		}
		this->param.erase(0, params[0].size() + 1);
		this->user->privmsgToUser(dest, this->param);
	} else {
		Channel*	chan = this->server.getChannelByName(params[0]);
		if (chan == NULL) {
			if (irc::numericReply(403, this->user, params) == -1) {
				this->server.deleteUser(this->user);
			}
			return;
		}
		this->param.erase(0, params[0].size() + 1);
		this->user->privmsgToChannel(chan, this->param);
	}
}

// Simplified version of Notice (only nickname or channel, no mask involved)
void	Command::intNotice() {
	std::vector<std::string>	params;
	std::string					is_chan = "&#+!";

	params = irc::split(this->param, " ");
	if (params.empty()) {
		return ;
	} else if (params.size() < 2) {
		return ;	
	}
	if (is_chan.find_first_of(params[0]) == std::string::npos) {
		User*	dest = this->server.getUserByNick(params[0]);
		if (dest == NULL) {
			return;
		}
		this->param.erase(0, params[0].size() + 1);
		this->user->noticeToUser(dest, this->param);
	} else {
		Channel*	chan = this->server.getChannelByName(params[0]);
		if (chan == NULL) {
			return;
		}
		this->param.erase(0, params[0].size() + 1);
		this->user->privmsgToChannel(chan, this->param);
	}
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
		if ((*it)->get_w()) {
			int res = irc::sendString((*it)->getFd(), sentence);
			if (res == -1) {
				//TODO close connection
			}
		}
		it++;
	}
}

User *Command::getUser()
{
	return (user);
}

void Command::intJoin() {
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
	// std::vector<User> 			users;
	// std::string 				nicks;

	vec = irc::split(param, " ");
	vec_chan_names = irc::split(vec[0], ",");
	if (vec.size() > 1) {
		vec_keys = irc::split(vec[1], ",");
	}
	if (vec_chan_names.size() == 1 && vec_keys.size() == 0 && vec_chan_names[0] == "0")    // JOIN 0	// ????? Does not work
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
		if (vec_keys.size() > i && vec_keys[i].size() != 0)
			key = vec_keys[i];
		if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
			name.insert(0, "#");
		chan_found = server.getChannelByName(name);
		if (chan_found == NULL) {
			if ((chan_found = server.createChannel(name)) == NULL) {
				return ;
			}
			// TODO modify to user be channel operator
			//user->set_o(true);
			if (key.size() > 0) {
				chan_found->setChanPassword(key);
			}
			if (chan_found->addUser(user) == -1) {
				return ;
			}   
			chan_found->addOperator(user, user); 
		} else if (vec_keys.size() > i && vec_keys[i] != chan_found->getChanPassword()) {  // ERR_BADCHANNELKEY
			params.push_back(vec_chan_names[i]);
			if (irc::numericReply(475, user, params) == -1) {
				this->server.deleteUser(user);
				return ;
			}
			i++;
			continue ;
		} else {
			if (chan_found->addUser(user) == -1) {
				return ;
			}
		}
		i++;
	}
	std::cout << "BEFORE LEAVING INTJOIN FUNCTION\n";
	std::vector<User *> users_end = chan_found->getVecChanUsers();
	i = 0;
	while (i < users_end.size())
	{
		std::cout << "users_end[" << i << "]: " << users_end[i]->getNickname() << std::endl;
		i++;
	}
	std::cout << "LEAVING INTJOIN FUNCTION\n";
	return ;
}

void Command::intInvite() {
	std::vector<std::string>	vec;
	std::string                 nickname;
	std::string                 chan_name;
	Channel                     *chan_found;
	User *                      user_asked;
	std::vector<std::string> 	params;

	vec = irc::split(param, " ");
	if (vec.size() < 2)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(user);
		}
		return ;
	}
	nickname = vec[0];
	chan_name = vec[1];
	chan_found = NULL;
	if (chan_name[0] != '&' && chan_name[0] != '#' && chan_name[0] != '+' && chan_name[0] !=  '!') {
		chan_name.insert(0, "#");
	}
	chan_found = server.getChannelByName(chan_name);
	user_asked = server.getUserByNick(nickname);
	if (user_asked == NULL) // ERR_NOSUCHNICK
	{
		params.push_back(nickname);
		if (irc::numericReply(401, user, params) == -1) {
			this->server.deleteUser(user);
		}
		return ;
	}
	if (chan_found->userIsInChanFromNickname(user->getNickname()) == 0) // ERR_NOTONCHANNEL
	{
		params.push_back(chan_name);
		if (irc::numericReply(442, user, params) == -1) {
			this->server.deleteUser(user);
		}
		return ;
	}
	if (irc::there_is_no('i', chan_found->getChanMode()) == 0 && chan_found->isOperator(user) == 0)  // ERR_CHANOPRIVSNEEDED
	{
		params.push_back(chan_name);
		if (irc::numericReply(482, user, params) == -1) {
			this->server.deleteUser(user);
		}
		return ;
	}
	if (chan_found->userIsInChanFromNickname(nickname)) // ERR_USERONCHANNEL
	{
		params.push_back(nickname);
		params.push_back(chan_name);
		if (irc::numericReply(443, user, params) == -1) {
			this->server.deleteUser(user);
		}
		return ;
	}
	if (chan_found->receivingAnInvitation(user, user_asked) == -1) {
		this->server.deleteUser(user);
		return ;
	}
	params.push_back(nickname);
	params.push_back(chan_name);
	if (irc::numericReply(341, user, params) == -1) {
		this->server.deleteUser(user);
		return ;
	}
	if (user_asked->get_a())
	{
		params.push_back(nickname);
		params.push_back(user_asked->getAwayMessage());
		if (irc::numericReply(301, user, params) == -1) {
			this->server.deleteUser(user);
		}
		return ;
	}
}

void Command::intOper()			// ??? Server does not receive this command and user is asked key but there is none...
{
	std::vector<std::string>	vec;
	std::string                 name;
	std::string                 key;
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
	} else if (user->get_o())  // RPL_YOUREOPER
	{
		irc::numericReply(481, user, arg);
		return ;
	}
	//this->server->setServerOperator(this->user);
	return ;
}

void Command::intPart() {
	std::vector<std::string>	vec;
	std::vector<std::string>	vec_chan_names;
	std::string					name;
	unsigned long				i;
	Channel                     *chan_found;
	std::vector<std::string> 	params;

	vec = irc::split(param, ":");
	vec_chan_names = irc::split(vec[0], ",");
	std::cout << "INTPART 1\n";
	if (vec_chan_names.size() == 0)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	std::cout << "INTPART 2\n";

	i = 0;
	while (i < vec_chan_names.size())
	{
		vec_chan_names[i] = irc::trim(vec_chan_names[i], " ");
		i++;
	}
	std::cout << "INTPART 3\n";

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
			std::cout << "INTPART 4\n";
			continue ;
		}
		if (chan_found->userIsInChanFromUsername(user->getUsername()) == 0) // ERR_NOTONCHANNEL
		{
			params.push_back(name);
			irc::numericReply(442, user, params);
			i++;
			std::cout << "INTPART 5\n";
			continue ;
		}
		else
		{
			std::cout << "INTPART 5,5\nuser : " << user->getNickname() << std::endl;

			chan_found->deleteUser(user, "");
			if (vec.size() > 1) {
				std::cout << "INTPART 6\n";
				if (chan_found->writeToAllChanUsers(vec[1]) == -1)
					return ;
			}
		}
		i++;
	}
	// this->server.deleteUser(user);
	std::cout << "INTPART 7\n";

	std::cout << "BEFORE LEAVING INTPART FUNCTION\n";
	std::vector<User *> users_end = chan_found->getVecChanUsers();
	std::cout << "users_end.size(): " << users_end.size() << std::endl;
	i = 0;
	while (i < users_end.size())
	{
		std::cout << "users_end[" << i << "]: " << users_end[i]->getNickname() << std::endl;
		i++;
	}
	std::cout << "LEAVING INTPART FUNCTION\n";
	return ;
}

void    Command::intQuit() {
	std::string                 message;
	unsigned long				i;
	std::vector<Channel *>    	vec_chan;
	Channel                     *chan_found;
	std::string                 name;

	message = irc::trim(param, ":");
	vec_chan = user->getChannels();
	i = 0;
	while (i < vec_chan.size())
	{
		chan_found = NULL;
		name = vec_chan[i]->getChanName();
		if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
			name.insert(0, "#");
		chan_found = server.getChannelByName(name);
		std::cout << message << std::endl;
		if (chan_found->writeToAllChanUsers(message) == -1) {
			this->server.deleteUser(user);
			return ;
		}
		chan_found->deleteUser(user, "PART " + chan_found->getChanName() + ":" + message);
		i++;
	}
	this->server.sendError(user, this->param);	
	this->server.deleteUser(user);
}

void    Command::intNames() {
	std::vector<Channel *>		vec_chan;
	std::vector<std::string>	vec_chan_names;
	std::string                 name;
	unsigned long				i;
	Channel                     *chan_found;

	if (param.empty())
	{
		i = 0;
		vec_chan = user->getChannels();
		while (i < vec_chan.size())
		{
			if (vec_chan[i]->listAllUsersInChan(user) == -1) {
				this->server.deleteUser(user);
				return ;
			}
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
			if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!') {
				name.insert(0, "#");
			}
			chan_found = server.getChannelByName(name);
			if (chan_found == NULL)
			{
				i++;
				continue ;
			}
			else {
				if (chan_found->listAllUsersInChan(user) == -1) {
					this->server.deleteUser(user);
					return;
				}
			}
			i++;
		}
	}
}

void Command::intKick() {
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
				chan_found->deleteUser((chan_found->getUserFromUsername(user_str)), message);
				// TODO : modify message to : "KICK chan_name user_kicking :reason"
			}
			i++;
		}
	}
	return ;
}

void Command::intTopic()				// when client sends /topic or /topic channel, server gets no request... But irssi answers by giving topic
{
	std::vector<std::string>	vec;
	std::string 	            name;
	std::string                 new_topic;
	Channel                     *chan_found;
	std::vector<std::string>	params;

	if (param.size() == 0)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		irc::numericReply(461, user, params);
		return ;
	}
	if (param[param.size() - 1] == ':')
		param += " ";
	std::cout << param << std::endl;
	vec = irc::split(param, ":");
	name = vec[0];
	if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
		name.insert(0, "#");
	chan_found = server.getChannelByName(name);
	if (chan_found == NULL) {
		return ;
	}
	if (chan_found->userIsInChanFromUsername(user->getUsername()) == 0) { // ERR_NOTONCHANNEL
		params.push_back(name);
		irc::numericReply(442, user, params);
		return ;
	}
	if (irc::there_is_no(':', param)) {    // RPL_TOPIC
		params.push_back(name);
		params.push_back(chan_found->getChanTopic());
		irc::numericReply(332, user, params);
		return ;
	}
	if (irc::there_is_no('t', chan_found->getChanMode()) == 0 && chan_found->isOperator(user) == 0) {// ERR_CHANOPRIVSNEEDED
		params.push_back(name);
		irc::numericReply(482, user, params);
		return ;
	} else {
		if (vec.size() > 1 && vec[1] != " ")
			new_topic = irc::trim(vec[1], ":");
		else
			new_topic = "";
		chan_found->setChanTopic(new_topic, user);
		// std::cout << "chan_found->getChanTopic(): " << chan_found->getChanTopic() << std::endl;
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

void Command::intKill() {
	std::vector<std::string>		vec;
	std::string             		nickname;
	std::string             		comment;
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
	if (letters[i] == '+' || letters[i] == '-')
		i++;
	while (letters[i])
	{
		if (letters[i] != 'O' && letters[i] != 'o' && letters[i] != 'i' &&
			letters[i] != 't' && letters[i] != 'k' && letters[i] != 'l')
			return (1);
		i++;
	}
	return (0);
}	

// TODO :
// Do verification for ERR_NEEDMOREPARAMS
// Check if CHANNEL or USER MODE
// For User : Check that user calling MODE is either modified user or ope
// If not : ERR_USERSDONTMATCH
// Go to User->mode()
// For Channel : LOOK UP RFC
void	 Command::intMode() {
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
	//user->mode(vec);
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
	name = vec[0];			// can be name of channel OR USER... :'(
	if (name[0] != '&' && name[0] != '#' && name[0] != '+' && name[0] !=  '!')
		name.insert(0, "#");
	chan_found = server.getChannelByName(name);
	if (chan_found == NULL)
	{
		name = irc::trim(name, "#");
		user_found = server.getUserByNick(name);
		if (user_found == NULL)
		{
			std::cout << "No such channel or user.\n";
			return ;
		}
	}
	mode = vec[1];
	if (mode[0] != '-' && mode[0] != '+')
		mode.insert(0, "+");
	letters = irc::trim(mode, "-+");
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
	if (chan_found)
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
			chan_found->deleteOperator(this->user, user_found);
		else if (mode[0] == '+')
			chan_found->addOperator(this->user, user_found);
	}
	if (irc::there_is_no('O', letters) == 0 && vec.size() == 2)
	{
		message = chan_found->getChanCreator();
		ret = irc::sendString(user->getFd(), message);
		if (ret == -1)
		{
			// TODO close connection
			return ;
		}
		return ;
	}
}

void		Command::intPass() {
	this->server.checkPassword(user, param);
}

void		Command::intSquit() {
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
void		Command::intError() {
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

void	Command::intList() {
	std::vector<std::string>	params;
	
	if (this->param.empty()) {
		this->server.listChannels(this->user);
	} else {
		std::vector<std::string>	split = irc::split(param, " ");
		std::vector<std::string>	channels = irc::split(split[0], ",");
		for (size_t i = 0; i < channels.size(); i++) {
			Channel* curr_chan = this->server.getChannelByName(channels[i]);
			if (curr_chan != NULL) {
				params.push_back(curr_chan->getChanName());
				char nb[3];
				sprintf(nb, "%d", curr_chan->getNbUsersInChan());
				params.push_back(std::string(nb));
				params.push_back(curr_chan->getChanTopic());
				if (irc::numericReply(322, this->user, params) == -1) {
					this->server.deleteUser(user);
					return;
				}
				params.clear();

			}
		}
	}
	if (irc::numericReply(323, this->user, params) == -1) {
		this->server.deleteUser(user);
		return ;
	}
}
