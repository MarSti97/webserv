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
#include <cstdlib>

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
#define BLUE "\033[0;94m"
#define PURPLE "\033[0;95m"
#define CYAN "\033[0;96m"
#define NOCOLOR "\033[0m"

class Request;
class Download;

// open_config
std::string	readFile( std::string filePath );
void initializeMimeTypes(std::map<std::string, std::string> &mimeTypes);
std::string getMimeType(const std::string& filePath);

// parse_config
std::string	parse_attribute(std::istringstream &iss, std::string token);
bool 	check_new_attribute(std::string token);
bool    correctfile(std::string file);

std::string makeStamp( void );
std::string	makeDirectoryList(std::string dirpath, std::string rel_path);
std::string	parse_attribute(std::istringstream &iss, std::string token, std::string line, size_t counter);
void	validate_location(Location temp_location, std::vector<Location> locations, std::string line, size_t counter);
void	parseMethods(std::istringstream &iss, std::string token, Location *temp_location, std::string line, size_t counter);
void	check_requirements(Config *temp);
void	parseServerNames(std::istringstream &iss, std::string token, Config *temp_config, std::string line, size_t counter);
void	parseErrorPages(std::istringstream &iss, std::string token, Config *temp_config, std::string line, size_t counter);
void 	throw_parsing_exception(std::string line, int flag, size_t counter);
bool	check_dup_methods(std::string token, std::string line, Location *temp_location, size_t counter);

void	ctrlc(int signum);
void	printlog(std::string msg, int arg, std::string color);
void	printerr(std::string msg, int arg, std::string color);
int     end_loop(int end);

// tools

int     removehead(char *file);
bool    compare(const char *find, char* str);
int     removeheadnoimg(char *file, int size);
int     getIntSize(int nbr);
std::string	getINFOone(std::string request, const char *what, int pos);
std::string	getINFOtwo(std::string request, const char *what, int pos);
std::string removeDashIfExists(std::string path);
Methods whatstheMethod(Methods meth, std::string word);
std::string getFirstLine(std::string content);


#endif
