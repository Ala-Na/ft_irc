#include "Utilitary.hpp"

using namespace irc;

std::string irc::left_trim(const std::string &s, std::string to_remove)
{
	if (s.size() == 0)
		return (NULL);
	size_t start = s.find_first_not_of(to_remove);
	if (start == std::string::npos)
		return (NULL);
	return (s.substr(start));
}

std::string irc::right_trim(const std::string &s, std::string to_remove)
{
	if (s.size() == 0)
		return (NULL);
	size_t end = s.find_last_not_of(to_remove);
	if (end == std::string::npos)
		return (NULL);
	return (s.substr(0, end + 1));
}

std::string irc::trim(const std::string &s, std::string to_remove)
{
	if (s.size() == 0)
		return (NULL);
	return (irc::right_trim(irc::left_trim(s, to_remove), to_remove));
}

std::string irc::ft_toupper_str(std::string& str)
{
	std::string new_str = str;
	std::transform(new_str.begin(), new_str.end(), new_str.begin(), ::toupper);
	return new_str;
}

int	irc::there_is_no(char c, std::string str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (0);
		i++;
	}
	return (1);
};

std::vector<std::string>	irc::split(std::string text, std::string space_delimiter, int dot)
{
	std::vector<std::string> words;

	size_t pos = 0;
	while ((pos = text.find_first_of(space_delimiter)) != std::string::npos)
	{
		if (text[0] == ':' && dot == 0)
		{
			words.push_back(text.substr(0));
			return (words);
		}
		else if (text[0] == ':')
		{
			text = trim(text, ":");
			continue ;
		}
		words.push_back(text.substr(0, pos));
		int nbDelimiters = 0;
		while (irc::there_is_no(text[pos + nbDelimiters], space_delimiter) == 0)
			nbDelimiters++;
		text.erase(0, pos + nbDelimiters);
	}
	words.push_back(text.substr(0));
	return (words);
}

int irc::sendString(int client_fd, const std::string& data) {
	int result = send(client_fd, data.c_str(), data.length(), MSG_NOSIGNAL);
	if (result == -1) {
		std::cerr << "Error: send() with client fd = " << client_fd << std::endl;
	}
	return result;
}
