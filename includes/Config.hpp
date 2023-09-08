#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"

class Config
{
	private :
		std::string port;
		std::string	host;
		std::string server_name;
		std::string error_pages;
		std::string	body_size;
		std::string methods; // maybe... project makes no sense 
};

#endif