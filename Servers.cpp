#include "./includes/webserv.hpp"

Servers::Servers(std::string file, char **environment) : env(environment)
{
	config = readFile(file);
	(void)env;
}

void	Servers::validate_config()
{
	std::istringstream iss(config);
	std::string token;
	int i = 0;
    while (iss >> token) 
	{
		if (token == "server")
		{
			Config temp_config;
			if (iss >> token && token == "{")
			{
				while (iss >> token) 
				{
					if (token == "listen")
						temp_config.port = parse_attribute(iss, token);
					else if (token == "host")
						temp_config.host = parse_attribute(iss, token);
					else if (token == "server_name")
					{
						while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
						{
							if (*(token.end() - 1) == ';')
								temp_config.server_name.push_back(token.substr(0, token.size() - 1));
							else
								temp_config.server_name.push_back(token);
						}
					}
					else if (token == "root")
						temp_config.root = parse_attribute(iss, token);
					else if (token == "index")
						temp_config.index = parse_attribute(iss, token);
					else if (token == "client_max_body_size")
						temp_config.max_body_size = parse_attribute(iss, token);
					else if (token == "allow")
					{
						while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
						{
							if (token == "GET" || token == "GET;")
								temp_config.allow_get = true;
							else if (token == "POST" || token == "POST;")
								temp_config.allow_post = true;
							else if (token == "DELETE" || token == "DELETE;")
								temp_config.allow_delete = true;
						}
					}
					else if (token == "autoindex")
						temp_config.autoindex = parse_attribute(iss, token);
					else if (token == "cgi")
					{
						iss >> token;
						if (*(token.end() - 1) != ';' && !check_new_attribute(token))
							temp_config.cgi_extension = token;
						if (iss >> token && *(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.cgi_directory = token.substr(0, token.size() - 1);						
					}
					else if (token == "error_page")
					{
						iss >> token;
						if (*(token.end() - 1) != ';' && !check_new_attribute(token))
							temp_config.error_pages[token] = "";
						std::string oldtoken = token;
						if (iss >> token && *(token.end() - 1) == ';' && !check_new_attribute(token))
							temp_config.error_pages[oldtoken] = token.substr(0, token.size() - 1);	
					}
					else if (token == "location")
					{
						Location temp_location;
						iss >> token;
						temp_location.path = token;
						if (!check_new_attribute(token) && iss >> token && token == "{")
						{
							while (iss >> token)
							{
								if (token == "allow")
								{
									while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
									{
										if (token == "GET" || token == "GET;")
											temp_location.allow_get = true;
										else if (token == "POST" || token == "POST;")
											temp_location.allow_post = true;
										else if (token == "DELETE" || token == "DELETE;")
											temp_location.allow_delete = true;
									}
								}
								else if (token == "root")
									temp_location.root = parse_attribute(iss, token);
								else if (token == "index")
									temp_location.index = parse_attribute(iss, token);
								else if (token == "autoindex")
									temp_location.autoindex = parse_attribute(iss, token);
								else if (token == "error_page")
								{
									iss >> token;
									if (*(token.end() - 1) != ';' && !check_new_attribute(token))
										temp_location.error_pages[token] = "";
									std::string oldtoken = token;
									if (iss >> token && *(token.end() - 1) == ';' && !check_new_attribute(token))
										temp_location.error_pages[oldtoken] = token.substr(0, token.size() - 1);	
								}
								else if (token == "return")
								{
									iss >> token;
									if (*(token.end() - 1) != ';' && !check_new_attribute(token))
										temp_location.redirect_status = token;
									if (iss >> token && *(token.end() - 1) == ';' && !check_new_attribute(token))
										temp_location.redirect_path = token.substr(0, token.size() - 1);						
								}
								else if (token == "}")
								{
									temp_config.location.push_back(temp_location);
									break ;
								}
							}
						}
					}
					else if (token == "}")
					{
	                    check_requirements(temp_config, ++i);
						servs.push_back(Serv(temp_config));
						break ;
					}
				}
			}
		}
    }
}

void	Servers::printAll() const
{
	for (size_t i = 0; i < servs.size(); i++)
		servs[i].print(i);
}

void Servers::init()
{
    validate_config();
    std::vector<Serv>::iterator it;
	int i = 0;
    for (it = servs.begin(); it != servs.end(); ++it)
	{
        it->establish_connection(); // FIX:: should program exit if one fails or continue with non failed ones
		fds.push_back(pollfd());
		fds[i].fd = it->getSocket();
		fds[i].events = POLLIN;
		i++;
	}
}

void Servers::run()
{
	struct sockaddr_in clientinfo;
    socklen_t size = sizeof(clientinfo);
	int timeout = 0;

	while (!end_loop(0))
    {
        // bzero(buffer, sizeof(buffer));
        // std::cout << fds.size() << std::endl;
        for (size_t i = 0; i < fds.size(); ++i)
        {
			timeout = 0;
			if (i >= servs.size())
				timeout = 10000;
			int ret = poll(&fds[i], 1, timeout);
			if (ret == -1)
			{
				std::cerr << "Error in poll" << std::endl;
				continue;
			}
			if (ret == 0 && !checkSockets(fds[i].fd))
			{
				printlog("TIMEOUT CLIENT", fds[i].fd - 2, RED);
				// close(fds[i].fd);
        		// fds.erase(fds.begin() + i);
			}
			else
			{
				if (fds[i].revents & POLLIN)
				{
					if (int socketfd = checkSockets(fds[i].fd))
					{
						if (acceptConnection(socketfd, &clientinfo, size, &fds))
							fds[i].events &= ~POLLIN;
						continue;
					}
					else if (fds[i].revents & POLLOUT)
					{
						printlog("NEW REQUEST FROM CLIENT", fds[i].fd - 2, YELLOW);

						Request req = parseRecv(fds, i);
						if (!(req.Get().empty() && req.Post().empty()))
						{
							req.SetClientFd(fds[i].fd);
							getCorrectServ(req).filterRequest(req);
							// int cgi_fd = getCorrectServ(req).filter_request(req);
						}
						
						// delete req;
					}
				}
			}
        }
    }
}

void	Serv::filterRequest( Request req )
{
	if (!(req.Get().empty()))
		PrepareResponse("GET", req.Get(), req);
	else if (!(req.Post().empty()))
		PrepareResponse("POST", req.Post(), req);
	else if (!(req.Del().empty()))
		PrepareResponse("DELETE", req.Del(), req);
	else
	{
		if (serv_info.error_pages["405"].empty())
			parseSend(getResponse(serv_info.root, "/405.html", getHeader("405 Method Not Allowed", "", "/405.html")), req.ClientFd()); // need to do the 405 page.
		else
			parseSend(getResponse(serv_info.root, serv_info.error_pages["405"], getHeader("405 Method Not Allowed", "", serv_info.error_pages["405"])), req.ClientFd());
	}
}

std::string	Serv::findFolder( std::string folder, int check ) //parse the path and if called with check it will return the previous folder.
{
	size_t f = folder.rfind('/');
	if ((f == folder.length() - 1) && !check)
		return folder;
	if (f == (folder.length() - 1) && check && (folder != "/"))
		return folder.substr(0, folder.rfind('/', f - 1));
	if (f != std::string::npos)
	{
		size_t i = folder.rfind('.', f);
		if (i != std::string::npos)
			return folder.substr(0, f);
		else
			return folder + "/";
	}
	else
		return "";
}

std::string makePathGreatAgain(std::string path, std::string root)
{
	size_t fi = path.rfind('/');
    if (fi != std::string::npos)
        return root.substr(1) + path.substr(fi);
    else
        return root.substr(1) + path;
}

void	Serv::PrepareResponse( std::string method, std::string path, Request req )
{
	if (CheckAllowed(method, path, serv_info.location))
	{
		std::string ass = makePathGreatAgain(path, CheckRoot(path, serv_info.location));
		if (findFolder(path, 0).length() < path.length()) // it is a file
		{
			if (method == "GET" || method == "POST")
			{
				if (!access(ass.c_str(), R_OK)) // file exists
				{
					std::cout << "fuck" << std::endl;
					if (ext_CGI(path) && folder_CGI(path)) // it is a CGI script
						parseSend(sendby_CGI(cgi_request(req, path, serv_info.cgi_extension)), req.ClientFd());
					else // "normal" request
						parseSend(getResponse(CheckRoot(path, serv_info.location), path, getHeader("200 OK", "", path)), req.ClientFd());
				}
				else // if does not exists, error 404
				{

					if (serv_info.error_pages["404"].empty())
						parseSend(getResponse(serv_info.root, "/404.html", getHeader("404 Not Found", "", path)), req.ClientFd());
					else
						parseSend(getResponse(serv_info.root, serv_info.error_pages["404"], getHeader("404 Not Found", "", path)), req.ClientFd());
				}
			}
			// else
				// delet;
		}
		else // it is a folder
		{
			if (!access(ass.c_str(), F_OK)) // file exists
			{
				std::string index = CheckIndex(path, serv_info.location);
				std::cout << "ass " << path << std::endl;
				if (!(index.empty()))
					parseSend(getResponse(CheckRoot(path, serv_info.location), index, getHeader("200 OK", "", index)), req.ClientFd());
				else
				{
					if (CheckAutoindex(path, serv_info.location))
					{
						if (!access(ass.c_str(), R_OK)) 
							parseSend(makeDirectoryList(path), req.ClientFd());
						else // if does not exists, error 404
						{
							if (serv_info.error_pages["403"].empty())
								parseSend(getResponse(serv_info.root, "/403.html", getHeader("403 Forbidden", "", path)), req.ClientFd());
							else
								parseSend(getResponse(serv_info.root, serv_info.error_pages["403"], getHeader("403 Forbidden", "", path)), req.ClientFd());
						}
					}
					else // if does not exists, error 404
					{
						if (serv_info.error_pages["404"].empty())
							parseSend(getResponse(serv_info.root, "/404.html", getHeader("404 Not Found", "", path)), req.ClientFd());
						else
							parseSend(getResponse(serv_info.root, serv_info.error_pages["404"], getHeader("404 Not Found", "", path)), req.ClientFd());
					}		
				}
			}
			else // if does not exists, error 404
			{
				if (serv_info.error_pages["404"].empty())
					parseSend(getResponse(serv_info.root, "/404.html", getHeader("404 Not Found", "", path)), req.ClientFd());
				else
					parseSend(getResponse(serv_info.root, serv_info.error_pages["404"], getHeader("404 Not Found", "", path)), req.ClientFd());
			}
		}
	}
	else
	{
		if (serv_info.error_pages["405"].empty())
			parseSend(getResponse(serv_info.root, "/405.html", getHeader("405 Method Not Allowed", "", "/405.html")), req.ClientFd()); // need to do the 405 page.
		else
			parseSend(getResponse(serv_info.root, serv_info.error_pages["405"], getHeader("405 Method Not Allowed", "", serv_info.error_pages["405"])), req.ClientFd());
	}

}

bool	Serv::CheckAutoindex( std::string path, std::vector<Location> Locations)
{
	std::vector<Location>::iterator it;
	int check = 0;

    for (it = Locations.begin(); it != Locations.end(); ++it)
    {
		if (it->path == findFolder(path, check))
		{
			if (!(it->autoindex.empty()))
				return true;
			else
				return 0;
		}
    }
    return "";
}

std::string	Serv::CheckRoot( std::string path, std::vector<Location> Locations)
{
	std::vector<Location>::iterator it;
	int check = 0;

    for (it = Locations.begin(); it != Locations.end(); ++it)
    {
		if (it->path == findFolder(path, check) && std::cout << path << " ko " << findFolder(path, check) << std::endl)
		{
			std::cout << serv_info.root << std::endl;
			if (!(it->root.empty()))
				return it->root;
			else
				return serv_info.root;
		}
    }
    return "";
}

std::string	Serv::CheckIndex( std::string path, std::vector<Location> Locations)
{
	std::vector<Location>::iterator it;
	int check = 0;

    for (it = Locations.begin(); it != Locations.end(); ++it)
    {
		if (it->path == findFolder(path, check))
		{
			if (!(it->index.empty()))
				return it->index;
		}
    }
    return "";
}

std::string	Serv::sendby_CGI(int cgi_fd)
{
	char    buffer[4096];
	std::string response;
	std::string responseHeaders;
	ssize_t bytesRead;

	if (cgi_fd > 2)
	{	
		while ((bytesRead = read(cgi_fd, buffer, sizeof(buffer))) > 0)
		{
				response.append(buffer, bytesRead);
			}

			if (bytesRead < 0) {
				perror("Error reading from file descriptor");
				// Handle the error as needed
			}
			responseHeaders = "HTTP/1.1 200 OK\r\n";
			responseHeaders += "Content-Type: text/html\r\n";
			responseHeaders += "Connection: keep-alive\r\n";
			std::stringstream ss;
			ss << response.length();
			responseHeaders += "Content-Length: " + ss.str() + "\r\n\r\n";
			//std::cout << responseHeaders + response << std::endl;
			close(cgi_fd);
	}
	return responseHeaders + response;
}

bool	Serv::ext_CGI(std::string path_info)
{
	std::string extension_string;

	size_t	extension_start = path_info.rfind('.');
	if (extension_start != std::string::npos)
	{
		size_t query_start = path_info.rfind('?');
		if (query_start != std::string::npos)
			extension_string = path_info.substr(extension_start, query_start - extension_start);
		else
			extension_string = path_info.substr(extension_start);
		if (extension_string == serv_info.cgi_extension)
			return 1;
	}
	return 0;
}

bool	Serv::folder_CGI(std::string path)
{
	size_t k = path.find(serv_info.cgi_directory, 0);
	if (k == serv_info.cgi_directory.length())
		return 1;
	else
		return 0;
}


bool	Serv::CheckAllowed( std::string method, std::string path, std::vector<Location> Locations)
{
	std::vector<Location>::iterator it;
	int check = 0;

    for (it = Locations.begin(); it != Locations.end(); ++it)
    {
		std::string newpath = path;
        while (!((newpath = findFolder(newpath, check)).empty()))
        {
			if (it->path == findFolder(newpath, check))
			{
				if (method == "GET")
				{
					if (it->allow_get)
						return it->allow_get;
				}
				else if (method == "POST")
				{
					if (it->allow_post)
						return it->allow_post;
				}
				else if (method == "DELETE")
				{
					if (it->allow_delete)
						return it->allow_delete;
				}
			}
			check++;
		}
		check = 0;
    }
    return false;
}

int	Servers::checkSockets(int fd)
{
	std::vector<Serv>::iterator it;
	for (it = servs.begin(); it != servs.end(); ++it)
	{
		// std::cout << "COMP:: " << it->getSocket() << " " << fd << std::endl; 
		if (it->getSocket() == fd)
			return fd;
	}
	return 0;
}

Serv	&Servers::getCorrectServ(Request req)
{
	std::vector<Serv>::iterator it;
	for (it = servs.begin(); it != servs.end(); ++it)
	{
		// std::cout << "ENTER: " << req.Host() << " " << req.Port() << std::endl;
		if (it->compareHostPort(req.Host(), req.Port()))
		{
			// it->printshit();
			return *it;
		}
	}
	return *servs.end();
}

int Serv::establish_connection()
{
    struct addrinfo *addr;
    if (getaddrinfo(serv_info.server_name[0].c_str(), serv_info.port.c_str(), NULL, &addr) < 0){ // port 80 to not write everytime the port with the address
        std::cerr << "Error: couldn't get address" << std::endl;
        return 1;
    }
    socketfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (socketfd < 0) {
        return failToStart("Error: socket creation", addr, socketfd);
	}
	int opt = 1;
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) // ignores wait time for rebinding
		return failToStart("Error: socket optimise", addr, socketfd);
	//glob_fd = socketfd;
	signal(SIGINT, ctrlc);
    int flags = fcntl(socketfd, F_GETFL, 0); // set the socket to non-blocking;
    if (flags == -1)
        return failToStart("Error getting socket flags", addr, socketfd);
    if (fcntl(socketfd, F_SETFL, flags | O_NONBLOCK) == -1)
        return failToStart("Error setting socket to non-blocking", addr, socketfd);
    if (bind(socketfd, addr->ai_addr, addr->ai_addrlen) == -1)
        return failToStart("Error: bind unsuccesful", addr, socketfd);
    if (listen(socketfd, 2) == -1) {
        return failToStart("Error: listen unsuccesful", addr, socketfd);
	}
    freeaddrinfo(addr);
	return 0; //fix returns and also figure out if exceptions are better
}

void	Serv::print(int counter) const
{
	std::cout << std::endl;
	std::cout << "Server Configuration " << counter << std::endl;
	std::cout << "host: " << serv_info.host << std::endl;	
	std::cout << "port: " << serv_info.port << std::endl;
	std::cout << "root: " << serv_info.root << std::endl;
	std::cout << "index: " << serv_info.index << std::endl;
	std::cout << "server_name: ";
	for (size_t j = 0; j < serv_info.server_name.size(); j++)
	{
		std::cout << serv_info.server_name[j] << " ";
	}
	std::cout << std::endl;
	std::cout << "max_body_size: " << serv_info.max_body_size << std::endl;
	std::cout << (serv_info.allow_get ? "GET: allowed" : "GET: denied") << std::endl;
	std::cout << (serv_info.allow_post ? "POST: allowed" : "POST: denied") << std::endl;
	std::cout << (serv_info.allow_delete ? "DELETE: allowed" : "DELETE: denied") << std::endl;
	std::cout << "autoindex: " << serv_info.autoindex << std::endl;
	std::cout << "cgi_extension: " << serv_info.cgi_extension << std::endl;
	std::cout << "cgi_directory: " << serv_info.cgi_directory << std::endl;
	std::map<std::string, std::string>::const_iterator it2;
	for (it2 = serv_info.error_pages.begin(); it2 != serv_info.error_pages.end(); it2++)
	{
		std::cout << "error_page " << it2->first << ": " << it2->second << std::endl;
	}
	std::vector<Location>::const_iterator it;
	for (it = serv_info.location.begin(); it != serv_info.location.end(); it++)
	{
		std::cout << std::endl;
		std::cout << "Location " << it->path << std::endl;
		std::cout << "root: " << it->root << std::endl;
		std::cout << "index: " << it->index << std::endl;
		std::cout << (it->allow_get ? "GET: allowed" : "GET: denied") << std::endl;
		std::cout << (it->allow_post ? "POST: allowed" : "POST: denied") << std::endl;
		std::cout << (it->allow_delete ? "DELETE: allowed" : "DELETE: denied") << std::endl;
		std::cout << "redirect_status: " << it->redirect_status << std::endl;
		std::cout << "redirect_path: " << it->redirect_path << std::endl;			
		if (!(it->error_pages.empty()))
		{
			std::map<std::string, std::string>::const_iterator it3;
			for (it3 = it->error_pages.begin(); it3 != it->error_pages.end(); it3++)
			{
				std::cout << "error_page " << it3->first << ": " << it3->second << std::endl;
			}
		}
	}
}

bool Serv::compareHostPort(std::string host, std::string port)
{
	if (serv_info.host == host && serv_info.port == port)
		return true;
	return false;
}


int Serv::getSocket()
{
	return socketfd;
}
