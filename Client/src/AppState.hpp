#include <vector>

#include "config.hpp"
#include "server_connection.hpp"

using std::vector;

class Appstate {
	vector<ServerConnection> connections;
	Config::config conf;
};
