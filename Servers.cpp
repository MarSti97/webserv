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
    while (iss >> token) 
	{
		if (token == "server")
		{
			Config temp_config;
			if (iss >> token && token == "{")
			{
				while (iss >> token) 
				{
					if (token == "listen" && check_duplicate_attr(temp_config.port))
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
					else if (token == "autoindex")
						temp_config.autoindex = parse_attribute(iss, token);
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
									if (temp_location.path == "/")
									{
										while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
										{
											std::string res = (token[token.length() - 1] == ';' ? token.substr(0, token.length() - 1) : token);
											temp_config.methods.insert(std::make_pair(res, true));
										}
									}
									while (*(token.end() - 1) != ';' && iss >> token && !check_new_attribute(token))
									{
										std::string res = (token[token.length() - 1] == ';' ? token.substr(0, token.length() - 1) : token);
										temp_location.methods.insert(std::make_pair(res, true));
									}
								}
								else if (token == "root")
								{
									if (temp_location.path == "/")
										temp_config.root = parse_attribute(iss, token);
									else
										temp_location.root = parse_attribute(iss, token);
								}
								else if (token == "index")
								{
									if (temp_location.path == "/")
										temp_config.index = parse_attribute(iss, token);
									else
										temp_location.index = parse_attribute(iss, token);
								}
								else if (token == "autoindex")
								{
									if (temp_location.path == "/")
										temp_config.autoindex = parse_attribute(iss, token);
									else
										temp_location.autoindex = parse_attribute(iss, token);
								}
								else if (token == "cgi")
								{
									if (temp_location.path == "/")
										temp_config.cgi_extension = parse_attribute(iss, token);
									else
										temp_location.cgi_extension = parse_attribute(iss, token);
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
	                    check_requirements(temp_config);
						servs.push_back(Serv(temp_config));
						break ;
					}
				}
			}
		}
    }
}

Request Servers::parseRecv(std::vector<pollfd> &fd, int pos)
{
    char buffer[4097];
    std::vector<std::pair<char *, int> > full_buf;
    std::string findbuffer;
    ssize_t n;
    int counter = 0;
    int buf_size = 0;
    while (1)
    {
        bzero(buffer, sizeof(buffer));
        n = recv(fd[pos].fd, buffer, 4096, 0);
        // std::cout << "THIS: recv " << n << std::endl;
        if (n <= 0)
        {
            if (n == 0)
            {
                if (!counter)
                {
                    // Connection closed by the client
                    printlog("LOST CLIENT", fd[pos].fd - 2, RED);
                    close(fd[pos].fd);
                    fd.erase(fd.begin() + pos);
                    return Request();
                }
                break;
            }
            else if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // No data available for non-blocking receive
                // std::cout << "all recv" << std::endl;
                break;
            }
            else {
                // Handle other receive errors
                std::cerr << "Error: reading from poll" << std::endl;
                perror("read");
                return Request();
            }
        }
        else
        {
            char *bug = new char[n + 1];
            for (int o = 0; o < n; ++o)
                bug[o] = buffer[o];
            full_buf.push_back(std::make_pair(bug, n));
        }
        counter++;
        buf_size += n;
        if (n < 4096)
            break;
    }
    char buf[buf_size + 1];
    std::vector<std::pair<char *, int> >::iterator it;
    int f = 0;
    for (it = full_buf.begin(); it != full_buf.end(); ++it)
    {
        int i = -1;
        while (++i < it->second)
            buf[f++] = it->first[i];
        delete[] it->first;
    }
    buf[buf_size] = '\0';
    std::vector<std::pair<char *, int> >().swap(full_buf);
    findbuffer = std::string(buf, buf_size);
    Request req = postThings(findbuffer, buf, fd[pos].fd, buf_size);
    return req;
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
						if (!(req.Get().empty() && req.Post().empty() && req.Del().empty()))
						{
							req.SetClientFd(fds[i].fd);
							getCorrectServ(req).filterRequest(req);
							// int cgi_fd = getCorrectServ(req).filter_request(req);
						}
						
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
