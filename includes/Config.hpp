#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"

struct Location
{
	std::map<std::string, bool> methods;
	std::string	root;
	std::string index;
	std::string autoindex;
	std::string path;
	std::string cgi_extension;
	std::string redirect_path;
	std::string redirect_status;

	Location(): root(""), index(""), path("") {}
	~Location() {}
};

class Config
{
	public:
		std::map<std::string, bool> methods;
		std::string host;
		std::string port;
		std::string	root;
		std::string index;
		std::string	max_body_size;
		std::string autoindex;
		std::string redirect_path;
		std::vector<std::string> server_name;
		std::string cgi_extension;
		std::string cgi_directory;
		std::vector<Location> location;
		std::map<std::string, std::string> error_pages;
		
		Config() {}
		~Config() {std::vector<Location>().swap(location); std::vector<std::string>().swap(server_name); error_pages.clear();}
};

#endif