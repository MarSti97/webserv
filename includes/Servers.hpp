#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "webserv.hpp"

class Servers
{
	private :
		std::vector<Serv> servs;
		std::string config;
		void validate_config();
	
	public : 
		Servers() {}
		Servers(std::string file);
		virtual ~Servers() {}

		void print(); // fix print
		void init();

	// Exceptions 
	class InsufficientInformation : public std::exception {
		public :
			InsufficientInformation() {}
			virtual const char* what() const throw() {
				return "";
			}
	};
};

class NotConfigFile : public std::exception {
	public :
		NotConfigFile() {}
		virtual const char* what() const throw() {
			return "Error: argument not a config file.";
		}
};

class Serv
{
	private :
		Config serv_info;
		int socketfd;
		struct sockaddr_in clientinfo;

	public :
		Serv(Config info) : serv_info(info) {}
		virtual ~Serv() {}

		int establish_connection();
};

#endif