#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"

std::string	parse_attribute(std::istringstream &iss, std::string token, std::string line, size_t counter)
{
	std::string parsed;
	
	if (iss.eof())
		throw_parsing_exception(line, 3, counter);
	iss >> token;
	if (*(token.end() - 1) == ';' && token.find_first_not_of(";'\"") != std::string::npos && !(iss >> token))
	{
		if (*(token.end() - 1) == ';')
			parsed = token.substr(0, token.size() - 1);
		else
			throw_parsing_exception(line, 4, counter);
	}
	else
		throw_parsing_exception(line, 5, counter);
	return (parsed);
}

void	parseServerNames(std::istringstream &iss, std::string token, Config *temp_config, std::string line, size_t counter)
{
	if (iss.eof())
		throw_parsing_exception(line, 3, counter);
	while (iss >> token)
	{
		if (*(token.end() - 1) != ';' && token.find_first_not_of(";'\"") != std::string::npos)
			temp_config->server_name.push_back(token);
		else if (*(token.end() - 1) == ';' && token.find_first_not_of(";'\"") != std::string::npos && !(iss >> token))
			temp_config->server_name.push_back(token.substr(0, token.size() - 1));
		else
			throw_parsing_exception(line, 5, counter);
	}
}

void	parseErrorPages(std::istringstream &iss, std::string token, Config *temp_config, std::string line, size_t counter)
{
	std::string error_number;
	std::string error_pagename;
	if (iss.eof())
		throw_parsing_exception(line, 3, counter);
	while (iss >> token)
	{
		if (token.find_first_not_of("0123456789") == std::string::npos && !(iss.eof()))
			error_number = token;
		else
			throw_parsing_exception(line, 5, counter);
		if (iss >> token && *(token.end() - 1) == ';' && !(iss >> token))
			error_pagename = token;
		else
			throw_parsing_exception(line, 4, counter);
	}
	if (temp_config->error_pages[error_number].empty())
		temp_config->error_pages[error_number] = error_pagename.substr(0, error_pagename.size() - 1);
	else
		throw_parsing_exception(line, 1, counter);
}

void	parseMethods(std::istringstream &iss, std::string token, Location *temp_location, std::string line, size_t counter)
{
	std::string type = token;
	if (iss.eof())
		throw_parsing_exception(line, 3, counter);
	while (iss >> token)
	{
		if ((token == "GET" || token == "POST" || token == "DELETE") && type == "allow")
			temp_location->methods[token] = ALLOWED;
		else if ((token == "GET" || token == "POST" || token == "DELETE") && type == "deny")
			temp_location->methods[token] = DENIED;
		else if (iss >> token)
			throw_parsing_exception(line, 5, counter);
	}
	if (*(token.end() - 1) != ';')
		throw_parsing_exception(line, 4, counter);
	if ((token == "GET;" || token == "POST;" || token == "DELETE;") && !(iss >> token) && type == "allow")
	{
		temp_location->allow_limit = true;
		temp_location->methods[token.substr(0, token.size() - 1)] = ALLOWED;
	}
	else if ((token == "GET;" || token == "POST;" || token == "DELETE;") && !(iss >> token) && type == "deny")
	{
		temp_location->deny_limit = true;
		temp_location->methods[token.substr(0, token.size() - 1)] = DENIED;
	}
	else
		throw_parsing_exception(line, 5, counter);
}

bool	check_dup_methods(std::string token, std::string line, Location *temp_location, size_t counter)
{
	if ((token == "allow" && temp_location->allow_limit) || (token == "deny" && temp_location->deny_limit))
		throw_parsing_exception(line, 1, counter);
	return true;
}

void	check_requirements(Config *temp)
{
	int error_flag = 0;
	
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

void	validate_location(Location temp_location, std::vector<Location> locations, std::string line, size_t counter)
{
	std::vector<Location>::iterator it;
	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->path == temp_location.path)
			throw_parsing_exception(line, 0, counter);
	}
	if (temp_location.path[0] != '/' || (temp_location.path != "/" && *(temp_location.path.end() - 1) == '/') 
	|| temp_location.path.find("//") != std::string::npos || temp_location.path.find('\\') != std::string::npos)
		throw_parsing_exception(line, 6, counter);
}

bool correctfile(std::string file)
{
	size_t point = file.find(".");
	if (point != std::string::npos)
		if (file.substr(point) == ".config")
			return true;
	return false;
}

void throw_parsing_exception(std::string line, int flag, size_t counter)
{
	std::string linenumber;
	if (counter != 0)
	{
		std::stringstream ss;
		ss << counter;
		linenumber = ss.str();
	}
	if (line.empty())
		printerr("Error while parsing config file", -2, RED);
	else
		printerr("Error while parsing config file\non line " + linenumber + ": '" + line + "'", -2, RED);
	switch (flag)
	{
		case 0:
			throw DuplicateLocation();
		case 1:
			throw DuplicateAttribute();
		case 2:
			throw InvalidLine();
		case 3:
			throw EmptyAttributeValue();
		case 4:
			throw UnenclosedAttributeLine();
		case 5:
			throw InvalidValues();
		case 6:
			throw InvalidLocationName();
	}
}