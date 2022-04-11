#ifndef UTILITARY_HPP
#define UTILITARY_HPP

#include <string>
#include <vector>
#include <iostream>

namespace irc {
	int							there_is_no (char c, std::string str);
	std::vector<std::string>	split (std::string text, std::string space_delimiter);
};

#endif
