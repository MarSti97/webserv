#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "webserv.hpp"

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