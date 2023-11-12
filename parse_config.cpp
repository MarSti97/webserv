#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"

std::string	parse_attribute(std::istringstream &iss, std::string token, std::string line)
{
	std::string parsed;
	
	if (iss.eof())
		throw_parsing_exception(line, 3);
	iss >> token;
	if (*(token.end() - 1) == ';' && token.find_first_not_of(";'\"") != std::string::npos && !(iss >> token))
	{
		if (*(token.end() - 1) == ';')
			parsed = token.substr(0, token.size() - 1);
		else
			throw_parsing_exception(line, 4);
	}
	else
		throw_parsing_exception(line, 5);
	return (parsed);
}

void	parseServerNames(std::istringstream &iss, std::string token, Config *temp_config, std::string line)
{
	if (iss.eof())
		throw_parsing_exception(line, 3);
	while (iss >> token)
	{
		if (*(token.end() - 1) != ';' && token.find_first_not_of(";'\"") != std::string::npos)
			temp_config->server_name.push_back(token);
		else if (*(token.end() - 1) == ';' && token.find_first_not_of(";'\"") != std::string::npos && !(iss >> token))
			temp_config->server_name.push_back(token.substr(0, token.size() - 1));
		else
			throw_parsing_exception(line, 5);
	}
}

void	parseErrorPages(std::istringstream &iss, std::string token, Config *temp_config, std::string line)
{
	std::string error_number;
	std::string error_pagename;
	if (iss.eof())
		throw_parsing_exception(line, 3);
	while (iss >> token)
	{
		if (token.find_first_not_of("0123456789") == std::string::npos && !(iss.eof()))
			error_number = token;
		else
			throw_parsing_exception(line, 5);
		if (iss >> token && *(token.end() - 1) == ';' && !(iss >> token))
			error_pagename = token;
		else
			throw_parsing_exception(line, 4);
	}
	if (temp_config->error_pages[error_number].empty())
		temp_config->error_pages[error_number] = error_pagename.substr(0, error_pagename.size() - 1);
	else
		throw_parsing_exception(line, 1);
}

void	parseMethods(std::istringstream &iss, std::string token, Location *temp_location, std::string line, std::string type)
{
	if (iss.eof())
		throw_parsing_exception(line, 3);
	while (iss >> token)
	{
		if ((token == "GET" || token == "POST" || token == "DELETE") && type == "allow")
			temp_location->methods.insert(std::make_pair(token, ALLOWED));
		else if ((token == "GET" || token == "POST" || token == "DELETE") && type == "deny")
			temp_location->methods.insert(std::make_pair(token, DENIED));
		else if (iss >> token)
			throw_parsing_exception(line, 5);
	}
	if (*(token.end() - 1) != ';')
		throw_parsing_exception(line, 4);
	if ((token == "GET;" || token == "POST;" || token == "DELETE;") && !(iss >> token) && type == "allow")
		temp_location->methods.insert(std::make_pair(token.substr(0, token.size() - 1), ALLOWED));
	else if ((token == "GET;" || token == "POST;" || token == "DELETE;") && !(iss >> token) && type == "deny")
		temp_location->methods.insert(std::make_pair(token.substr(0, token.size() - 1), DENIED));	
	else
		throw_parsing_exception(line, 5);
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

void	validate_location(Location temp_location, std::vector<Location> locations, std::string line)
{
	std::vector<Location>::iterator it;
	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->path == temp_location.path)
			throw_parsing_exception(line, 0);
	}
	if (temp_location.path[0] != '/' || (temp_location.path != "/" && *(temp_location.path.end() - 1) == '/') 
	|| temp_location.path.find("//") != std::string::npos || temp_location.path.find('\\') != std::string::npos)
		throw_parsing_exception(line, 6);
}

bool correctfile(std::string file)
{
	size_t point = file.find(".");
	if (point != std::string::npos)
		if (file.substr(point) != ".config")
			return false;
	return true;
}

void throw_parsing_exception(std::string line, int flag)
{
	std::cerr << "Error while parsing config file\non line: '" << line << "'" << std::endl;
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