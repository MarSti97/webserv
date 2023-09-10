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
						if (token.back() == ';')
							temp_config.port = token;
					}
					else if (token == "server_name")
					{
						while (token.back() != ';')
						{
							iss >> token;
							temp_config.server_names.push_back(token);
						}
					}
					else if (token == "root")
					{
						iss >> token;
						if (token.back() == ';')
							temp_config.root = token;
					}
					else if (token == "client_max_body_size")
					{
						iss >> token;
						if (token.back() == ';')
							temp_config.max_body_size = token;
					}
					else if (token == "autoindex")
					{
						iss >> token;
						if (token.back() == ';')
							temp_config.autoindex = token;
					}
				}
			}
			config_array.push_back(temp_config);
		}
    }
	print_server_config(config_array);
}

void	print_server_config(std::vector<Config> config_array)
{
	for (size_t i = 0; i < config_array.size(); i++)
	{
		std::cout << "port: " << config_array[i].port << std::endl;
		std::cout << "root: " << config_array[i].root << std::endl;
		std::cout << "server_name: ";
		for (size_t j = 0; j < config_array[i].server_names.size(); j++)
		{
			std::cout << config_array[i].server_names[j] << " ";
		}
		std::cout << std::endl;
		std::cout << "max_body_size: " << config_array[i].max_body_size << std::endl;
		std::cout << "autoindex: " << config_array[i].autoindex << std::endl;
	}
	
}