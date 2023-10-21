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
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <poll.h>
#include <cstdio>
#include <sys/wait.h>
#include <ctime>

#include "Disposition.hpp"
#include "Content.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Servers.hpp"
#include "Serv.hpp"
#include "Download.hpp"

#define RED "\033[0;91m"
#define GREEN "\033[0;92m"
#define YELLOW "\033[0;93m"
#define NOCOLOR "\033[0m"

class Request;
class Download;

std::string makeStamp( void );
int failToStart(std::string error, struct addrinfo *addr, int socketfd);
std::string	readFile( std::string filePath );
std::string getMimeType(const std::string& filePath);
void	ctrlc(int signum);
void	printlog(std::string msg, int arg, std::string color);
int acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fds);
int GetbyUser(std::string buffer);
int checkAllowGet(std::string folder, std::vector<Location> Location);
int checkAllowPost(std::string folder, std::vector<Location> Location);
int checkAllowDelete(std::string folder, std::vector<Location> Location);
std::string findcommand(std::string command);
bool    correctfile(std::string file);
void    createPost(char *buf, int client, int checker);
Request postThings(std::string findbuffer, char *buffer, int fd, int size);
char *strjoin(char *str1, char *str2, int sizestr1, int sizestr2);
int end_loop(int end);
std::string	makeDirectoryList(std::string dirpath, std::string rel_path);
std::string getHeader( std::string ARG, std::string extra, std::string filePath );
std::string	parse_attribute(std::istringstream &iss, std::string token);
void	check_duplicate_location(Location temp_location, std::vector<Location> locations);
bool	check_duplicate_attr(std::string attribute);
void	check_requirements(Config temp);
bool 	check_new_attribute(std::string token);


// tools
int removehead(char *file);
bool compare(const char *find, char* str);
int removeheadnoimg(char *file, int size);
std::string	getINFOone(std::string request, const char *what, int pos);
std::string	getINFOtwo(std::string request, const char *what, int pos);


#endif
