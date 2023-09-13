#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"

void	validate_config()
{
	std::string	config = readFile("example.config");
	std::istringstream iss(config);
	std::vector<Config> config_array;

	std::string token;
    while (iss >> token) 
	{
		if (token == "server")
		{
			Config temp_config;
			if (iss >> token && token == "{")
			{
				while (iss >> token) 
				{
					if (token == "listen")
					{
						iss >> token;
						if (*(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.port = token.substr(0, token.size() - 1);
					}
					else if (token == "server_name")
					{
						while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
						{
							if (*(token.end() - 1) == ';')
								temp_config.server_name.push_back(token.substr(0, token.size() - 1));
							else
								temp_config.server_name.push_back(token);
						}
					}
					else if (token == "root")
					{
						iss >> token;
						if (*(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.root = token.substr(0, token.size() - 1);;
					}
					else if (token == "index")
					{
						iss >> token;
						if (*(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.index = token.substr(0, token.size() - 1);;
					}
					else if (token == "client_max_body_size")
					{
						iss >> token;
						if (*(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.max_body_size = token.substr(0, token.size() - 1);;
					}
					else if (token == "allow")
					{
						while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
						{
							if (token == "GET" || token == "GET;")
								temp_config.allow_get = true;
							else if (token == "POST" || token == "POST;")
								temp_config.allow_post = true;
							else if (token == "DELETE" || token == "DELETE;")
								temp_config.allow_delete = true;
						}
					}
					else if (token == "autoindex")
					{
						iss >> token;
						if (*(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.autoindex = token.substr(0, token.size() - 1);;
					}
					else if (token == "cgi")
					{
						iss >> token;
						if (*(token.end() - 1) != ';' && !check_new_attribute(token))
							temp_config.cgi_extension = token;
						if (iss >> token && *(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.cgi_directory = token.substr(0, token.size() - 1);;						
					}
					else if (token == "location")
					{
						Location temp_location = {0, 0, 0, "", "", ""};
						iss >> token;
						temp_location.path = token;
						if (!check_new_attribute(token) && iss >> token && token == "{")
						{
							while (iss >> token)
							{
								if (token == "allow")
								{
									while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
									{
										if (token == "GET" || token == "GET;")
											temp_location.allow_get = true;
										else if (token == "POST" || token == "POST;")
											temp_location.allow_post = true;
										else if (token == "DELETE" || token == "DELETE;")
											temp_location.allow_delete = true;
									}
								}
								else if (token == "root")
								{
									iss >> token;
									if (*(token.end() - 1) == ';' && !check_new_attribute(token))
										temp_location.root = token.substr(0, token.size() - 1);;
								}
								else if (token == "index")
								{
									iss >> token;
									if (*(token.end() - 1) == ';' && !check_new_attribute(token))
										temp_location.index = token.substr(0, token.size() - 1);;
								}
								else if (token == "}")
								{
									temp_config.location.push_back(temp_location);
									break ;
								}
							}
						}
					}
					else if (token == "}")
					{
						config_array.push_back(temp_config);
						break ;
					}
				}
			}
		}
    }
	print_server_config(config_array);
}

void	print_server_config(std::vector<Config> config_array)
{
	for (size_t i = 0; i < config_array.size(); i++)
	{
		std::cout << "Server Configuration " << i << std::endl;
		std::cout << "port: " << config_array[i].port << std::endl;
		std::cout << "root: " << config_array[i].root << std::endl;
		std::cout << "index: " << config_array[i].index << std::endl;
		std::cout << "server_name: ";
		for (size_t j = 0; j < config_array[i].server_name.size(); j++)
		{
			std::cout << config_array[i].server_name[j] << " ";
		}
		std::cout << std::endl;
		std::cout << "max_body_size: " << config_array[i].max_body_size << std::endl;
		std::cout << (config_array[i].allow_get ? "GET: allowed" : "GET: denied") << std::endl;
		std::cout << (config_array[i].allow_post ? "POST: allowed" : "POST: denied") << std::endl;
		std::cout << (config_array[i].allow_delete ? "DELETE: allowed" : "DELETE: denied") << std::endl;
		std::cout << "autoindex: " << config_array[i].autoindex << std::endl;
		std::cout << "cgi_extension: " << config_array[i].cgi_extension << std::endl;
		std::cout << "cgi_directory: " << config_array[i].cgi_directory << std::endl;
		std::vector<Location>::iterator it;
		for (it = config_array[i].location.begin(); it != config_array[i].location.end(); it++)
		{
			std::cout << std::endl;
			std::cout << "Location " << it->path << std::endl;
			std::cout << "root: " << it->root << std::endl;
			std::cout << "index: " << it->index << std::endl;
			std::cout << (it->allow_get ? "GET: allowed" : "GET: denied") << std::endl;
			std::cout << (it->allow_post ? "POST: allowed" : "POST: denied") << std::endl;
			std::cout << (it->allow_delete ? "DELETE: allowed" : "DELETE: denied") << std::endl;
		}
		
	}
	
}

bool check_new_attribute(std::string token)
{	
	std::string attributes[] = {"listen", "server_name", "root", "client_max_body_size", "autoindex", "cgi", "index", "location"};
	
	for (size_t i = 0; i < 6; i++)
	{
		if (token == attributes[i])
			return true;
	}
	return false;
}