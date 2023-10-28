#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"

// std::string	parse_attribute(std::istringstream &iss, std::string token)
// {
// 	std::string parsed;
	
// 	iss >> token;
// 	if (*(token.end() - 1) == ';' && !check_new_attribute(token))
// 		parsed = token.substr(0, token.size() - 1);
// 	return (parsed);
// }

std::string	parse_attribute(std::istringstream &iss, std::string token)
{
	std::string parsed;
	
	if (iss.eof())
		throw EmptyAttributeValue();
	iss >> token;
	if (iss.eof())
	{
		if (*(token.end() - 1) == ';')
			parsed = token.substr(0, token.size() - 1);
		else
			throw UnenclosedAttributeLine();
	}
	else
		throw InvalidNumberValues();
	return (parsed);
}

void	parseServerNames(std::istringstream &iss, std::string token, Config *temp_config)
{
	if (iss.eof())
		throw EmptyAttributeValue();
	while (!(iss.eof()))
	{
		iss >> token;
		if (*(token.end() - 1) != ';')
			temp_config->server_name.push_back(token);
		else if (*(token.end() - 1) == ';' && iss.eof())
			temp_config->server_name.push_back(token.substr(0, token.size() - 1));
		else
			throw InvalidNumberValues();
	}
}

void	parseErrorPages(std::istringstream &iss, std::string token, Config *temp_config)
{
	std::string error_number;
	std::string error_pagename;
	if (iss.eof())
		throw EmptyAttributeValue();
	while (!(iss.eof()))
	{
		iss >> token;
		if (token.find_first_not_of("0123456789") == std::string::npos && !(iss.eof()))
			error_number = token;
		else
			throw InvalidNumberValues();
		if (iss >> token && *(token.end() - 1) == ';' && iss.eof())
			error_pagename = token;
		else
			throw UnenclosedAttributeLine();
	}
	if (temp_config->error_pages[error_number].empty())
		temp_config->error_pages[error_number] = error_pagename.substr(0, error_pagename.size() - 1);
	else
		throw DuplicateAttribute();
}


// bool check_new_attribute(std::string token)
// {	
// 	std::string attributes[] = {"listen", "host", "error_page", "server_name", "root", "client_max_body_size", "autoindex", "cgi", "index", "location", "allow", "return"};
	
// 	for (size_t i = 0; i < 6; i++)
// 	{
// 		if (token == attributes[i])
// 			return true;
// 	}
// 	return false;
// }

void	check_requirements(Config temp)
{
	int error_flag = 0;
	
	// check if there is a "/" location
	std::vector<Location>::iterator it;
	for (it = temp.location.begin(); it != temp.location.end(); it++)
	{
		if (it->path == "/" && !(temp.root.empty()))
		{
			error_flag = 0;
			break;
		}
		error_flag = 1;
	}

	if (temp.port.empty() || temp.host.empty())
		error_flag = 1;
	if (error_flag == 1)
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