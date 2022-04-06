#include "Command.hpp"

using namespace irc;

Command::Command (Server& server, User* user, std::string& content) : server(server), user(user), content(content) {
	this->prefix = "";
}

// TODO check if memory is allocated and need to be free
Command::~Command () {}

void	Command::parseCommand () {
	std::string	first_word = this->getWord();
	if (first_word.empty()) {
		this->prefix = this->content;
		this->content = "";
	} else {
		this->prefix = first_word;
	}
	std::cout << this->prefix << std::endl;
	std::transform(this->prefix.begin(), this->prefix.end(), this->prefix.begin(), ::toupper);
	this->parameters = this->content;
	std::cout << this->parameters << std::endl;
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
	int nbr_cmd = 29;
	void (Command::pmf[nbr_cmd]) = {&Command::intOper, &Command::intJoin, \
		&Command::intTopic, &Command::intMode, &Command::intPart, \
		&Command::intNames, &Command::intList, &Command::intInvite, \
		&Command::intKick, &Command::intPrivmsg, &Command::intNotice, \
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
			this->*pmf[i]();
			return ;
		}
	}
	// TODO send error unknown code 421 if not found
}
