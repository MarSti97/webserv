#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "webserv.hpp"

class Request;
class Serv;

class Servers
{
	private :
		char **env;
		std::vector<Serv> servs;
		std::string config;
    	std::vector<pollfd> fds;
		void validate_config();
		bool checkContentSizeToMax(char *buffer, ssize_t n, int clientfd);
		bool payloadTooLarge_413;
	
	public : 
		Servers() {}
		Servers(std::string file, char **environment);
		~Servers(); //{std::vector<Serv>().swap(servs); std::vector<pollfd>().swap(fds);}

		Request parseRecv(std::vector<pollfd> &fds, int pos);
		void	printAll() const;
		void	init();
		void	run();
		int		checkSockets(int fd);
		Serv	&getCorrectServ(Request req);
};


// Exceptions 
class InsufficientInformation : public std::exception {
	public :
		InsufficientInformation(){}
		virtual const char* what() const throw() {
			return ("Error: Insufficient information on server configuration \n- You need at least a listen port and a server address.");
		}
};

class DuplicateLocation : public std::exception {
	public :
		DuplicateLocation(){}
		virtual const char* what() const throw() {
			return ("Error: Duplicate location tag on server configuration");
		}
};

class DuplicateAttribute : public std::exception {
	public :
		DuplicateAttribute(){}
		virtual const char* what() const throw() {
			return ("Error: Duplicate attribute on server configuration ");
		}
};

class NotConfigFile : public std::exception {
	public :
		NotConfigFile(){}
		virtual const char* what() const throw() {
			return "Error: argument not a config file.";
		}
};

#endif