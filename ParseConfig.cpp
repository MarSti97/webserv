#include "./includes/webserv.hpp"

template <typename T>
bool	check_duplicate_attr(T attribute, std::string line, size_t counter)
{
	if (!(attribute.empty()))
		throw_parsing_exception(line, 1, counter);
	return true;
}

void	Servers::validate_config()
{
	std::string			line;
	std::istringstream	file(config);
	bool				insideServerBlock = false;
	bool				insideLocationBlock = false;
	size_t				counter = 0;

	while (std::getline(file, line))
	{
		counter++;
		Config temp_config;
		

		std::istringstream s(line);
		std::string token;
		if (s.str().find_first_not_of(" \t") == std::string::npos)
			continue;
		if (!insideServerBlock)
		{
			if (s >> token && token == "server" && s >> token && token == "{" && s.eof())
				insideServerBlock = true;
			else
				throw_parsing_exception(line, 2, counter);
		}
		while (insideServerBlock)
		{
			std::getline(file, line);
			counter++;
			std::istringstream ss(line);
			ss >> token;
			if (ss.str().find_first_not_of(" \t") == std::string::npos)
				continue;
			if (token == "listen" && check_duplicate_attr(temp_config.port, line, counter))
				temp_config.port = parse_attribute(ss, token, line, counter);
			else if (token == "host" && check_duplicate_attr(temp_config.host, line, counter))
				temp_config.host = parse_attribute(ss, token, line, counter);
			else if (token == "client_max_body_size" && check_duplicate_attr(temp_config.max_body_size, line, counter))
				temp_config.max_body_size = parse_attribute(ss, token, line, counter);
			else if (token == "server_name" && check_duplicate_attr(temp_config.server_name, line, counter))
				parseServerNames(ss, token, &temp_config, line, counter);
			else if (token == "error_page")
				parseErrorPages(ss, token, &temp_config, line, counter);
			else if (token == "location" && ss >> token && !(ss.eof()))
			{
				std::string location_name = token;
				if (ss >> token && token == "{" && ss.eof())
				{
					Location temp_location;
					insideLocationBlock = true;
					temp_location.path = location_name;
					while (insideLocationBlock)
					{
						std::getline(file, line);
						counter++;
						std::istringstream iss(line);
						iss >> token;
						if (iss.str().find_first_not_of(" \t") == std::string::npos)
							continue;
						else if (token == "root" && check_duplicate_attr(temp_location.root, line, counter))
							temp_location.root = parse_attribute(iss, token, line, counter);
						else if (token == "index" && check_duplicate_attr(temp_location.index, line, counter))
							temp_location.index = parse_attribute(iss, token, line, counter);	
						else if (token == "autoindex" && check_duplicate_attr(temp_location.autoindex, line, counter))
							temp_location.autoindex = parse_attribute(iss, token, line, counter);	
						else if (token == "cgi" && check_duplicate_attr(temp_location.cgi_extension, line, counter))
							temp_location.cgi_extension = parse_attribute(iss, token, line, counter);	
						else if (token == "return" && check_duplicate_attr(temp_location.redirect_path, line, counter))
							temp_location.redirect_path = parse_attribute(iss, token, line, counter);
						else if (token == "allow" && check_dup_methods(token, line, &temp_location, counter))
							parseMethods(iss, token, &temp_location, line, counter);
						else if (token == "deny" && check_dup_methods(token, line, &temp_location, counter))
							parseMethods(iss, token, &temp_location, line, counter);						
						else if (token == "}" && iss.eof())
						{
							validate_location(temp_location, temp_config.location, line, counter);
							temp_config.location.push_back(temp_location);			
							insideLocationBlock = false;
						}
						else
							throw_parsing_exception(line, 2, counter);
					}
				}
			}
			else if (token == "}" && ss.eof())
			{
				insideServerBlock = false;		
				check_requirements(&temp_config);
				servs.push_back(Serv(temp_config));
			}
			else
				throw_parsing_exception(line, 2, counter);
		}
	}
}
