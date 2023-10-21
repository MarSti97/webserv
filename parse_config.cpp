#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"

std::string	parse_attribute(std::istringstream &iss, std::string token)
{
	std::string parsed;
	
	iss >> token;
	if (*(token.end() - 1) == ';' && !check_new_attribute(token))
		parsed = token.substr(0, token.size() - 1);
	return (parsed);
}


bool check_new_attribute(std::string token)
{	
	std::string attributes[] = {"listen", "server_name", "root", "client_max_body_size", "autoindex", "cgi", "index", "location", "allow"};
	
	for (size_t i = 0; i < 6; i++)
	{
		if (token == attributes[i])
			return true;
	}
	return false;
}

void	check_requirements(Config temp, int i)
{
	if (temp.port.empty() || temp.server_name.empty()) {
		std::cerr << "Error: Insufficient information on server configuration " << i << ".\nYou need at least a listen port and a server address.";
		throw InsufficientInformation();
	}
}

bool correctfile(std::string file)
{
	size_t point = file.find(".");
	if (point != std::string::npos)
		if (file.substr(point) != ".config")
			return false;
	return true;
}