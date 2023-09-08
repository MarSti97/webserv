#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"

class Config
{
	public :
		std::string port;
		std::string server_name;
		std::string	root;
		
		std::string error_pages;
		std::string	body_size;
		std::string methods; // maybe... project makes no sense 
};

#endif