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

std::vector<User>	Channel::getVecChanUsers() {
	return (vec_chan_users);
};

int	Channel::getNbUsersInChan() {
	return (vec_chan_users.size());
};

int	Channel::getMaxNbUsersInChan() {
	return (max_nb_users_in_chan);
};

std::vector<User>	Channel::getChanOperators() {
	return (vec_chan_operators);
};

std::string	Channel::getChanCreator() {
	return (chan_creator);
};

std::vector<User>	Channel::getVecChanBannedUsers() {
	return (vec_chan_banned_users);
};

std::string	Channel::getChanNameAndTopic() {
	std::string ret;

	ret = "NAME: " + chan_name + " - TOPIC: " + chan_topic;
	return (ret);
};

User *Channel::getUserFromUsername(std::string username) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i].getUsername() == username)
			return (&vec_chan_users[i]);
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
	while (i < vec_chan_operators.size())
	{
		if (vec_chan_operators[i].getNickname() == user->getNickname())
			return (1);
		i++;
	}
	return (0);
}

void	Channel::setChanTopic(std::string topic, User* user_who_changes) {
	if (irc::there_is_no('t', chan_mode) || (irc::there_is_no('t', chan_mode) == 0 && isOperator(user_who_changes)))
		chan_topic = topic;
};

void	Channel::setChanMode(std::string mode) {
	chan_mode += mode;
};

void	Channel::setVecChanUsers(std::vector<User> vec_users) {
	vec_chan_users = vec_users;
};

void	Channel::setMaxNbUsersInChan(int max_nb) {
	if (max_nb > 0)
		max_nb_users_in_chan = max_nb;
};

void	Channel::setChanOperators(std::vector<User> vec_operators) {
	vec_chan_operators = vec_operators;
};

void	Channel::setChanCreator(std::string creator) {
	chan_creator = creator;
};

void	Channel::setVecChanBannedUsers(std::vector<User> vec_banned_users) {
	vec_chan_banned_users = vec_banned_users;
};

int	Channel::userIsInChanFromUsername(std::string username_to_search) {
	unsigned long i;

	i = 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i].getUsername() == username_to_search)
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
		if (vec_chan_users[i].getNickname() == nickname_to_search)
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
		if (vec_chan_banned_users[i].getUsername() == username_to_search)
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
	message += " INVITE " + user_invited->getNickname() + " :" + this->chan_name;
	ret = irc::sendString(user_invited->getFd(), message);
	if (ret == -1)
	{
		std::cerr << "Could not send invitation\n";
		this->server->deleteUser(user_invited);
		return (-1);
	}
	return (0);
};

// TODO modify for RPL_NAMREPLY response 
int Channel::listAllUsersInChan(User* user_asking)
{
	unsigned long				i;
	int							ret;
	std::string					names;
	std::vector<std::string>	params;

	i = 0;
	// Channel name after = if public, @ if secret, * if private
	// =/*/@channel :
	// params.push_back(channel_type);
	while (i < vec_chan_users.size())
	{
		//In front of nick: @ if operator, + if permission to speak on moderated channel
		names += vec_chan_users[i].getNickname();
		if (i < vec_chan_users.size() - 1)
			names += " ";
		i++;
		params.push_back(names);
	}
	params.push_back(names);
	ret = irc::numericReply(353, user_asking, params);
	if (ret == -1)
	{
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
		ret = irc::sendString(vec_chan_users[i].getFd(), sentence_to_send);
		if (ret == -1)
		{
			std::cerr << "Could not send message to user " << vec_chan_users[i].getNickname() << "\n";
			return (-1);
		}
		i++;
	}
	return (0);
}

int Channel::addUser(User & user_to_add)
{
	std::vector<User>::iterator	found;
	unsigned long				i;
	std::string					user_added;
	int							ret;

	// std::cout << "DANS ADDUSER\n";
	if (vec_chan_users.size() == max_nb_users_in_chan)
	{
		ret = writeToAllChanUsers("Cannot add user: channel is full\n");
		if (ret == -1) 
			return (1);
		std::cerr << "Cannot add user: channel is full\n";
		return (1);
	}
	found = std::find(vec_chan_users.begin(), vec_chan_users.end(), user_to_add);
	if (found == vec_chan_users.end())
	{
		ret = writeToAllChanUsers("Cannot add user: already exists\n");
		if (ret == -1)
			return (-1);
		// std::cout << "1 - vec_chan_users.size(): " << vec_chan_users.size() << std::endl << std::endl;
		return (1);
	}
	i = 0;
	// std::cout << "2 - vec_chan_users.size(): " << vec_chan_users.size() << std::endl << std::endl;
	return 0;
	while (i < vec_chan_users.size())
	{
		if (vec_chan_users[i].getUsername() == user_to_add.getUsername())
		{
			ret = writeToAllChanUsers("Cannot add user: username already exists\n");
			if (ret == -1)
				return (1);
			std::cerr << "Cannot add user: username already exists\n";
			return (1);
		}
		if (vec_chan_users[i].getNickname() == user_to_add.getNickname())
		{
			ret = writeToAllChanUsers("Cannot add user: nickname already exists\n");
			if (ret == -1)
				return (1);
			std::cerr << "Cannot add user: nickname already exists\n";
			return (1);
		}
		i++;
	}
	vec_chan_users.push_back(user_to_add);
	user_added = user_to_add.getNickname() + " joined the channel " + chan_name + "\n";
	ret = writeToAllChanUsers(user_added);
	if (ret == -1)
		return (1);
	user_to_add.addChannel(chan_name);
	return (0);
};

int Channel::deleteUser(User & user_to_delete, std::string message)
{
	std::vector<User>::iterator	found;
	int							ret;

	found = std::find(vec_chan_users.begin(), vec_chan_users.end(), user_to_delete);
	if (found == vec_chan_users.end())
	{
		std::cerr << "Cannot delete user: not found\n";
		return (1);
	}
	vec_chan_users.erase(found);
	if (message.empty())
		message = user_to_delete.getNickname() + " has quit the channel " + chan_name + "\n";
	ret = writeToAllChanUsers(message);
	if (ret == -1)
		return (1);
	user_to_delete.deleteChannel(chan_name);
	return (0);
};

int Channel::addOperator(User & operator_to_add)
{
	std::vector<User>::iterator	found;
	int							ret;
	std::string					operator_added;

	found = std::find(vec_chan_operators.begin(), vec_chan_operators.end(), operator_to_add);
	if (found != vec_chan_operators.end())
	{
		std::cerr << "Cannot add operator: already exists\n";
		return (1);
	}
	vec_chan_operators.push_back(operator_to_add);
	operator_to_add.getNickname().insert(0, "@");
	operator_added = operator_to_add.getNickname() + " is now operator in channel " + chan_name + "\n";
	ret = writeToAllChanUsers(operator_added);
	if (ret == -1)
		return (1);
	return (0);
};

int Channel::deleteOperator(User & operator_to_delete)
{
	std::vector<User>::iterator	found;
	int							ret;
	std::string					operator_deleted;

	found = std::find(vec_chan_operators.begin(), vec_chan_operators.end(), operator_to_delete);
	if (found != vec_chan_operators.end())
	{
		std::cerr << "Cannot delete operator: not found\n";
		return (1);
	}
	vec_chan_operators.erase(found);
	operator_deleted = operator_to_delete.getNickname() + " is no longer operator in channel " + chan_name + "\n";
	ret = writeToAllChanUsers(operator_deleted);
	if (ret == -1)
		return (1);
	return (0);
};

int Channel::addBannedUser(User & user_to_ban)
{
	std::vector<User>::iterator	found;
	std::string					user_banned;
	// int							ret;

	found = std::find(vec_chan_banned_users.begin(), vec_chan_banned_users.end(), user_to_ban);
	if (found != vec_chan_banned_users.end())
	{
		std::cerr << "Cannot ban user: already banned\n";
		return (1);
	}
	user_banned = user_to_ban.getNickname() + " is now banned from channel " + chan_name + "\n";
	deleteUser(user_to_ban, user_banned);
	vec_chan_banned_users.push_back(user_to_ban);
	// ret = writeToAllChanUsers(user_banned);
	// if (ret == -1)
	// 	return (1);
	return (0);
};

int Channel::deleteBannedUser(User & user_to_unban)
{
	std::vector<User>::iterator	found;
	std::string					user_unbanned;
	int							ret;

	found = std::find(vec_chan_banned_users.begin(), vec_chan_banned_users.end(), user_to_unban);
	if (found != vec_chan_banned_users.end())
	{
		std::cerr << "Cannot unban user: not found in ban list\n";
		return (1);
	}
	vec_chan_banned_users.erase(found);
	user_unbanned = user_to_unban.getNickname() + " is now unbanned from channel " + chan_name + "\n";
	ret = writeToAllChanUsers(user_unbanned);
	if (ret == -1)
		return (1);
	return (0);
};
