#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Config.hpp"
#include <vector>
#include <poll.h>
#include <sys/wait.h>
#include "request.hpp"
#include "Servers.hpp"

int failToStart(std::string error, struct addrinfo *addr, int socketfd);
std::string	readFile( std::string filePath );
std::string getMimeType(const std::string& filePath);
void	ctrlc(int signum);
void	printlog(std::string msg, int arg);
void	validate_config(void);
void	print_server_config(std::vector<Config> config_array);
bool 	check_new_attribute(std::string token);
int acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fds);
std::string parseRecv(std::vector<pollfd> &fds, int pos);
int parseSend(std::vector<pollfd> &fds, int pos, Request req, char **env);
std::string getResponse(Request req, std::string path, std::string index, char **env);
int GetbyUser(std::string buffer);
int checkAllowGet(std::string folder, std::vector<Location> Location);
int checkAllowPost(std::string folder, std::vector<Location> Location);
int checkAllowDelete(std::string folder, std::vector<Location> Location);
std::string	parse_attribute(std::istringstream &iss, std::string token);
std::string findcommand(std::string command);
// int	execute_command(std::string pathcmd, std::string pathfile, char **env);
bool    correctfile(std::string file);
void	check_requirements(Config temp, int i);

#endif
