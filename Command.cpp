#include "Command.hpp"
#include "Numerics.hpp"

using namespace irc;

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
	this->prefix = ft_toupper_str(this->prefix);
	this->param = this->content;

	this->goToExecution();

}

User*	Command::getUser() {
	return (user);
}

std::string Command::getParam() {
	return (param);
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
	int const nbr_cmd = 24;
	void (Command::*pmf[nbr_cmd])() = {&Command::intPass, &Command::intNick, \
		&Command::intUser, &Command::intOper, \
		&Command::intJoin, &Command::intTopic, &Command::intMode, \
		&Command::intPart, &Command::intNames, &Command::intList, \
		&Command::intInvite, &Command::intKick, &Command::intPrivMsg, \
		&Command::intNotice, &Command::intKill, &Command::intQuit, \
		&Command::intWhoIs, &Command::intAway, &Command::intWallops, \
		&Command::intUserhost, &Command::intMotd, \
		&Command::intSummon, &Command::intUsers, &Command::intPing};
	std::string msg[nbr_cmd] = {"PASS", "NICK", "USER", "OPER", "JOIN", "TOPIC", "MODE", "PART", "NAMES", \
		"LIST", "INVITE", "KICK", "PRIVMSG", "NOTICE", "KILL", "QUIT", \
		"WHOIS", "AWAY", "WALLOPS", "USERHOST", "MOTD", "SUMMON", "USERS", "PING"};

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
		this->server.deleteUser(this->user, "Fatal error");
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

	if (param.empty()) {
		if (irc::numericReply(431, this->user, arg) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	arg = irc::split(this->param, " ", 0);
	if (arg.size() >= 2 && arg[0] != this->server.getName()) {
		if (irc::numericReply(402, this->user, arg) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	if (arg.size() >= 2) {
		names = irc::split(arg[1], ",", 0);
	} else {
		names = irc::split(arg[0], ",", 0);
	}
	for (size_t i = 0; i < names.size(); i++) {
		User* who = this->server.getUserByNick(names[i]);
		if (who == NULL) {
			arg.clear();
			arg.push_back(names[i]);
			if (irc::numericReply(401, user, arg) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
				return;
			}
			continue;
		}
		if (this->user->whois(who) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
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
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	nicks = irc::split(param, " ", 0);
	for (size_t i = 0; i < nicks.size(); i++) {
		User* usr = this->server.getUserByNick(nicks[i]);
		if (usr != NULL) {
			reply.append(usr->getNickname());
			if (usr->get_o()) {
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
		this->server.deleteUser(this->user, "Fatal error");
	}
}

void	Command::intAway() {
	if (this->user->away(this->param.erase(0, 1)) == -1) {
		this->server.deleteUser(this->user, "Fatal error");
	}
}

// Simplified version of PrivMsg (only nickname or channel, no mask involved)
// Consequence : No ERR_WILDTOPLEVEL, ERR_NOTOPLEVEL or ERR_TOOMANYTARGETS
void	Command::intPrivMsg() {
	std::vector<std::string>	params;
	std::string					is_chan = "#";

	params = irc::split(this->param, " :", 0);
	if (params.empty()) {
		params.push_back(this->prefix);
		if (irc::numericReply(411, this->user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	} else if (params.size() < 2) {
		if (irc::numericReply(412, this->user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	if (is_chan.find_first_of(params[0]) == std::string::npos) {
		User*	dest = this->server.getUserByNick(params[0]);
		if (dest == NULL) {
			if (irc::numericReply(401, this->user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
			}
			return;
		}
		this->param.erase(0, params[0].size() + 1);
		if (param[0] == ':') {
			this->param.erase(0, 1);
		}
		this->user->privmsgToUser(dest, this->param);
	} else {
		Channel*	chan = this->server.getChannelByName(params[0]);
		if (chan == NULL) {
			if (irc::numericReply(403, this->user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
			}
			return;
		}
		this->param.erase(0, params[0].size() + 1);
		if (param[0] == ':') {
			this->param.erase(0, 1);
		}
		this->user->privmsgToChannel(chan, this->param);
	}
}

// Simplified version of Notice (only nickname or channel, no mask involved)
void	Command::intNotice() {
	std::vector<std::string>	params;
	std::string					is_chan = "#";

	params = irc::split(this->param, " :", 0);
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
		if (param[0] == ':') {
			this->param.erase(0, 1);
		}
		this->user->noticeToUser(dest, this->param);
	} else {
		Channel*	chan = this->server.getChannelByName(params[0]);
		if (chan == NULL) {
			return;
		}
		this->param.erase(0, params[0].size() + 1);
		if (param[0] == ':') {
			this->param.erase(0, 1);
		}
		this->user->noticeToChannel(chan, this->param);
	}
}

void	Command::intWallops() {
	std::vector<std::string>	params;

	if (this->param.empty()) {
		std::vector<std::string> params;
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	this->server.sendWallops(this->param.erase(0,1));
}

void Command::intJoin() {
	std::vector<std::string>	vec;
	std::vector<std::string>	vec_chan_names;
	std::vector<std::string>	vec_keys;
	std::vector<std::string> 	params;
	size_t						i = 0;

	if (param.empty()) { // ERR_NEEDMOREPARAMS
		params.push_back(this->prefix);
		if (irc::numericReply(461, this->user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	vec = irc::split(param, " ", 0);
	vec_chan_names = irc::split(vec[0], ",", 0);
	if (vec.size() > 1) {
		vec_keys = irc::split(vec[1], ",", 0);
	}
	while (i < vec_chan_names.size()) {
		params.clear();
		if (vec_chan_names[i] == "0") { // SPECIAL CASE
			std::vector<Channel *>	usr_chan = this->user->getChannels();
			for (std::vector<Channel *>::iterator chan = usr_chan.begin(); chan != usr_chan.end(); chan ++) {
				this->user->partChannel(*chan, "");
			}
			i++;
			continue ;
		} else if (this->user->getNbOfChannels() >= this->server.getMaxChannelbyUser()) { // ERR_TOOMANYCHANNELS
			params.push_back(vec_chan_names[i]);
			if (irc::numericReply(405, this->user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			i++;
			continue ;
		} else if (vec_chan_names[i].find_first_of(": ,") != std::string::npos) {
			params.push_back(vec_chan_names[i]);
			if (irc::numericReply(476, this->user, params) == -1) { // ERR_BADCHANMASK
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			i++;
			continue ;
		}
		if (vec_chan_names[i][0] != '#') {
			vec_chan_names[i].append(0, '#');
		}
		Channel*	to_join = this->server.getChannelByName(vec_chan_names[i]);
		if (to_join == NULL) {
			if ((to_join = this->server.createChannel(vec_chan_names[i])) == NULL) {
				i++;
				continue ;
			}
			if (i < vec_keys.size() && !vec_keys[i].empty() && vec_keys[i] != "x") {
				to_join->addKey(this->user, vec_keys[i]);
			}
			std::vector<User *>	op;
			op.push_back(this->user);
			to_join->setChanOperators(op);
			if (to_join->addUser(user) == -1) {
				return ;
			}
		} else {
			if (!(to_join->getChanPassword().empty()) && (vec_keys.size() == 0 || vec_keys.size() < i \
				|| vec_keys[i] != to_join->getChanPassword())) {  // ERR_BADCHANNELKEY
				params.push_back(vec_chan_names[i]);
				if (irc::numericReply(475, user, params) == -1) {
					this->server.deleteUser(user, "Fatal error");
					return ;
				}
				i++;
				continue ;
			}
			if (to_join->addUser(this->user) == -1) {
				return ;
			}
		}
		i++;
	}
}

void Command::intInvite() {
	std::vector<std::string>	vec;
	std::string                 nickname;
	std::string                 chan_name;
	Channel                     *chan_found;
	User *                      user_asked;
	std::vector<std::string> 	params;

	vec = irc::split(param, " ", 0);
	if (vec.size() < 2)  // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(user, "Fatal error");
		}
		return ;
	}
	nickname = vec[0];
	chan_name = vec[1];
	chan_found = NULL;
	if (chan_name[0] != '#') {
		chan_name.insert(0, "#");
	}
	chan_found = server.getChannelByName(chan_name);
	user_asked = server.getUserByNick(nickname);
	if (user_asked == NULL) // ERR_NOSUCHNICK
	{
		params.push_back(nickname);
		if (irc::numericReply(401, user, params) == -1) {
			this->server.deleteUser(user, "Fatal error");
		}
		return ;
	}
	if (chan_found->userIsInChanFromNickname(user->getNickname()) == false) // ERR_NOTONCHANNEL
	{
		params.push_back(chan_name);
		if (irc::numericReply(442, user, params) == -1) {
			this->server.deleteUser(user, "Fatal error");
		}
		return ;
	}
	if (irc::there_is_no('i', chan_found->getChanMode()) == 0 && chan_found->isOperator(user) == 0)  // ERR_CHANOPRIVSNEEDED
	{
		params.push_back(chan_name);
		if (irc::numericReply(482, user, params) == -1) {
			this->server.deleteUser(user, "Fatal error");
		}
		return ;
	}
	if (chan_found->userIsInChanFromNickname(nickname) == true) // ERR_USERONCHANNEL
	{
		params.push_back(nickname);
		params.push_back(chan_name);
		if (irc::numericReply(443, user, params) == -1) {
			this->server.deleteUser(user, "Fatal error");
		}
		return ;
	}
	if (chan_found->receivingAnInvitation(user, user_asked) == -1) {
		this->server.deleteUser(user, "Fatal error");
		return ;
	}
	params.push_back(nickname);
	params.push_back(chan_name);
	if (irc::numericReply(341, user, params) == -1) {
		this->server.deleteUser(user, "Fatal error");
		return ;
	}
	if (user_asked->get_a())
	{
		params.push_back(nickname);
		params.push_back(user_asked->getAwayMessage());
		if (irc::numericReply(301, user, params) == -1) {
			this->server.deleteUser(user, "Fatal error");
		}
		return ;
	}
}

// As there's no restriction for user to become OPER, ERR_NOOPERHOST is only
// called when name doesn't suit the user nickname
void Command::intOper() {
	std::vector<std::string>	vec;
	std::string                 name;
	std::string                 key;
	std::vector<std::string> 	arg;

	vec = irc::split(param, " ", 0);
	if (vec.size() < 2) { // ERR_NEEDMOREPARAMS
		arg.push_back(prefix);
		if (irc::numericReply(461, user, arg) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	name = vec[0];
	key = vec[1];
	if (name != this->user->getNickname()) {
		if (irc::numericReply(491, this->user, arg) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	} else if (key != server.getOpPass()) { // ERR_PASSWDMISMATCH
		if (irc::numericReply(464, this->user, arg) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	} else if (this->user->get_o() == false) {
		if (this->server.setServOperator(this->user) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
			return ;
		}
	}
	this->prefix = "MODE";
	this->param = this->user->getNickname();
	intMode();
	return ;
}

void Command::intPart() {
	std::vector<std::string>	vec;
	std::vector<std::string>	chans;
	Channel                     *to_part;
	std::vector<std::string> 	params;
	std::string					part_msg;

	vec = irc::split(param, ":", 0);
	chans = irc::split(vec[0], ",", 0);
	if (chans.size() == 0) {  // ERR_NEEDMOREPARAMS
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	for (size_t i = 0; i < chans.size(); i++) {
		part_msg = "PART ";
		chans[i] = irc::trim(chans[i], " ");
		if (chans[i][0] != '#') {
			chans[i].insert(0, "#");
		}
		to_part = server.getChannelByName(chans[i]);
		if (to_part == NULL) { // ERR_NOSUCHCHANNEL
			params.push_back(chans[i]);
			if (irc::numericReply(403, this->user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			continue ;
		} else if (to_part->userIsInChan(user) == false) { // ERR_NOTONCHANNEL
			params.push_back(chans[i]);
			if (irc::numericReply(442, this->user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			continue ;
		}
		part_msg += chans[i] + " :";
		if (vec.size() > 1) {
			part_msg += vec[1];
		}
		to_part->deleteChanUser(this->user, part_msg);
		this->server.deleteChannel(to_part);
	}
}

void    Command::intQuit() {
	std::string                 message;
	std::vector<Channel *>    	vec_chan;
	std::string                 chan_name;

	message = irc::trim(param, ":");
	vec_chan = this->user->getChannels();
	for (std::vector<Channel *>::reverse_iterator rit = vec_chan.rbegin(); rit != vec_chan.rend(); rit++) {
		(*rit)->deleteChanUser(user, "QUIT :" + message);
	}
	this->server.deleteUser(user, this->param.erase(0, 1));
	this->server.deleteEmptyChannels();
}

void    Command::intNames() {
	std::vector<Channel *>		chan;
	std::vector<User *>			users;
	std::vector<std::string>	chan_names;
	std::vector<std::string>	params;
	Channel                     *to_list;

	params = (irc::split(param, " ", 0));
	if (params.size() >= 2 && params[1] != this->server.getName()) {
		params.erase(params.begin());
		if (irc::numericReply(402, this->user, params) == -1) {
			this->server.deleteUser(user, "Fatal error");
		}
		return ;
	}
	param = params[0];
	params.clear();
	if (this->param.empty()) {
		chan = user->getChannels();
		for (size_t i = 0; i < chan.size(); i++) {
			if (chan[i]->listAllUsersInChan(user) == -1) {
				this->server.deleteUser(user, "Fatal error");
				return ;
			}
		}
		users = this->server.getServUsers();
		for (size_t i = 0; i < users.size(); i++) {
			if (users[i]->getNbOfChannels() == 0 && users[i]->get_i() == false) {
				params.push_back("");
				params.push_back("*");
				params.push_back(users[i]->getNickname());
				if (irc::numericReply(353, this->user, params) == -1) {
					this->server.deleteUser(user, "Fatal error");
					return ;
				}
				params.clear();
			}
		}
		return ;
	}
	chan_names = irc::split(param, ",", 0);
	for (size_t i = 0; i < chan_names.size(); i++) {
		if (chan_names[i][0] != '#') {
			chan_names[i].insert(0, "#");
		}
		to_list = server.getChannelByName(chan_names[i]);
		if (to_list == NULL) {
			params.push_back(chan_names[i]);
			if (irc::numericReply(366, this->user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			params.clear();
			continue ;
		}
		if (to_list->listAllUsersInChan(user) == -1) {
			this->server.deleteUser(user, "Fatal error");
			return;
		}
	}
}

void Command::intKick() {
	std::vector<std::string>	vec;
	std::vector<std::string>	chans;
	std::vector<std::string>	users;
	std::string                 message;
	Channel                     *kick_from;
	std::vector<std::string>	params;

	vec = irc::split(param, " ", 0);
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	chans = irc::split(vec[0], ",", 0);
	users = irc::split(vec[1], ",", 0);
	if (vec.size() > 2 && vec[2] != ":") {
		message = trim(vec[2], ":");
	}
	else {
		message = "no reason";
	}
	if (chans.size() == 0 || users.size() == 0 ||
		(chans.size() > 1 && chans.size() != users.size()))    // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	for (size_t i = 0; i < users.size(); i++) {
		if (chans[i][0] !=  '#')
			chans[i].insert(0, "#");		
	}
	for (size_t i = 0; i < users.size(); i++) {
		if (chans.size() > 1) {
			kick_from = server.getChannelByName(chans[i]);
		} else {
			kick_from = server.getChannelByName(chans[0]);
		}
		if (kick_from == NULL) { // ERR_NOSUCHCHANNEL
			params.push_back(chans[i]);
			if (irc::numericReply(403, user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			continue ;
		} else if (kick_from->userIsInChan(this->user) == false) {
			params.push_back(chans[i]);
			if (irc::numericReply(442, user, params) == -1) { //ERR_NOTONCHANNEL
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			continue ;			
		} else if (kick_from->isOperator(this->user) == 0) {
			params.push_back(chans[i]);
			if (irc::numericReply(482, user, params) == -1) { //ERR_CHANOPRIVSNEEDED
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			continue ;				
		} else if (kick_from->userIsInChanFromNickname(users[i]) == false)	{ // ERR_USERNOTINCHANNEL
			params.push_back(users[i]);
			params.push_back(chans[i]);
			if (irc::numericReply(441, user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
				return ;
			}
			continue ;
		}
		message = ":" + this->user->getNickname() + "!" + this->user->getUsername() + \
			"@" + this->user->getHostname() + " KICK " + chans[i] + " " + users[i] + " :" + message;
		kick_from->deleteChanUser((kick_from->getUserFromNickname(users[i])), message, true);
		this->server.deleteChannel(kick_from);
	}
}

void Command::intTopic() {
	std::vector<std::string>	vec;
	std::string                 new_topic;
	Channel*                    chan;
	std::vector<std::string>	params;

	if (param.size() == 0) { // ERR_NEEDMOREPARAMS
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	vec = irc::split(param, ":", 0);
	if (vec[0][0] != '#') {
		vec[0].insert(0, "#");
	}
	chan = server.getChannelByName(vec[0]);
	if (chan == NULL) {
		return ;
	} else if (chan->userIsInChan(user) == false) { // ERR_NOTONCHANNEL
		params.push_back(vec[0]);
		if (irc::numericReply(442, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	} else if (irc::there_is_no(':', param) == 1) {    // RPL_TOPIC && RPL_NOTOPIC
		params.push_back(vec[0]);
		params.push_back(chan->getChanTopic());
		if (params.back().empty()) {
			if (irc::numericReply(331, user, params) == -1) {
				this->server.deleteUser(this->user, "Fatal error");
			} else {
				if (irc::numericReply(332, user, params) == -1) {
					this->server.deleteUser(this->user, "Fatal error");
				}
			}
			return ;
		}
	} else if (irc::there_is_no('t', chan->getChanMode()) == 0 && chan->isOperator(this->user) == 0) { // ERR_CHANOPRIVSNEEDED
		params.push_back(vec[0]);
		if (irc::numericReply(482, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	if (vec.size() > 1 && vec[1] != " ") {
		new_topic = vec[1];
	}
	else {
		new_topic = "";
	}
	chan->setChanTopic(new_topic, user);
}

void Command::intMotd() {
	this->server.getMotd(this->user, param);
}

void Command::intKill() {
	std::vector<std::string>		vec;
	User*                   	   	user_to_kill;
	std::vector<std::string> 		params;
	std::vector<Channel *>			chans;

	vec = irc::split(param, " ", 0);
	if (vec.size() < 2)      // ERR_NEEDMOREPARAMS
	{
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	} else if (server.isServOperator(this->user) == false) { // ERR_NOPRIVILEGES
		if (irc::numericReply(481, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	user_to_kill = server.getUserByNick(vec[0]);
	if (user_to_kill == NULL) {  // ERR_NOSUCHNICK
		params.push_back(vec[0]);
		if (irc::numericReply(401, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	chans = user_to_kill->getChannels();
	vec[1].erase(0,1);
	if (vec[1].empty()) {
		vec[1] = "no reason specified";
	}
	for (size_t i = 0; i < chans.size(); i++) {
		chans[i]->deleteChanUser(user_to_kill, "PART " + chans[i]->getChanName() + " :KILL - " + vec[1]);
	}
	this->server.deleteEmptyChannels();
	this->server.deleteUser(user_to_kill, "KILL - " + vec[1]);
}

// ERR_NOCHANMODES non supported as only "#" channels are supported
void	 Command::intMode() {
	std::vector<std::string>	vec;
	Channel*                  	mode_chan;
	User*                     	mode_user;
	std::vector<std::string> 	params;
	size_t						name_size;

	vec = irc::split(param, " ", 0);
	if (vec.size() < 1) { // ERR_NEEDMOREPARAMS
		params.push_back(prefix);
		if (irc::numericReply(461, user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	mode_user = this->server.getUserByNick(vec[0]);
	if (mode_user == NULL) {
		name_size = vec[0].size();
		if (vec[0][0] != '#') {
			vec[0].insert(0, "#");
		}
		if ((mode_chan = this->server.getChannelByName(vec[0])) == NULL) {
			params.push_back(vec[0]);
			if (irc::numericReply(403, this->user, params) == -1) { // ERR_NOSUCHCHANNEL (not in RFC but used by open.ircnet.net)
				this->server.deleteUser(this->user,"Fatal error");
			}
			return ;
		}
		mode_chan->setModes(this->user, this->param.erase(0, name_size + 1));
		return ;
	} else if (this->user != mode_user) { // ERR_USERSDONTMATCH
		if (irc::numericReply(502, this->user, params) == -1) {
			this->server.deleteUser(this->user, "Fatal error");
		}
		return ;
	}
	this->param.erase(0, vec[0].size() + 1);
	user->mode(user, this->param);
}

void		Command::intPass() {
	this->server.checkPassword(user, param);
}

void	Command::intSummon() {
	std::vector<std::string>	params;

	if (irc::numericReply(445, user, params) == -1) {
		this->server.deleteUser(this->user, "Fatal error");
	}
}

void	Command::intUsers() {
	std::vector<std::string>	params;

	if (irc::numericReply(446, user, params) == -1) {
		this->server.deleteUser(this->user, "Fatal error");
	}
}

void	Command::intList() {
	std::vector<std::string>	params;

	if (this->param.empty()) {
		this->server.listChannels(this->user);
	} else {
		std::vector<std::string>	split = irc::split(param, " ", 0);
		if (split.size() > 1 && split[1] != this->server.getName()) {
			params.push_back(split[1]);
			if (irc::numericReply(402, this->user, params) == -1) {
				this->server.deleteUser(user, "Fatal error");
			}
			return ;
		}
		std::vector<std::string>	channels = irc::split(split[0], ",", 0);
		for (size_t i = 0; i < channels.size(); i++) {
			Channel* curr_chan = this->server.getChannelByName(channels[i]);
			if (curr_chan != NULL) {
				params.push_back(curr_chan->getChanName());
				char nb[3];
				sprintf(nb, "%d", curr_chan->getNbUsersInChan());
				params.push_back(nb);
				params.push_back(curr_chan->getChanTopic());
				if (irc::numericReply(322, this->user, params) == -1) {
					this->server.deleteUser(user, "Fatal error");
					return ;
				}
				params.clear();
			}
		}
	}
	if (irc::numericReply(323, this->user, params) == -1) {
		this->server.deleteUser(user, "Fatal error");
		return ;
	}
}
