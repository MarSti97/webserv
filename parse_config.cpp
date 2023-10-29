#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"

std::string	parse_attribute(std::istringstream &iss, std::string token)
{
	std::string parsed;
	
	if (iss.eof())
		throw EmptyAttributeValue();
	iss >> token;
	if (*(token.end() - 1) == ';' && token.find_first_not_of(";'\"") != std::string::npos && !(iss >> token))
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
	while (iss >> token)
	{
		if (*(token.end() - 1) != ';' && token.find_first_not_of(";'\"") != std::string::npos)
			temp_config->server_name.push_back(token);
		else if (*(token.end() - 1) == ';' && token.find_first_not_of(";'\"") != std::string::npos && !(iss >> token))
			temp_config->server_name.push_back(token.substr(0, token.size() - 1));
		else
		{
			iss >> token;
			throw InvalidNumberValues();
		}
	}
}

void	parseErrorPages(std::istringstream &iss, std::string token, Config *temp_config)
{
	std::string error_number;
	std::string error_pagename;
	if (iss.eof())
		throw EmptyAttributeValue();
	while (iss >> token)
	{
		if (token.find_first_not_of("0123456789") == std::string::npos && !(iss.eof()))
			error_number = token;
		else
			throw InvalidNumberValues();
		if (iss >> token && *(token.end() - 1) == ';' && !(iss >> token))
			error_pagename = token;
		else
			throw UnenclosedAttributeLine();
	}
	if (temp_config->error_pages[error_number].empty())
		temp_config->error_pages[error_number] = error_pagename.substr(0, error_pagename.size() - 1);
	else
		throw DuplicateAttribute();
}

void	parseMethods(std::istringstream &iss, std::string token, Location *temp_location)
{
	if (iss.eof())
		throw EmptyAttributeValue();
	while (iss >> token)
	{
		if (token == "GET" || token == "POST" || token == "DELETE")
			temp_location->methods.insert(std::make_pair(token, true));
		else if (iss >> token)
			throw InvalidNumberValues();
	}
	if (*(token.end() - 1) != ';')
		throw UnenclosedAttributeLine();
	if ((token == "GET;" || token == "POST;" || token == "DELETE;") && !(iss >> token))
		temp_location->methods.insert(std::make_pair(token.substr(0, token.size() - 1), true));
	else
		throw InvalidNumberValues();
}

void	check_requirements(Config *temp)
{
	int error_flag = 0;
	
	// check if there is a "/" location
	std::vector<Location>::iterator it;
	for (it = temp->location.begin(); it != temp->location.end(); it++)
	{
		if (it->path == "/" && !(it->root.empty()))
		{
			temp->autoindex = it->autoindex;
			temp->index = it->index;
			temp->cgi_extension = it->cgi_extension;
			temp->root = it->root;
			temp->redirect_path = it->redirect_path;
			temp->methods = it->methods;
			error_flag = 0;
			break;
		}
		error_flag = 1;
	}

	if (temp->port.empty() || temp->host.empty())
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

bool correctfile(std::string file)
{
	size_t point = file.find(".");
	if (point != std::string::npos)
		if (file.substr(point) != ".config")
			return false;
	return true;
}