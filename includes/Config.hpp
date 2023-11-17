#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"

enum Methods
{
	ALLOWED,
	DENIED,
	UNDEFINED,
};


struct Location
{
	std::map<std::string, Methods> methods;
	std::string	root;
	std::string index;
	std::string autoindex;
	std::string path;
	std::string cgi_extension;
	std::string redirect_path;
	std::string redirect_status;
	bool 		allow_limit;
	bool		deny_limit;
	Location(): root(""), index(""), path(""), allow_limit(0), deny_limit(0) {}
	~Location() {}
};

class Config
{
	public:
		std::map<std::string, Methods> methods;
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