#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


void	validate_config()
{
	std::string	config = readFile("example.config");
	std::istringstream iss(config);
	Config	config_array[100];

	std::string token;
    while (iss >> token) 
	{
        // Process each value here
        std::cout << "Extracted token: " << token << std::endl;

		if (token == "server")
		{
			if (iss >> token && token == "{")
			{
				Config temp_config;
				iss >> token;
				if (token == "listen")
				{
					iss >> token;
					temp_config.port = token;
				}
				else if (token == "server_name")
				{
					iss >> token;
					temp_config.server_name = token;
				}
				else if (token == "root")
				{
					iss >> token;
					temp_config.root = token;
				}
			}
		}
    }

}