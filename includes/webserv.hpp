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
#include <vector>
#include <poll.h>

int failToStart(std::string error, struct addrinfo *addr, int socketfd);
std::string	readFile( std::string filePath );
std::string getMimeType(const std::string& filePath);
std::string getURL(char *buffer);
void	ctrlc(int signum);
void	printlog(std::string msg, int arg);
int acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fds);
int parseRecv(std::vector<pollfd> &fds, int pos, char *buffer);
int parseSend(std::vector<pollfd> &fds, int pos, char *buffer);
std::string getResponse(char *buffer, std::string path, std::string index);


#endif