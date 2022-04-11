#include <map>
#include <iostream>
#include <string>
#include <fstream>

int readConfFile (std::map<std::string, std::string>& map) {
	std::ifstream	in;
	int				res = 0;
	std::string		line;
	std::string		key;
	std::string		value = "";
    ssize_t         found;

	in.open("./ft_irc.conf", std::ifstream::in);
	if (!in.good()) {
		res = -1;
	}
	while (!in.eof() && in.good()) {
        value = "";
		std::getline(in, line);
        found = line.find_first_not_of(' ');
        line.erase(0, found);
        if (line.empty() || line[found] == '#') {
            continue;
        }
		if ((found = line.find("=\"")) == std::string::npos) {
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
		map.insert(std::make_pair(key, value));
	}
	if (!in.eof()) {
		res = -1;
	}
	in.close();
	return res;
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