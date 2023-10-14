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
						// std::cout << "request received from client " << (struct sockaddr *)clientinfo.sin_addr.s_addr << std::endl;
						printlog("NEW REQUEST FROM CLIENT", fds[i].fd - 2, YELLOW);

						Request req = parseRecv(fds, i);
						//std::cout << req->content.getContent() << std::endl;
						int cgi_fd = getCorrectServ(&req).filter_request(req);
						if (!(req.Get().empty() && req.Post().empty()))
							parseSend(fds, i, req, cgi_fd);
						
						// delete req;
					}
				}
			}
        }
    }
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

Serv	&Servers::getCorrectServ(Request *req)
{
	std::vector<Serv>::iterator it;
	for (it = servs.begin(); it != servs.end(); ++it)
	{
		if (it->compareHostPort(req->Host(), req->Port()))
		{
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
