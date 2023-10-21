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
	std::string attributes[] = {"listen", "host", "error_page", "server_name", "root", "client_max_body_size", "autoindex", "cgi", "index", "location", "allow", "return"};
	
	for (size_t i = 0; i < 6; i++)
	{
		if (token == attributes[i])
			return true;
	}
	return false;
}

void	check_requirements(Config temp)
{
	int error_flag = 0;
	
	// check if there is a "/" location
	std::vector<Location>::iterator it;
	for (it = temp.location.begin(); it != temp.location.end(); it++)
	{
		std::cout << it->path << " | " << it->root << std::endl;
		if (it->path == "/" && !(temp.root.empty()))
		{
			std::cout << "GOOD" << std::endl;
			error_flag = 0;
			break;
		}
		error_flag = 1;
		std::cout << "HERELOC" << std::endl;
	}

	// check if there's a host and port
	if (temp.port.empty() || temp.host.empty()) {
		std::cout << "HEREATTR" << std::endl;
		error_flag = 1;
	}

	if (error_flag == 1)
		// std::cerr << "Error: Insufficient information on server configuration " << i << ".\nYou need at least a listen port and a server address.";
		throw InsufficientInformation();
}

void	check_duplicate_location(Location temp_location, std::vector<Location> locations)
{
	std::vector<Location>::iterator it;
	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->path == temp_location.path)
			throw DuplicateLocation();
	}
}

bool	check_duplicate_attr(std::string attribute)
{
	//std::cout << attribute << std::endl;
	if (!(attribute.empty()))
		throw DuplicateAttribute();
	return true;
}

bool correctfile(std::string file)
{
	size_t point = file.find(".");
	if (point != std::string::npos)
		if (file.substr(point) != ".config")
			return false;
	return true;
}