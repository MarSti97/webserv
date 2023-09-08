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

int failToStart(std::string error, struct addrinfo *addr, int socketfd);
std::string	readFile( std::string filePath );
std::string getMimeType(const std::string& filePath);
std::string getURL(char *buffer);
void	ctrlc(int signum);
void	printlog(std::string msg, int arg);
void	validate_config(void);

#endif