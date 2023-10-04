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
#include <ctime>
#include "request.hpp"
#include "Servers.hpp"

#define RED "\033[0;91m"
#define GREEN "\033[0;92m"
#define YELLOW "\033[0;93m"
#define NOCOLOR "\033[0m"

class Request;

std::string makeStamp( void );
int failToStart(std::string error, struct addrinfo *addr, int socketfd);
std::string	readFile( std::string filePath );
std::string getMimeType(const std::string& filePath);
void	ctrlc(int signum);
void	printlog(std::string msg, int arg, std::string color);
bool 	check_new_attribute(std::string token);
int acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fds);
std::string parseRecv(std::vector<pollfd> &fds, int pos);
int parseSend(std::vector<pollfd> &fds, int pos, Request req, int cgi_fd);
std::string getResponse(Request req, std::string path, std::string index, int cgi_fd);
int GetbyUser(std::string buffer);
int checkAllowGet(std::string folder, std::vector<Location> Location);
int checkAllowPost(std::string folder, std::vector<Location> Location);
int checkAllowDelete(std::string folder, std::vector<Location> Location);
std::string	parse_attribute(std::istringstream &iss, std::string token);
std::string findcommand(std::string command);
bool    correctfile(std::string file);

#endif
