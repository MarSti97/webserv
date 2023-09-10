#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"

class Config
{
	public :
		std::string port;
		std::string	root;
		std::string	max_body_size;
		std::string autoindex;
		std::vector<std::string> server_names;


		std::string error_pages;

		std::string methods; // maybe... project makes no sense 
};

#endif