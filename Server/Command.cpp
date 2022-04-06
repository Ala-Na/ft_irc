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
	} else {
		this->prefix = first_word;
	}
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
