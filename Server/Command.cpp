#include "Command.hpp"

using namespace irc;

Command::Command(Server& server, User* user, std::string& content) : server(server), user(user), content(content) {
	this->prefix = "";
}

// TODO check if memory is allocated and need to be free
Command::~Command() {}

void	Command::parseCommand() {
	std::string	first_word = this->getWord();
	if (first_word.empty()) {
		this->prefix = this->content;
		this->content.clear();
	} else {
		this->prefix = first_word;
	}
	std::transform(this->prefix.begin(), this->prefix.end(), this->prefix.begin(), ::toupper);
	// Check prefix exist
	// Get parameters.
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

bool	Command::checkPrefix () {
	// TODO delete unimplemented functions
	int nbr_cmd = 40;
	void (Command::pmf[]) = {&Command::intOper, &Command::intJoin, \
		&Command::intTopic, &Command::intMode, &Command::intPart, \
		&Command::intNames, &Command::intList, &Command::intInvite, \
		&Command::intKick, &Command::intPrivmsg, &Command::intNotice, \
		&Command::
		});
	std::string msg[] = {"OPER", "JOIN", "TOPIC", "MODE", "PART", "NAMES", \
		"LIST", "INVITE", "KICK", "PRIVMSG", "NOTICE", "KILL", "QUIT", "NICK", \
		"WHOIS", "AWAY", "WALLOPS", "USERHOST", "PASS", "USER", "SQUIT", "LIST", \
		"MOTD", "TIME", "LUSERS", "VERSION", "STATS", "LINKS", "ISON", "RESTART", \
		"PING", "PONG", "ERROR", "DIE", "WHO", "WHOWAS", "SUMMON", "INFO", \
		"TRACE", "ADMIN"};
	// TODO send error unknown code 421 if not found
}
