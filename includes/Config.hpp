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
};

class Configfile
{
	private :
		std::vector<Config> config_array;
		std::string config;
	
	public : 
		Configfile() {}
		Configfile(std::string file);
		virtual ~Configfile() {}

		void validate_config();
		void check_requirements();
		void print();
		const char* getPort() {return config_array[0].port.c_str();} // these need fixing! 
		const char* getName() {return config_array[0].server_name[0].c_str();} // figure out how to work with multipe names

	// Exceptions 
	class NotConfigFile : public std::exception {
		public :
			NotConfigFile() {}
			virtual const char* what() const throw() {
				return "Error: argument not a config file.";
			}
	};

	class InsufficientInformation : public std::exception {
		public :
			InsufficientInformation() {}
			virtual const char* what() const throw() {
				return "";
			}
	};
};

#endif