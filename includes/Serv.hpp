#ifndef SERV_HPP
# define SERV_HPP

#include "webserv.hpp"
class Request;

class Serv
{
	private :
		Config serv_info;
		int socketfd;

	public :
		Serv() : serv_info(Config()), socketfd(0) {}
		Serv(Config info) : serv_info(info) {}
		~Serv() {}

		std::vector<pollfd> *fds;

		int		establish_connection();
		void	print(int i) const;
		int		getSocket();
		int		cgi_request(Request req, std::string path_info, std::string script_extension);
		void 	init_cgi_meta_vars(Request req, std::vector<std::string> *meta_vars, std::string abs);
		char	**create_cgi_env(std::vector<std::string> meta_vars);
		int		execute_script(std::string cmd_path, std::string path_info, char **env, Request req);
		bool	compareHostPort(std::string host, std::string port);
		void	filterRequest( Request req );
		void	PrepareResponse( std::string method, std::string path, Request req );
		bool	ext_CGI(std::string path_info);
		int		parseSend(std::string response, int fd, Request req);
		bool	CheckAutoindex( std::string path);
		void	deleteMethod(std::string abs, Request req);
		void	deleteFolderMethod(std::string path, Request req);
		void	chunkedResponse(Request req);
		void	errorPageCheck(std::string code, std::string message, std::string path, Request req);
		bool	redirection(std::string path, Request req);
		bool	compareServerName(std::string ServName);
		bool	deleteFolderRecusively(std::string path);
		int		failToStart(std::string error, struct addrinfo *addr, int socket);

		Methods	CheckAllowed( std::string method, std::string path);
		
		std::string	getHeader( std::string ARG, std::string extra, std::string filePath );
		std::string findcommand(std::string command);
		std::string	findFolder( std::string folder );
		std::string	sendby_CGI(int cgi_fd);
		std::string getResponse(std::string path, std::string file, std::string responseHeaders);
		std::string	CheckIndex( std::string path);
		std::string createAbsolutePath(std::string path);
		std::string	CheckCGI( std::string path);
		std::string getMaxBodySize();
		std::string getServerHostPort();
};

#endif