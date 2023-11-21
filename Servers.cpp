#include "./includes/webserv.hpp"

template <typename T>
bool	check_duplicate_attr(T attribute, std::string line, size_t counter)
{
	if (!(attribute.empty()))
		throw_parsing_exception(line, 1, counter);
	return true;
}

Servers::Servers(std::string file, char **environment) : env(environment)
{
	config = readFile(file);
	(void)env;
}

void	Servers::validate_config()
{
	std::string			line;
	std::istringstream	file(config);
	bool				insideServerBlock = false;
	bool				insideLocationBlock = false;
	size_t				counter = 0;

	while (std::getline(file, line))
	{
		counter++;
		Config temp_config;
		

		std::istringstream s(line);
		std::string token;
		if (s.str().find_first_not_of(" \t") == std::string::npos)
			continue;
		if (!insideServerBlock)
		{
			if (s >> token && token == "server" && s >> token && token == "{" && s.eof())
				insideServerBlock = true;
			else
				throw_parsing_exception(line, 2, counter);
		}
		while (insideServerBlock)
		{
			std::getline(file, line);
			counter++;
			std::istringstream ss(line);
			ss >> token;
			if (ss.str().find_first_not_of(" \t") == std::string::npos)
				continue;
			if (token == "listen" && check_duplicate_attr(temp_config.port, line, counter))
				temp_config.port = parse_attribute(ss, token, line, counter);
			else if (token == "host" && check_duplicate_attr(temp_config.host, line, counter))
				temp_config.host = parse_attribute(ss, token, line, counter);
			else if (token == "client_max_body_size" && check_duplicate_attr(temp_config.max_body_size, line, counter))
				temp_config.max_body_size = parse_attribute(ss, token, line, counter);
			else if (token == "server_name" && check_duplicate_attr(temp_config.server_name, line, counter))
				parseServerNames(ss, token, &temp_config, line, counter);
			else if (token == "error_page")
				parseErrorPages(ss, token, &temp_config, line, counter);
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
						counter++;
						std::istringstream iss(line);
						iss >> token;
						if (iss.str().find_first_not_of(" \t") == std::string::npos)
							continue;
						else if (token == "root" && check_duplicate_attr(temp_location.root, line, counter))
							temp_location.root = parse_attribute(iss, token, line, counter);
						else if (token == "index" && check_duplicate_attr(temp_location.index, line, counter))
							temp_location.index = parse_attribute(iss, token, line, counter);	
						else if (token == "autoindex" && check_duplicate_attr(temp_location.autoindex, line, counter))
							temp_location.autoindex = parse_attribute(iss, token, line, counter);	
						else if (token == "cgi" && check_duplicate_attr(temp_location.cgi_extension, line, counter))
							temp_location.cgi_extension = parse_attribute(iss, token, line, counter);	
						else if (token == "return" && check_duplicate_attr(temp_location.redirect_path, line, counter))
							temp_location.redirect_path = parse_attribute(iss, token, line, counter);
						else if (token == "allow" && check_dup_methods(token, line, &temp_location, counter))
							parseMethods(iss, token, &temp_location, line, counter);
						else if (token == "deny" && check_dup_methods(token, line, &temp_location, counter))
							parseMethods(iss, token, &temp_location, line, counter);						
						else if (token == "}" && iss.eof())
						{
							validate_location(temp_location, temp_config.location, line, counter);
							temp_config.location.push_back(temp_location);			
							insideLocationBlock = false;
						}
						else
							throw_parsing_exception(line, 2, counter);
					}
				}
			}
			else if (token == "}" && ss.eof())
			{
				insideServerBlock = false;		
				check_requirements(&temp_config);
				servs.push_back(Serv(temp_config));
				// std::cout << "NUMBER OF LINES " << counter << std::endl;
			}
			else
				throw_parsing_exception(line, 2, counter);
		}
	}
	
}

bool Servers::checkContentSizeToMax(Request req, Serv serv)
{
	std::string max_string = serv.getMaxBodySize();
	std::string len_string = req.Contentlength();
	if (max_string == "" || len_string == "")
	{
		// printerr("Error: unable to calculate max body size", 0, RED);
		return true;
	}
	long max;
	long contentlen;

	std::istringstream(max_string) >> max;
	std::istringstream(len_string) >> contentlen;
	if (max < contentlen)
	{
		serv.errorPageCheck("413", "Payload Too Large", "/413.html", req);
		return false;
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
    while (1)
    {
        bzero(buffer, sizeof(buffer));
        n = recv(fd[pos].fd, buffer, 4096, 0);
        if (n <= 0)
        {
            if (n <= 0)
            {
                if (!counter) { // Connection closed by the client
                    handleLostClient(fd, pos);
					if (n == -1)
						break;
                    return Request();
                }
                break;
            }
			// Handle other receive errors
			printerr("Error: reading from poll", 0, RED);
			perror("read");
			return Request();
        }
        else
        {
			if (counter == 0)
			{
				Request tempReq(buffer, n);
				//printlog("REQUEST: " + getFirstLine(tempReq.request()) + " FROM CLIENT", fds[pos].fd - 2, CYAN);
				if (tempReq.Post() != "")
					if (!getCorrectServ(tempReq, fd[pos].fd, MAXCHECK))
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
	printlog("Initializing server", -1, YELLOW);
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
		printlog("Server: " + it->getServerHostPort() + " connection established", -1, YELLOW);
	}
}



void Servers::run()
{
	printlog("Running webserv... ", -1, PURPLE);
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
					printerr("Poll was interupted by a signal", 0, RED);
				else
					printerr("Error in poll", 0, RED);
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
					int socketfd = checkSockets(fds[i].fd);
					if (socketfd)
					{
						if (acceptConnection(socketfd, &clientinfo, size, &fds))
							fds[i].events &= ~POLLIN;
						continue;
					}
					else if (fds[i].revents & POLLOUT)
					{
						printlog("NEW REQUEST FROM CLIENT", fds[i].fd - 2, YELLOW);

						Request req = parseRecv(fds, i);
						if (!(req.request().empty()))
							getCorrectServ(req, fds[i].fd, DEFAULT);
					}
				}
			}
        }
    }
}

bool	Servers::getCorrectServ(Request req, int clientfd, ServSelect option) // this is probably the right place to implement the server_name differentiation
{
	std::vector<Serv>::iterator it;
	for (it = servs.begin(); it != servs.end(); ++it)
	{
		if (it->getSocket() == ClientServer(clientfd, 0, CLIENTSOCKET))
		{
			if (it->compareHostPort(req.Host(), req.Port()) || it->compareServerName(req.Host()))
			{
				req.SetClientFd(clientfd);
				if (option == MAXCHECK)
				{
					if (!checkContentSizeToMax(req, *it))
						return false;
				}
				else if (req.getContinue100())
					it->parseSend("HTTP/1.1 100 Continue\r\nConnection: keep-alive\r\n", clientfd);
				else if (headcheck(req.request()))
				{
					if (it->getSocket() != 0)
						it->filterRequest(req);
					else
						it->errorPageCheck("400", "Bad Request", "/400.html", req);
				}
				break ;
			}
		}
	}
	return true;
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

Servers::~Servers()
{
	std::vector<pollfd>::iterator it;
	for (it = fds.begin(); it != fds.end(); ++it)
	{
		close(it->fd);
	}
	fds.clear();
}
