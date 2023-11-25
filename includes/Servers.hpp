#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "webserv.hpp"

class Request;
class Serv;

enum ServSelect
{
	DEFAULT,
	MAXCHECK
};

class Servers
{
	private :
		std::vector<Serv> servs;
		std::string config;
    	std::vector<pollfd> fds;

		void validate_config();
		bool checkContentSizeToMax(Request req, Serv serv);
	
	public : 
		Servers() {}
		Servers(std::string file);
		~Servers();
		Servers &operator=(Servers const &other);

		Request parseRecv(std::vector<pollfd> &fds, int pos);
		void	printAll() const;
		void	init();
		void	run();
		int		checkSockets(int fd);
		bool	getCorrectServ(Request req, int clientfd, ServSelect option);
		int		acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fd);
		Request	postThings(std::string findbuffer, char *buffer, int fd, int size);
		bool 	headcheck(std::string buf);
};


// Exceptions 
class InsufficientInformation : public std::exception {
	public :
		virtual const char* what() const throw() {
			return ("Error while parsing config file\nDescription: Insufficient information on server configuration. \nYou need at least a listen port and a server address,\nas well as a '/' location with a root.");
		}
};

class DuplicateLocation : public std::exception {
	public :
		virtual const char* what() const throw() {
			return ("Description: Duplicate location tag on server configuration.");
		}
};

class DuplicateAttribute : public std::exception {
	public :
		virtual const char* what() const throw() {
			return ("Description: Duplicate attribute on server configuration.");
		}
};

class InvalidLine : public std::exception {
	public :
		virtual const char* what() const throw() {
			return ("Description: Invalid line while parsing configuration file.");
		}
};

class EmptyAttributeValue : public std::exception {
	public :
		virtual const char* what() const throw() {
			return ("Description: Empty value on attribute line.");
		}
};

class UnenclosedAttributeLine : public std::exception {
	public :
		virtual const char* what() const throw() {
			return ("Description: Unenclosed attribute line. Please add a ';' at the end.");
		}
};

class InvalidValues : public std::exception {
	public :
		virtual const char* what() const throw() {
			return ("Description: Attribute line with unexpected number or format of values.");
		}
};

class InvalidLocationName : public std::exception {
	public :
		virtual const char* what() const throw() {
			return "Description: location has invalid pathname.\nPlease write it in the format '/pathname',\nyou may add subpaths by repeating this syntax";
		}
};

class NotConfigFile : public std::exception {
	public :
		virtual const char* what() const throw() {
			return "Error: argument not a config file.";
		}
};

class TooManyArguments : public std::exception {
	public :
		virtual const char* what() const throw() {
			return "Error: too many arguments. Please write only the path to a configuration file.";
		}
};

#endif