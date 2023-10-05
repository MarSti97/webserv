#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "webserv.hpp"
class Request;

class Serv
{
	private :
		Config serv_info;
		int socketfd;

	public :
		Serv(Config info) : serv_info(info) {}
		virtual ~Serv() {}

		int establish_connection();
		void print(int i) const;
		int	getSocket();
		void	filter_request(Request &req);
		void	cgi_request(Request &req, std::string path_info, std::string script_extension);
		void 	init_cgi_meta_vars(Request &req, std::vector<std::string> *meta_vars);
		char	**create_cgi_env(std::vector<std::string> meta_vars);
		int	execute_script(std::string cmd_path, std::string path_info, char **env, Request &req);
		bool compareHostPort(std::string host, std::string port);
};

class Servers
{
	private :
		char **env;
		std::vector<Serv> servs;
		std::string config;
    	std::vector<pollfd> fds;
		void validate_config();
	
	public : 
		Servers() {}
		Servers(std::string file, char **environment);
		virtual ~Servers() {}

		void	printAll() const;
		void	init();
		void	run();
		int		checkSockets(int fd);
		Serv	&getCorrectServ(Request *req);
};


// Exceptions 
class InsufficientInformation : public std::exception {
	public :
		InsufficientInformation() {}
		virtual const char* what() const throw() {
			return "";
		}
};

class NotConfigFile : public std::exception {
	public :
		NotConfigFile() {}
		virtual const char* what() const throw() {
			return "Error: argument not a config file.";
		}
};

#endif