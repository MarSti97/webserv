#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
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


int failToStart(std::string error, struct addrinfo *addr, int socketfd);
std::string	readFile( std::string filePath );
std::string getMimeType(const std::string& filePath);
std::string getURL(char *buffer);
void	ctrlc(int signum);
void	printlog(std::string msg, int arg);
void	validate_config(void);
void	print_server_config(std::vector<Config> config_array);

#endif