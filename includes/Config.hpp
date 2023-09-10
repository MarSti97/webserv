#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"

struct Location
{
	
}

struct Config
{
		std::string port;
		std::string	root;
		std::string	max_body_size;
		std::string autoindex;
		std::vector<std::string> server_name;
		std::string cgi_extension;
		std::string cgi_directory;
		std::vector<Location> location;


		std::string error_pages;
};

#endif