#include <map>
#include <iostream>
#include <string>
#include <fstream>

int	Server::readConfFile () {
	std::ifstream	in;
	int				res = 0;
	std::string		line;
	std::string		key;
	std::string		value = "";
    ssize_t         found;

	in.open("./ft_irc.conf", std::ifstream::in);
	if (!in.good()) {
		std::cerr << "Can't open configuration file" << std::endl;
		std::cerr << "Please check that ./ft_irc.conf is present in Server folder" << std::endl; 
		return (-1);
	}
	while (!in.eof() && in.good()) {
        value = "";
		std::getline(in, line);
        if ((found = line.find_first_not_of(' ')) != std::string::npos) {
        	line.erase(0, found);
		}
        if (line.empty() || line[found] == '#') {
            continue;
        } else if ((found = line.find("=\"")) == std::string::npos) {
			std::cerr << "Should be under format key=\"value\"" << std::endl;
			res = -1;
			break;
		}
		key = line.substr(0, found);
		line.erase(0, found + 2);
		while ((found = line.find('"')) == std::string::npos) {
			value.append(line);
			std::getline(in, line);
			value.append(" ");
		}
		value += line.substr(0, found);
		line.erase(0, found + 1);
		if (!line.empty()) {
			std::cerr << "Error in configuration file" << std::endl;
			std::cerr << "key=\"value\" should be followed by a line return" << std::endl;
			res = -1;
			break;
		}
		this->conf.insert(std::make_pair(key, value));
	}
	if (!in.eof()) {
		std::cerr << "Error while reading configuration file" << std::endl;
		res = -1;
	}
	in.close();
	return res;
}

int	checkConf () {
	if (this->conf.count("name") == 0 || this->conf.count("version") == 0
		|| this->conf.count("adminloc1") == 0 || this->conf.count("adminemail") == 0) {
		std::cerr << "Missing at least one non-optionnal configuration parameter" << std::endl;
		return (-1);
	}
	if ((this->conf.find("name")->second).size() > 63) {
		std::cerr << "Server name should be under 63 characters (please, modify .conf file)" << std::endl;
		return (-1);
	}
	return 0;
}

int main () {
    std::map<std::string, std::string> map;
    int res = readConfFile(map);
    for (std::map<std::string, std::string>::iterator it = map.begin(); 
        it != map.end(); it++) {
            std::cout << (*it).first << " => " << (*it).second << std::endl;
    }
    std::cout << res << std::endl;
}