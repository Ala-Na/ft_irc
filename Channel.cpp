#include "Channel.hpp"
#include "Server.hpp"

using namespace irc;

Channel::Channel(Server *server_ptr, std::string name) {
	server = server_ptr;
	chan_name = name;
	chan_password = "";
	chan_topic = "";
	chan_mode = "";
	max_nb_users_in_chan = 10;
	chan_creator = "";
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

std::string	Channel::getChanCreator() {
	return (chan_creator);
};

std::vector<User *>	Channel::getVecChanBannedUsers() {
	return (vec_chan_banned_users);
};

std::string	Channel::getChanNameAndTopic() {
	std::string ret;

	ret = "NAME: " + chan_name + " - TOPIC: " + chan_topic;
	return (ret);
};

User* Channel::getUserFromUsername(std::string username) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i]->getUsername() == username)
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
	if (irc::there_is_no('t', chan_mode) || (irc::there_is_no('t', chan_mode) == 0 && isOperator(user_who_changes))) {
		chan_topic = topic;
		std::string	msg = ":" + user_who_changes->getNickname() + "!" + user_who_changes->getUsername() + "@" + user_who_changes->getHostname();
		msg += " TOPIC " + this->chan_name + " :" + topic + "\r\n";
		writeToAllChanUsers(msg);
	}
};

void	Channel::setChanMode(std::string mode) {
	chan_mode += mode;
};

void	Channel::setVecChanUsers(std::vector<User *> vec_users) {
	vec_chan_users = vec_users;
};

void	Channel::setMaxNbUsersInChan(int max_nb) {
	if (max_nb > 0)
		max_nb_users_in_chan = max_nb;
};

void	Channel::setChanOperators(std::vector<User *> vec_operators) {
	vec_chan_operators = vec_operators;
};

void	Channel::setChanCreator(std::string creator) {
	chan_creator = creator;
};

void	Channel::setVecChanBannedUsers(std::vector<User *> vec_banned_users) {
	vec_chan_banned_users = vec_banned_users;
};

int	Channel::userIsInChanFromUsername(std::string username_to_search) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i]->getUsername() == username_to_search)
			return (1);
		i++;
	}
	return (0);
};

int	Channel::userIsInChanFromNickname(std::string nickname_to_search) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i]->getNickname() == nickname_to_search)
			return (1);
		i++;
	}
	return (0);
};

int	Channel::userIsBannedFromChan(std::string username_to_search) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_banned_users.size())
	{
		if (vec_chan_banned_users[i]->getUsername() == username_to_search)
			return (1);
		i++;
	}
	return (0);
};

void	Channel::addMode(std::string mode) {
	chan_mode += mode;
};

int	Channel::receivingAnInvitation(User* user_inviting, User* user_invited) {
	std::string	message;
	int			ret;

	message = ":" + user_inviting->getNickname() + "!" + user_inviting->getUsername() + "@" + user_inviting->getHostname();
	message += " INVITE " + user_invited->getNickname() + " :" + this->chan_name + "\r\n";
	ret = irc::sendString(user_invited->getFd(), message);
	if (ret == -1) {
		this->server->deleteUser(user_invited);
		return (-1);
	}
	return (0);
};

int Channel::listAllUsersInChan(User* user_asking)
{
	unsigned long				i;
	int							ret;
	std::string					names;
	std::string					type;
	std::vector<std::string>	params;

	i = 0;
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
	while (i < vec_chan_users.size()) {
		if (this->isOperator(vec_chan_users[i]) == 1) {
			names += "@";
		} // NOTE : Moderated channel not taken into account, but if permission to speak + in front of nickname
		names += vec_chan_users[i]->getNickname();
		i++;
		params.push_back(names);
	}
	params.push_back(names);
	ret = irc::numericReply(353, user_asking, params);
	if (ret == -1) {
		this->server->deleteUser(user_asking);
		return (-1);
	}
	return (0);
};

int	Channel::writeToAllChanUsers(std::string sentence_to_send)
{
	unsigned long	i;
	int				ret;

	i = 0;
	while (i < vec_chan_users.size())
	{
		ret = irc::sendString(vec_chan_users[i]->getFd(), sentence_to_send);
		if (ret == -1) {
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

	if (vec_chan_users.size() == max_nb_users_in_chan) { // ERR_CHANNELISFULL
		params.push_back(this->chan_name);
		if (irc::numericReply(471, user_to_add, params) == -1) {
			this->server->deleteUser(user_to_add);
			return (-1);
		}
		return (0);
	} else if (this->userIsBannedFromChan(user_to_add->getUsername())) { // ERR_BANNEDFROMCHAN
		params.push_back(this->chan_name);
		if (irc::numericReply(474, user_to_add, params) == -1) {
			this->server->deleteUser(user_to_add);
			return (-1);
		}
		return (0);
	} else if (irc::there_is_no('i', this->getChanMode()) == 0) {  // ERR_INVITEONLYCHAN
		params.push_back(this->chan_name);
		if (irc::numericReply(473, user_to_add, params) == -1) {
			this->server->deleteUser(user_to_add);
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
	ret = this->writeToAllChanUsers(join_msg); // Sent JOIN to everyone
	params.push_back(this->chan_name);
	params.push_back(this->chan_topic);
	ret += irc::numericReply(332, user_to_add, params); // Sent Rpl_Topic to user added
	ret += listAllUsersInChan(user_to_add); // Send Rpl_NamReply to user added
	if (ret < 0) {
		return (-1);
	}
	return (0);
};

// Here, message is either a PART or KICK formated message (KICK/PART nick :reason)
int Channel::deleteUser(User* user_to_delete, std::string message) {
	std::string msg;

	msg = ":" + user_to_delete->getNickname() + "!" + user_to_delete->getUsername() + "@" + user_to_delete->getHostname();
	msg += " " + message + "\r\n";
	for (std::vector<User *>::iterator it = vec_chan_users.begin(); it != vec_chan_users.end(); it++) {
		if ((*it) == user_to_delete) {
			writeToAllChanUsers(msg);
			// std::cout << "IN DELETEUSER nickname:/top " << user_to_delete->getNickname() << std::endl;
			vec_chan_users.erase(it);
			user_to_delete->deleteChannel(this);
			return (0);
		}
	}
	return (0);
};

// TODO : maybe delete +O mode, or keep it and change message for +O in this case
int Channel::addOperator(User* operator_adding, User* operator_to_add) {
	std::string					msg;

	for (std::vector<User *>::iterator it = vec_chan_operators.begin(); it != vec_chan_operators.end(); it++) {
		if ((*it) == operator_to_add) {
			return (0); // User was already an operator
		}
	}
	if (operator_adding != operator_to_add) { 
		vec_chan_operators.push_back(operator_to_add);
		msg = ":"+ operator_adding->getNickname() + "!" + operator_adding->getUsername() + "@" + operator_adding->getHostname();
		msg += " MODE " + this->chan_name + " +o " + operator_to_add->getNickname() + "\r\n";
		if (writeToAllChanUsers(msg) == -1) {
			return (-1);
		}
	}
	return (0);
};

int Channel::deleteOperator(User* operator_deleting, User* operator_to_delete)
{
	std::string					msg;

	for (std::vector<User *>::iterator it = vec_chan_operators.begin(); it != vec_chan_operators.end(); it++) {
		if ((*it) == operator_to_delete) {
			vec_chan_operators.erase(it);
			msg = ":"+ operator_deleting->getNickname() + "!" + operator_deleting->getUsername() + "@" + operator_deleting->getHostname();
			msg += " MODE " + this->chan_name + " -o " + operator_to_delete->getNickname() + "\r\n";
			if (writeToAllChanUsers(msg) == -1) {
				return (-1);
			}
			return (0);
		}
	} 
	return (0);
};

int Channel::addBannedUser(User* user_banning, User* user_to_ban) {
	std::string					msg;

	for (std::vector<User *>::iterator it = vec_chan_users.begin(); it != vec_chan_users.end(); it++) {
		if ((*it) == user_to_ban) {
			vec_chan_banned_users.push_back(user_to_ban);
			msg = ":"+ user_banning->getNickname() + "!" + user_banning->getUsername() + "@" + user_banning->getHostname();
			msg += " MODE " + this->chan_name + " +b " + user_to_ban->getNickname() + "\r\n";
			if (writeToAllChanUsers(msg) == -1) {
				return (-1);
			}
			return (0);
		}
	} 
	return (0);
};

int Channel::deleteBannedUser(User* user_unbanning, User* user_to_unban) {
	std::string					msg;

	for (std::vector<User *>::iterator it = vec_chan_banned_users.begin(); it != vec_chan_banned_users.end(); it++) {
		if ((*it) == user_to_unban) {
			vec_chan_banned_users.erase(it);
			msg = ":"+ user_unbanning->getNickname() + "!" + user_unbanning->getUsername() + "@" + user_unbanning->getHostname();
			msg += " MODE " + this->chan_name + " -b " + user_to_unban->getNickname() + "\r\n";
			if (writeToAllChanUsers(msg) == -1) {
				return (-1);
			}
			return (0);
		}
	} 
	return (0);
};
