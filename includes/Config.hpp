#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"

struct Location
{
	bool allow_get;
	bool allow_post;
	bool allow_delete;
	std::string	root;
	std::string index;
	std::string path;
	std::map<std::string, std::string> error_pages;
	std::string redirect_path;
	std::string redirect_status;

	Location(): allow_get(false), allow_post(false), allow_delete(false), root(""), index(""), path(""), error_pages(){}
	~Location() {error_pages.clear();}
};

class Config
{
	public:
		bool allow_get;
		bool allow_post;
		bool allow_delete;
		std::string host;
		std::string port;
		std::string	root;
		std::string index;
		std::string	max_body_size;
		std::string autoindex;
		std::vector<std::string> server_name;
		std::string cgi_extension;
		std::string cgi_directory;
		std::vector<Location> location;
		std::map<std::string, std::string> error_pages;
		Config() : allow_get(false), allow_post(false), allow_delete(false) {}
		~Config() {std::vector<Location>().swap(location); std::vector<std::string>().swap(server_name); error_pages.clear();}
};

#endif