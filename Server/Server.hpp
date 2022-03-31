#include <vector>
#include <string>
// TODO modify following path
#include "User.hpp"
#include "Channel.hpp"

// TODO maybe put in canonical form ?
class Server {
	private :
		std::vector<User *>		users;
		std::vector<Channel *>	channels; // NOTE must keep track of channels modes

		std::string				name; // NOTE max 63 characters
		std::string				password;
		const char				*port;

	public :
		Server (); // TODO maybe set in private ?
		Server (std::string name, std::string password, int port); // TODO need port ? password ?
		~Server ();


};
