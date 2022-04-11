#include <cstring>
#include <iostream>
#include <vector>

int main ()
{
	std::string datas = "Ceci est un test\r\nEst-ce que\r\nCa marche !";
	size_t cmd_end = datas.find("\r\n");
	while (cmd_end != std::string::npos) {
		std::string	content = datas.substr(0, cmd_end);
		datas.erase(0, cmd_end + 2);
		cmd_end = datas.find("\r\n");
		std::cout << content << std::endl;
	}
	std::cout << datas << std::endl;
	std::vector<int> vec(10);
	std::vector<int>::iterator it = vec.begin();
	it++;
	std::cout << it - vec.begin() << std::endl;
}
