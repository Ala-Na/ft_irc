#ifndef UTILITARY_HPP
#define UTILITARY_HPP

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

namespace irc {
	std::string					ft_toupper_str(std::string& str);
	int							there_is_no (char c, std::string str);
	std::vector<std::string>	split (std::string text, std::string space_delimiter);
};

#endif
