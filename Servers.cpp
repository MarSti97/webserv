#include "./includes/webserv.hpp"

template <typename T>
bool	check_duplicate_attr(T attribute, std::string line)
{
	if (!(attribute.empty()))
		throw_parsing_exception(line, 1);
	return true;
}

Servers::Servers(std::string file, char **environment) : env(environment), payloadTooLarge_413(false)
{
	config = readFile(file);
	(void)env;
}
void	Servers::validate_config()
{
	std::string	line;
	std::istringstream	file(config);
	bool		insideServerBlock;
	bool		insideLocationBlock;

	while (std::getline(file, line))
	{
		Config temp_config;
		if (line.empty())
			continue;

		std::istringstream s(line);
		std::string token;
		if (!insideServerBlock)
		{
			if (s >> token && token == "server" && s >> token && token == "{" && s.eof())
				insideServerBlock = true;
			else
				throw_parsing_exception(line, 2);
		}
		while (insideServerBlock)
		{
			std::getline(file, line);
			if (line.empty())
				continue;
			std::istringstream ss(line);
			ss >> token;
			if (token == "listen" && check_duplicate_attr(temp_config.port, line))
				temp_config.port = parse_attribute(ss, token, line);
			else if (token == "host" && check_duplicate_attr(temp_config.host, line))
				temp_config.host = parse_attribute(ss, token, line);
			else if (token == "client_max_body_size" && check_duplicate_attr(temp_config.max_body_size, line))
				temp_config.max_body_size = parse_attribute(ss, token, line);
			else if (token == "server_name" && check_duplicate_attr(temp_config.server_name, line))
				parseServerNames(ss, token, &temp_config, line);
			else if (token == "error_page")
				parseErrorPages(ss, token, &temp_config, line);
			else if (token == "location" && ss >> token && !(ss.eof()))
			{
				std::string location_name = token;
				if (ss >> token && token == "{" && ss.eof())
				{
					Location temp_location;
					insideLocationBlock = true;
					temp_location.path = location_name;
					while (insideLocationBlock)
					{
						std::getline(file, line);
						if (line.empty())
							continue;
						std::istringstream iss(line);
						iss >> token;
						if (token == "root" && check_duplicate_attr(temp_location.root, line))
							temp_location.root = parse_attribute(iss, token, line);
						else if (token == "index" && check_duplicate_attr(temp_location.index, line))
							temp_location.index = parse_attribute(iss, token, line);	
						else if (token == "autoindex" && check_duplicate_attr(temp_location.autoindex, line))
							temp_location.autoindex = parse_attribute(iss, token, line);	
						else if (token == "cgi" && check_duplicate_attr(temp_location.cgi_extension, line))
							temp_location.cgi_extension = parse_attribute(iss, token, line);	
						else if (token == "return" && check_duplicate_attr(temp_location.redirect_path, line))
							temp_location.redirect_path = parse_attribute(iss, token, line);
						else if (token == "allow" && check_duplicate_attr(temp_location.methods, line))
							parseMethods(iss, token, &temp_location, line);
						else if (token == "}" && iss.eof())
						{
							validate_location(temp_location, temp_config.location, line);
							temp_config.location.push_back(temp_location);			
							insideLocationBlock = false;
						}
						else
							throw_parsing_exception(line, 2);
					}
				}
			}
			else if (token == "}" && ss.eof())
			{
				std::cout << "HELLO" << std::endl;
				insideServerBlock = false;		
				check_requirements(&temp_config);
				servs.push_back(Serv(temp_config));
			}
			else
				throw_parsing_exception(line, 2);
		}
	}
	
}

// void	Servers::expectContinueOrChuncked(std::string buf, Serv serv, int clientfd)
// {


// 	std::cout << "CHECK: expect: " << expect << " chunked: " << chunked << std::endl;
// 	if (chunked == "chunked")
// 		chunked_data = true;
// 	if (expect == "100-continue")
// 	{
// 		serv.parseSend("HTTP/1.1 100 Continue\r\nConnection: keep-alive\r\n", clientfd);
// 		continue_100 = true;
// 	}
// }

bool Servers::checkContentSizeToMax(char *buffer, ssize_t n, int clientfd)
{
	std::string buf(buffer);
	std::cerr << buf << std::endl;
	Request req(buffer, n);
	if (req.Post() != "")
	{
		Serv temp = getCorrectServ(req);
		// expectContinueOrChuncked(buf, temp, clientfd);
		std::string max_string = temp.getMaxBodySize();
		std::string len_string = req.Contentlength();
		std::cerr << "ERROR: " << max_string << " | " << len_string << std::endl;
		if (max_string == "" || len_string == "")
		{
			std::cerr << "Error: unable to calculate max body size" << std::endl;
			return true;
		}
		long max;
		long contentlen;

		std::istringstream(max_string) >> max;
		std::istringstream(len_string) >> contentlen;
		if (max < contentlen)
		{
			req.SetClientFd(clientfd);
			temp.errorPageCheck("413", "Payload Too Large", "/413.html", req);
			payloadTooLarge_413 = true;
			return false;
		}
	}
	return true;
}

Request Servers::parseRecv(std::vector<pollfd> &fd, int pos)
{
    char buffer[4097];
    std::vector<std::pair<char *, int> > full_buf;
    std::string findbuffer;
    ssize_t n;
    int counter = 0;
    int buf_size = 0;
	// bool first = true;
    while (1)
    {
        bzero(buffer, sizeof(buffer));
        n = recv(fd[pos].fd, buffer, 4096, 0);
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
			if (counter == 0)
			{
				if (!checkContentSizeToMax(buffer, n, fd[pos].fd))
					return Request();
			}
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
    std::vector<std::pair<char *, int> >().swap(full_buf); // why
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
				if (errno == EINTR)
					std::cerr << "Poll was interupted by a signal" << std::endl;
				else
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
						if (req.getContinue100())
						{
							getCorrectServ(req).parseSend("HTTP/1.1 100 Continue\r\nConnection: keep-alive\r\n", fds[i].fd);
							continue;
						}
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

Serv	&Servers::getCorrectServ(Request req) // this is probably the right place to implement the server_name differentiation
{
	std::vector<Serv>::iterator it;
	for (it = servs.begin(); it != servs.end(); ++it)
	{
		if (it->compareHostPort(req.Host(), req.Port()))
		{
			return *it;
		}
	}
	return *servs.end();
}

Servers::~Servers()
{
	std::vector<pollfd>::iterator it;
	for (it = fds.begin(); it != fds.end(); ++it)
	{
		close(it->fd);
	}
	fds.clear();
}
