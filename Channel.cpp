#include "Channel.hpp"
#include "Server.hpp"

using namespace irc;

Channel::Channel(Server *server_ptr, std::string name) {
	server = server_ptr;
	chan_name = name;
	chan_password = "";
	chan_topic = "";
	chan_mode = "";
	max_nb_users_in_chan = 99;
};

Channel::~Channel() {};

std::string	Channel::getChanName() {
	return (chan_name);
};

std::string	Channel::getChanPassword() {
	return (chan_password);
};

std::string	Channel::getChanTopic() {
	return (chan_topic);
};

std::string	Channel::getChanMode() {
	return (chan_mode);
};

std::string	Channel::getChanModesParams() {
	std::string	modes = chan_mode;
	std::string params = "";
	bool		was_inserted = false;

	for (size_t i = 0; i < modes.size(); i++) {
		if (modes[i] == 'k') {
			if (was_inserted == true) {
				params += " ";
			}
			params += chan_password;
			was_inserted = true;
		} else if (modes[i] == 'l') {
			if (was_inserted == true) {
				params += " ";
			}
			char nb[2];
			sprintf(nb, "%d", this->getMaxNbUsersInChan());
			params += nb;
			was_inserted = true;
		}
	}
	return params;
}

std::vector<User *>	Channel::getVecChanUsers() {
	return (vec_chan_users);
};

int	Channel::getNbUsersInChan() {
	return (vec_chan_users.size());
};

int	Channel::getMaxNbUsersInChan() {
	return (max_nb_users_in_chan);
};

std::vector<User *>	Channel::getChanOperators() {
	return (vec_chan_operators);
};

std::vector<User *>	Channel::getVecChanBannedUsers() {
	return (vec_chan_banned_users);
};

std::string	Channel::getChanNameAndTopic() {
	std::string ret;

	ret = "NAME: " + chan_name + " - TOPIC: " + chan_topic;
	return (ret);
};

User* Channel::getUserFromNickname(std::string nickname) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i]->getNickname() == nickname)
			return (vec_chan_users[i]);
		i++;
	}
	return (NULL);
};

void	Channel::setChanPassword(std::string password) {
	chan_password = password;
};

int	Channel::isOperator(User* user) {
	unsigned long	i;

	i = 0;
	while (i < vec_chan_operators.size()) {
		if (vec_chan_operators[i]->getNickname() == user->getNickname()) {
			return (1);
		}
		i++;
	}
	return (0);
}

void	Channel::setChanTopic(std::string topic, User* user_who_changes) {
	chan_topic = topic;
	std::string	msg = ":" + user_who_changes->getNickname() + "!" + user_who_changes->getUsername() + "@" + user_who_changes->getHostname();
	msg += " TOPIC " + this->chan_name + " :" + topic + "\r\n";
	writeToAllChanUsers(msg, NULL);
};

void	Channel::setChanMode(std::string mode) {
	chan_mode += mode;
};

void	Channel::setVecChanUsers(std::vector<User *> vec_users) {
	vec_chan_users = vec_users;
};

void	Channel::setMaxNbUsersInChan(int max_nb) {
	if (max_nb > 0 && max_nb < 100)
		max_nb_users_in_chan = max_nb;
};

void	Channel::setChanOperators(std::vector<User *> vec_operators) {
	vec_chan_operators = vec_operators;
};

void	Channel::setVecChanBannedUsers(std::vector<User *> vec_banned_users) {
	vec_chan_banned_users = vec_banned_users;
};

bool	Channel::userIsInChanFromNickname(std::string nickname_to_search) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i]->getNickname() == nickname_to_search)
			return (true);
		i++;
	}
	return (false);
};

bool	Channel::userIsInChan(User* user) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i] == user)
			return (true);
		i++;
	}
	return (false);
};

bool	Channel::userIsBannedFromChan(User* user) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_banned_users.size())
	{
		if (vec_chan_banned_users[i] == user)
			return (true);
		i++;
	}
	return (false);
};

bool	Channel::userIsInvitedInChan(User* user) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_invited_users.size())
	{
		if (vec_chan_invited_users[i] == user)
			return (true);
		i++;
	}
	return (false);
};

bool	Channel::addMode(User* ope, std::string mode) {
	std::string	msg;

	if (this->chan_mode.find(mode) == std::string::npos) {
		this->chan_mode += mode;
		msg = ":"+ ope->getNickname() + "!" + ope->getUsername() + "@" + ope->getHostname();
		msg += " MODE " + this->chan_name + " +" + mode + "\r\n";
		writeToAllChanUsers(msg, NULL);
		return true;
	}
	return false;
};

bool	Channel::deleteMode(User* ope, std::string mode) {
	size_t		it = chan_mode.find(mode);
	std::string	msg;

	if (it != std::string::npos) {
		chan_mode.erase(chan_mode.begin() + it);
		msg = ":"+ ope->getNickname() + "!" + ope->getUsername() + "@" + ope->getHostname();
		msg += " MODE " + this->chan_name + " -" + mode + "\r\n";
		writeToAllChanUsers(msg, NULL);
		return true;
	}
	return false;
}

int	Channel::receivingAnInvitation(User* user_inviting, User* user_invited) {
	std::string	message;
	int			ret;

	addInvitedUser(user_inviting, user_invited);
	message = ":" + user_inviting->getNickname() + "!" + user_inviting->getUsername() + "@" + user_inviting->getHostname();
	message += " INVITE " + user_invited->getNickname() + " :" + this->chan_name + "\r\n";
	ret = irc::sendString(user_invited->getFd(), message);
	if (ret == -1) {
		this->server->deleteUser(user_invited, "Fatal error");
		return (-1);
	}
	return (0);
};

int Channel::listAllUsersInChan(User* user_asking) {
	int							ret;
	std::string					names = "";
	std::string					type;
	std::vector<std::string>	params;
	bool						in_chan = this->userIsInChanFromNickname(user_asking->getNickname());
	bool						is_oper = this->server->isServOperator(user_asking);

	type = this->getChanMode();
	if (type.find("s") != std::string::npos) {
		type = "@";
	} else if (type.find("p") != std::string::npos) {
		type = "*";
	} else {
		type = "=";
	}
	params.push_back(type);
	params.push_back(this->getChanName());
	for (size_t i = 0; i < vec_chan_users.size(); i++) {
		if (is_oper == false && in_chan == false && vec_chan_users[i]->get_i() == true) {
			continue ;
		} else if (this->isOperator(vec_chan_users[i]) == 1) {
			names += "@";
		}
		names += vec_chan_users[i]->getNickname();
		names += " ";
	}
	params.push_back(names);
	ret = irc::numericReply(353, user_asking, params);
	params.clear();
	params.push_back(this->getChanName());
	ret += irc::numericReply(366, user_asking, params);
	if (ret <= -1) {
		this->server->deleteUser(user_asking, "Fatal error");
		return (-1);
	}
	return (0);
};

int	Channel::writeToAllChanUsers(std::string sentence_to_send, User* to_avoid)
{
	unsigned long	i;

	i = 0;
	while (i < vec_chan_users.size()) {
		if (vec_chan_users[i] == to_avoid) {
			i++;
			continue ;
		}
		if (irc::sendString(vec_chan_users[i]->getFd(), sentence_to_send) == -1) {
			std::cerr << "Could not send message to user " << vec_chan_users[i]->getNickname() << "\n";
			return (-1);
		}
		i++;
	}
	return (0);
}

int Channel::addUser(User* user_to_add)
{
	std::vector<std::string>	params;
	int							ret;

	if (vec_chan_users.size() >= max_nb_users_in_chan) { // ERR_CHANNELISFULL
		params.push_back(this->chan_name);
		if (irc::numericReply(471, user_to_add, params) == -1) {
			this->server->deleteUser(user_to_add, "Fatal error");
			return (-1);
		}
		return (0);
	} else if (this->userIsBannedFromChan(user_to_add) == true) { // ERR_BANNEDFROMCHAN
		params.push_back(this->chan_name);
		if (irc::numericReply(474, user_to_add, params) == -1) {
			this->server->deleteUser(user_to_add, "Fatal error");
			return (-1);
		}
		return (0);
	} else if (irc::there_is_no('i', this->getChanMode()) == 0 && \
		this->userIsInvitedInChan(user_to_add) == false) {  // ERR_INVITEONLYCHAN
		params.push_back(this->chan_name);
		if (irc::numericReply(473, user_to_add, params) == -1) {
			this->server->deleteUser(user_to_add, "Fatal error");
			return (-1);
		}
		return (0);
	}
	for (std::vector<User *>::iterator it = vec_chan_users.begin(); it != vec_chan_users.end(); it++) {
		if ((*it) == user_to_add) {
			return (0); // User already present
		}
	}
	vec_chan_users.push_back(user_to_add);
	user_to_add->addChannel(this);
	std::string join_msg = ":" + user_to_add->getNickname() + "!" + user_to_add->getUsername() + "@" + user_to_add->getHostname();
	join_msg += " JOIN :" + this->chan_name + "\r\n";
	ret = this->writeToAllChanUsers(join_msg, NULL); // Sent JOIN to everyone
	params.push_back(this->chan_name);
	if (!this->chan_topic.empty()) {
		params.push_back(this->chan_topic);
		ret += irc::numericReply(332, user_to_add, params); // Sent Rpl_Topic to user added
	}
	ret += this->listAllUsersInChan(user_to_add); // Send Rpl_NamReply to user added
	if (ret < 0) {
		return (-1);
	}
	return (0);
};

// Here, message is either a PART or KICK formated message (KICK/PART #channel (nick for KICK) :reason)
int Channel::deleteChanUser(User* user_to_delete, std::string message, bool kick) {
	std::string msg;

	if (kick == false) {
		msg = ":" + user_to_delete->getNickname() + "!" + user_to_delete->getUsername() + "@" + user_to_delete->getHostname();
		msg += " ";
	}
	msg += message + "\r\n";
	for (std::vector<User *>::iterator it = vec_chan_users.begin(); it != vec_chan_users.end(); it++) {
		if ((*it) == user_to_delete) {
			if (message.find("QUIT") == 0) {
				writeToAllChanUsers(msg, user_to_delete);
			} else {
				writeToAllChanUsers(msg, NULL);
			}
			vec_chan_users.erase(it);
			user_to_delete->leaveChannel(this);
			return (0);
		}
	}
	return (0);
};

void Channel::addOperator(User* operator_adding, std::string nick_target) {
	std::string					msg;
	std::vector<std::string>	param;
	User*						operator_to_add = this->getUserFromNickname(nick_target);

	if (operator_to_add == NULL) {
		param.push_back(nick_target);
		if (this->server->getUserByNick(nick_target) == NULL) {
			irc::numericReply(401, operator_adding, param); // ERR_NOSUCHNICK
		} else {
			param.push_back(chan_name);
			irc::numericReply(441, operator_adding, param); // ERR_NOTONCHANNEL
		}
		return ;
	}
	for (std::vector<User *>::iterator it = vec_chan_operators.begin(); it != vec_chan_operators.end(); it++) {
		if ((*it) == operator_to_add) {
			return ; // User was already an operator
		}
	}
	if (operator_adding != operator_to_add) {
		vec_chan_operators.push_back(operator_to_add);
		msg = ":"+ operator_adding->getNickname() + "!" + operator_adding->getUsername() + "@" + operator_adding->getHostname();
		msg += " MODE " + this->chan_name + " +o " + operator_to_add->getNickname() + "\r\n";
		writeToAllChanUsers(msg, NULL);
	}
};

void Channel::deleteOperator(User* operator_deleting, std::string nick_target) {
	std::string					msg;
	std::vector<std::string>	param;
	User*						operator_to_delete = this->getUserFromNickname(nick_target);

	if (operator_to_delete == NULL) {
		param.push_back(nick_target);
		if (this->server->getUserByNick(nick_target) == NULL) {
			irc::numericReply(401, operator_deleting, param); // ERR_NOSUCHNICK
		} else {
			param.push_back(chan_name);
			irc::numericReply(441, operator_deleting, param); // ERR_NOTONCHANNEL
		}
		return ;
	}
	for (std::vector<User *>::iterator it = vec_chan_operators.begin(); it != vec_chan_operators.end(); it++) {
		if ((*it) == operator_to_delete) {
			vec_chan_operators.erase(it);
			msg = ":"+ operator_deleting->getNickname() + "!" + operator_deleting->getUsername() + "@" + operator_deleting->getHostname();
			msg += " MODE " + this->chan_name + " -o " + operator_to_delete->getNickname() + "\r\n";
			writeToAllChanUsers(msg, NULL);
			return ;
		}
	}
};

int Channel::addBannedUser(User* user_banning, User* user_to_ban) {
	std::string					msg;
	std::vector<User *>			serv_users = this->server->getServUsers();

	if (user_to_ban == NULL) {
		return 0;
	}
	for (std::vector<User *>::iterator it = serv_users.begin(); it != serv_users.end(); it++) {
		if ((*it) == user_to_ban) {
			vec_chan_banned_users.push_back(user_to_ban);
			msg = ":"+ user_banning->getNickname() + "!" + user_banning->getUsername() + "@" + user_banning->getHostname();
			msg += " MODE " + this->chan_name + " +b " + user_to_ban->getNickname() + "\r\n";
			writeToAllChanUsers(msg, NULL);
			return (0);
		}
	}
	return (0);
};

int Channel::deleteBannedUser(User* user_unbanning, User* user_to_unban) {
	std::string					msg;

	if (user_to_unban == NULL) {
		return (0);
	}
	for (std::vector<User *>::iterator it = vec_chan_banned_users.begin(); it != vec_chan_banned_users.end(); it++) {
		if ((*it) == user_to_unban) {
			vec_chan_banned_users.erase(it);
			msg = ":"+ user_unbanning->getNickname() + "!" + user_unbanning->getUsername() + "@" + user_unbanning->getHostname();
			msg += " MODE " + this->chan_name + " -b " + user_to_unban->getNickname() + "\r\n";
			writeToAllChanUsers(msg, NULL);
			return (0);
		}
	}
	return (0);
};

void	Channel::listBannedUsers(User* asking) {
	std::vector<std::string> param;
	for (std::vector<User *>::iterator it = vec_chan_banned_users.begin(); it != vec_chan_banned_users.end(); it++) {
		std::string	mask = (*it)->getNickname() + "!" + (*it)->getUsername() + "@" + (*it)->getHostname();
		param.clear();
		param.push_back(chan_name);
		param.push_back(mask);
		irc::numericReply(367, asking, param);
	}
	param.clear();
	param.push_back(chan_name);
	irc::numericReply(368, asking, param);
}

void	Channel::addKey(User* ope, std::string key) {
	std::string	msg;

	if (!chan_password.empty()) {
		std::vector<std::string>	param;
		param.push_back(chan_name);
		irc::numericReply(467, ope, param);
		return ;
	}
	chan_mode += "k";
	setChanPassword(key);
	msg = ":"+ ope->getNickname() + "!" + ope->getUsername() + "@" + ope->getHostname();
	msg += " MODE " + this->chan_name + " +k " + key + "\r\n";
	writeToAllChanUsers(msg, NULL);
}

void	Channel::deleteKey(User* ope) {
	std::string	msg;
	size_t		it = chan_mode.find('k');

	if (!chan_password.empty()) {
		chan_mode.erase(chan_mode.begin() + it);
		setChanPassword("");
		msg = ":"+ ope->getNickname() + "!" + ope->getUsername() + "@" + ope->getHostname();
		msg += " MODE " + this->chan_name + " -k * \r\n";
		writeToAllChanUsers(msg, NULL);
	}
}

void	Channel::addLimit(User* ope, std::string nb) {
	std::string	msg;
	int			i_nb;

	i_nb = ::atoi(nb.c_str());
	if (i_nb > 0 && i_nb < 100) {
		this->chan_mode += 'l';
		this->setMaxNbUsersInChan(i_nb);
		msg = ":"+ ope->getNickname() + "!" + ope->getUsername() + "@" + ope->getHostname();
		msg += " MODE " + this->chan_name + " +l " + nb + "\r\n";
		writeToAllChanUsers(msg, NULL);
	}
}

void	Channel::deleteLimit(User* ope) {
	std::string	msg;
	size_t		it = chan_mode.find('l');

	if (it != std::string::npos) {
		chan_mode.erase(chan_mode.begin() + it);
		this->setMaxNbUsersInChan(99);
		msg = ":"+ ope->getNickname() + "!" + ope->getUsername() + "@" + ope->getHostname();
		msg += " MODE " + this->chan_name + " -l\r\n";
		writeToAllChanUsers(msg, NULL);
	}
}

int Channel::addInvitedUser(User* user_inviting, User* user_to_invite) {
	std::string					msg;
	std::vector<User *>			serv_users = this->server->getServUsers();

	if (user_to_invite == NULL) {
		return 0;
	}
	for (std::vector<User *>::iterator it = serv_users.begin(); it != serv_users.end(); it++) {
		if ((*it) == user_to_invite) {
			vec_chan_invited_users.push_back(user_to_invite);
			msg = ":"+ user_inviting->getNickname() + "!" + user_inviting->getUsername() + "@" + user_inviting->getHostname();
			msg += " MODE " + this->chan_name + " +I " + user_to_invite->getNickname() + "\r\n";
			writeToAllChanUsers(msg, NULL);
			return (0);
		}
	}
	return (0);
};

int Channel::deleteInvitedUser(User* user_uninviting, User* user_to_uninvite) {
	std::string					msg;

	if (user_to_uninvite == NULL) {
		return (0);
	}
	for (std::vector<User *>::iterator it = vec_chan_invited_users.begin(); it != vec_chan_invited_users.end(); it++) {
		if ((*it) == user_to_uninvite) {
			vec_chan_invited_users.erase(it);
			msg = ":"+ user_uninviting->getNickname() + "!" + user_uninviting->getUsername() + "@" + user_uninviting->getHostname();
			msg += " MODE " + this->chan_name + " -I " + user_to_uninvite->getNickname() + "\r\n";
			writeToAllChanUsers(msg, NULL);
			return (0);
		}
	}
	return (0);
};

void	Channel::listInvitedUsers(User* asking) {
	std::vector<std::string> param;

	for (std::vector<User *>::iterator it = vec_chan_invited_users.begin(); it != vec_chan_invited_users.end(); it++) {
		std::string	mask = (*it)->getNickname() + "!" + (*it)->getUsername() + "@" + (*it)->getHostname();
		param.clear();
		param.push_back(chan_name);
		param.push_back(mask);
		irc::numericReply(346, asking, param);
	}
	param.clear();
	param.push_back(chan_name);
	irc::numericReply(347, asking, param);
}

void	Channel::setModes(User* user, std::string modes) {
	std::vector<std::string>	vec;
	std::vector<std::string>	param;
	int							change = 0;
	bool						ope = true;
	size_t						indicator = 1;
	bool						neutral = true;
	bool						plus = true;
	std::string					valid = "oitklbI";
	
	vec = irc::split(modes, " ", 0);
	if (this->userIsInChan(user) == false) { // ERR_USERNOTINCHANNEL
		param.push_back(user->getNickname());
		param.push_back(chan_name);
		if (irc::numericReply(441, user, param) == -1) {
			this->server->deleteUser(user, "Fatal error");
		}
		return ;
	} else if (modes.empty()) {
		param.push_back(this->getChanName());
		param.push_back("+" + this->getChanMode());
		param.push_back(this->getChanModesParams());
		if (irc::numericReply(324, user, param) == -1) {
			this->server->deleteUser(user, "Fatal error");
		}
		return ;
	} else if (this->isOperator(user) == 0) {
		ope = false;
	}
	for (size_t i = 0; i < vec[0].size(); i++) {
		if (change >= 3) {
			break ;
		} else if (ope == true && vec[0][i] == '+') {
			neutral = false;
			plus = true;
			change--;
		} else if (ope == true && vec[0][i] == '-') {
			neutral = false;
			plus = false;
			change--;
		} else if (ope == true && vec[0][i] == 'i' && neutral == false && plus == true) { // +i
			this->addMode(user, "i");
		} else if (ope == true && vec[0][i] == 't' && neutral == false && plus == true) { // +t
			this->addMode(user, "t");
		} else if (ope == true && vec[0][i] == 'i' && neutral == false && plus == false) { // -i
			this->deleteMode(user, "i");
		} else if (ope == true && vec[0][i] == 't' && neutral == false && plus == false) { // -t
			this->deleteMode(user, "t");
		} else if (ope == true && vec[0][i] == 'o' && neutral == false && plus == true) { // +o
			if (vec.size() > indicator) {
				this->addOperator(user, vec[indicator]);
			}
			indicator++;
		} else if (ope == true && vec[0][i] == 'o' && neutral == false && plus == false) { // -o
			if (vec.size() > indicator) {
				this->deleteOperator(user, vec[indicator]);
			}
			indicator++;
		} else if (ope == true && vec[0][i] == 'k' && neutral == false && plus == true) { // +k
			if (vec.size() > indicator) {
				this->addKey(user, vec[indicator]);
			}
			indicator++;
		} else if (ope == true && vec[0][i] == 'k' && neutral == false && plus == false) { // -k
			this->deleteKey(user);
		} else if (ope == true && vec[0][i] == 'l' && neutral == false && plus == true) { // +l
			if (vec.size() > indicator) {
				this->addLimit(user, vec[indicator]);
			}
			indicator++;
		} else if (ope == true && vec[0][i] == 'l' && neutral == false && plus == false) { // -l
			this->deleteLimit(user);
		} else if (vec[0][i] == 'b' && plus == true) { // +b
			if (ope == true && neutral == false && vec.size() > indicator) {
				this->addBannedUser(user, this->server->getUserByNick(vec[indicator]));
				indicator++;
			} else if (vec.size() <= 1) {
				this->listBannedUsers(user);
				if (ope == false) {
					change++;
				}
			}
		} else if (vec[0][i] == 'b' && plus == false) { // -b
			if (ope == true && neutral == false && vec.size() > indicator) {
				this->deleteBannedUser(user, this->server->getUserByNick(vec[indicator]));
				indicator++;
			} else if (vec.size() <= indicator) {
				this->listBannedUsers(user);
			}			
		} else if (vec[0][i] == 'I' && plus == true) { // +I
			if (ope == true && neutral == false && vec.size() > indicator) {
				this->addInvitedUser(user, this->server->getUserByNick(vec[indicator]));
				indicator++;
			} else if (vec.size() <= indicator) {
				this->listInvitedUsers(user);
				if (ope == false) {
					change++;
				}
			}
		} else if (vec[0][i] == 'I' && plus == false) { // -I
			if (ope == true && neutral == false && vec.size() > indicator) {
				this->deleteInvitedUser(user, this->server->getUserByNick(vec[indicator]));
				indicator++;
			} else if (vec.size() <= indicator) {
				this->listInvitedUsers(user);
			}			
		} else if (valid.find(vec[0][i]) == std::string::npos) { //ERR_UNKNOWNMODE
			param.push_back(std::string(1, vec[0][i]));
			param.push_back(chan_name);
			irc::numericReply(472, user, param);
			param.clear();
			if (ope == true) {
				change--;
			}
		}
		if (ope == true) {
			change++;
		}
	}
	if (ope == false && change == 0) {
		param.push_back(chan_name);
		if (irc::numericReply(482, user, param) == -1) { // ERR_CHANOPRIVSNEEDED
			this->server->deleteUser(user, "Fatal error");
		}
		return ;
	}
}
