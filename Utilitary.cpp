#include "Utilitary.hpp"

using namespace irc;


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

std::vector<std::string>	irc::split(std::string text, std::string space_delimiter)
{
	std::vector<std::string> words;

	size_t pos = 0;
	while ((pos = text.find_first_of(space_delimiter)) != std::string::npos)
	{
		std::cout << "substr: " << text.substr(0) << std::endl;
		if (text[0] == ':')
		{
			words.push_back(text.substr(0));
			return (words);
		}
		words.push_back(text.substr(0, pos));
		int nbDelimiters = 0;
		while (irc::there_is_no(text[pos + nbDelimiters], space_delimiter) == 0)
			nbDelimiters++;
		text.erase(0, pos + nbDelimiters);
	}
	words.push_back(text.substr(0));
	words.push_back("\0");
	return (words);
}

int	irc::sendData(int client_fd, const void* data, int data_size) {
	const char*	data_ptr = (const char*) data;
	int bytes_sent;

	while (data_size > 0) {
		std::cout<< "send" << std::endl;
		bytes_sent = send(client_fd, data_ptr, data_size, 0);
		if (bytes_sent == -1) {
			return (-1);
		}
		data_ptr += bytes_sent;
		data_size -= bytes_sent;
	}
	return (0);
}

int irc::sendString(int client_fd, const std::string& data) {
	ulong	data_size = htonl(data.size());

	std::cout << client_fd << " " << data << std::endl;
	int result = sendData(client_fd, &data_size, sizeof(data_size));
	if (result == 0) {
		result = sendData(client_fd, data.c_str(), data.size());
	}
	return result;
}
