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
		~Serv() {}

		int establish_connection();
		void print(int i) const;
		int	getSocket();
		int	filter_request(Request req);
		int	cgi_request(Request req, std::string path_info, std::string script_extension);
		void 	init_cgi_meta_vars(Request req, std::vector<std::string> *meta_vars);
		char	**create_cgi_env(std::vector<std::string> meta_vars);
		int	execute_script(std::string cmd_path, std::string path_info, char **env, Request req);
		bool compareHostPort(std::string host, std::string port);
		void	filterRequest( Request req );
		void	PrepareResponse( std::string method, std::string path, Request req );
		bool	CheckAllowed( std::string method, std::string path, std::vector<Location> Locations);
		std::string	findFolder( std::string folder, int check );
		bool	ext_CGI(std::string path_info);
		bool	folder_CGI(std::string path);
		std::string	sendby_CGI(int cgi_fd);
		std::string getResponse(std::string path, std::string file, std::string responseHeaders);
		int		parseSend(std::string response, int fd);
		std::string	CheckIndex( std::string path, std::vector<Location> Locations);
		std::string	CheckRoot( std::string path, std::vector<Location> Locations);
		bool	CheckAutoindex( std::string path, std::vector<Location> Locations);

		// void printshit() {
		// 	std::cout << serv_info.cgi_extension << "  THIS SJIT" << std::endl;
		// }
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
		~Servers() {std::vector<Serv>().swap(servs); std::vector<pollfd>().swap(fds);}

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