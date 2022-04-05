#include "Command.hpp"

using namespace irc;

Command::Command(std::string content) : content(content) {};

// TODO check if memory allocated need to be free
Command::~Command() {};

void	Command::parseCommand(){

};
