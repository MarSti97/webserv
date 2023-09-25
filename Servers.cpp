#include "./includes/webserv.hpp"

Servers::Servers(std::string file)
{
	config = readFile(file);
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
    for (it = servs.begin(); it != servs.end(); ++it)
        it->establish_connection(); // FIX:: should program exit if one fails or continue with non failed ones
}

int Servers::getSockets() // FIX:: to get all the sockets for pollfds n shit
{
	return servs[0].getSocket();
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

int Serv::getSocket()
{
	return socketfd;
}
