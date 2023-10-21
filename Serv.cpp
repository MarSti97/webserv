#include "./includes/webserv.hpp"


int Serv::parseSend(std::string response, int fd)
{
    if (!response.empty())
    {
        ssize_t n = send(fd, response.c_str(), response.size(), 0);
        if (n < 0)
        {
            std::cerr << "Error writing to socket" << std::endl;
            return 1;
        }
        if ((unsigned long)n != response.size())
        {
            std::cout << "bad response sent" << std::endl;
            return 1;
        }
        return n;
    }
    else
        std::cout << "post chuncked" << std::endl;
    return 0;
}

std::string Serv::getResponse(std::string abs, std::string page, std::string responseHeaders)
{
    std::cout << "DEBUG: getResponse " << (abs.substr(1) + page) << std::endl;
    std::string response = readFile(abs.substr(1) + page); // maybe need the substr(1)
    std::stringstream ss;
    ss << response.length();
    responseHeaders += "Content-Length: " + ss.str() + "\r\n\r\n";
    if (response == "" || response.empty())
        response = readFile(serv_info.root + "/404.html");
    return responseHeaders + response;

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
		if (serv_info.error_pages["501"].empty())
			parseSend(getResponse(serv_info.root, "/501.html", getHeader("501 Not Implemented", "", "/501.html")), req.ClientFd()); // need to do the 405 page.
		else
			parseSend(getResponse(serv_info.root, serv_info.error_pages["501"], getHeader("501 Not Implemented", "", serv_info.error_pages["501"])), req.ClientFd());
	}
}

std::string	Serv::findFolder( std::string path ) //parse the path and if called with check it will return the previous folder.
{
    size_t i = path.rfind('/');
	if (path.substr(i).find('.') != std::string::npos)
		return path;
	else
		return "";
}

std::string Serv::createAbsolutePath(std::string path)
{
	if (path == "/")
		return serv_info.root;
	std::vector<Location>::iterator it;
	std::string page;
	size_t i = path.rfind('/');
	if (path.substr(i).find('.') != std::string::npos)
		page = path.substr(i);
	else
		page = "";
	std::string newPath = path.substr(0, i);
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		std::cout << "DEBUG: " << newPath << " " << it->path << std::endl;
		if (!(it->root.empty()) && newPath == it->path)
		{
			std::cout << "DEBUG: createABsPath" << it->root << std::endl;
			return it->root + page;
		}
    }
	return serv_info.root + path;
}


std::string	Serv::CheckIndex( std::string path)
{
	if (path == "/")
		return serv_info.index;
	std::vector<Location>::iterator it;
	size_t i = path.rfind('/');
	std::string newPath = path.substr(0, i);
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (it->path == newPath && !(it->index.empty()))
			return it->index;
    }
    return "";
}

std::string	Serv::CheckCGI( std::string path)
{
	std::vector<Location>::iterator it;
	size_t i = path.rfind('/');
	size_t j = path.rfind('.');
	std::string extension = path.substr(j);
	std::string newPath = path.substr(0, i);
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		std::cout << "DEBUG cgi: " << extension << " | " << newPath << " & " << it->cgi_extension << std::endl;
		if (it->path == newPath && it->cgi_extension == extension)
			return extension;
    }
    return "";
}

void	Serv::PrepareResponse( std::string method, std::string path, Request req )
{
	if (CheckAllowed(method, path))
	{
		std::string abs = createAbsolutePath(path);
		std::cout << "DEBUG: absulote: " << abs << std::endl;
		if (findFolder(abs) != "") // it is a file
		{
			std::cout << "DEBUG: isAFile: " << std::endl;
			if (method == "GET" || method == "POST")
			{
				if (!access(abs.c_str() + 1, R_OK)) // file exists
				{
					std::string theExtension = CheckCGI(path);
					if (theExtension != "") // it is a CGI script
					{
						std::cout << "fuck" << std::endl;
						parseSend(sendby_CGI(cgi_request(req, abs, theExtension)), req.ClientFd());
					}
					else // "normal" request
					{
						// std::cout << path << " " << CheckRoot(path, serv_info.location) << std::endl;
						parseSend(getResponse(abs, "", getHeader("200 OK", "", abs)), req.ClientFd());
					}
				}
				else // if does not exists, error 404
				{

					if (serv_info.error_pages["404"].empty())
						parseSend(getResponse(serv_info.root, "/404.html", getHeader("404 Not Found", "", abs)), req.ClientFd());
					else
						parseSend(getResponse(serv_info.root, serv_info.error_pages["404"], getHeader("404 Not Found", "", abs)), req.ClientFd());
				}
			}
		}
		else // it is a folder
		{
			std::cout << "DEBUG: isAFolder: " << std::endl;
			if (access(abs.c_str() + 1, F_OK) != -1) // folder exists
			{
				std::string index = CheckIndex(path);
				std::cout << "abs " << path << std::endl;
				if (!(index.empty()))
					parseSend(getResponse(abs, index, getHeader("200 OK", "", index)), req.ClientFd());
				else
				{
					if (CheckAutoindex(path))
					{
						if (!access(abs.c_str() + 1, R_OK)) 
							parseSend(makeDirectoryList(abs, path), req.ClientFd()); // BUGGED
						else // if does not exists, error 404
						{
							if (serv_info.error_pages["403"].empty())
								parseSend(getResponse(serv_info.root, "/403.html", getHeader("403 Forbidden", "", abs)), req.ClientFd());
							else
								parseSend(getResponse(serv_info.root, serv_info.error_pages["403"], getHeader("403 Forbidden", "", abs)), req.ClientFd());
						}
					}
					else // if does not exists, error 404
					{
						std::cout << "DEBUG: noIndex: " << std::endl;
						if (serv_info.error_pages["404"].empty())
							parseSend(getResponse(serv_info.root, "/404.html", getHeader("404 Not Found", "", abs)), req.ClientFd());
						else
							parseSend(getResponse(serv_info.root, serv_info.error_pages["404"], getHeader("404 Not Found", "", abs)), req.ClientFd());
					}		
				}
			}
			else // if does not exists, error 404
			{
				std::cout << "DEBUG: noAccess: " << std::endl;
				if (serv_info.error_pages["404"].empty())
					parseSend(getResponse(serv_info.root, "/404.html", getHeader("404 Not Found", "", abs)), req.ClientFd());
				else
					parseSend(getResponse(serv_info.root, serv_info.error_pages["404"], getHeader("404 Not Found", "", abs)), req.ClientFd());
			}
		}
	}
	else
	{
		// std::cout << "DEGUB: Prepare Response" << std::endl;
		if (serv_info.error_pages["405"].empty())
			parseSend(getResponse(serv_info.root, "/405.html", getHeader("405 Method Not Allowed", "", "/405.html")), req.ClientFd()); // need to do the 405 page.
		else
			parseSend(getResponse(serv_info.root, serv_info.error_pages["405"], getHeader("405 Method Not Allowed", "", serv_info.error_pages["405"])), req.ClientFd());
	}

}

bool	Serv::CheckAutoindex( std::string path)
{
	if (path == "/" && serv_info.autoindex != "")
		return true;
	std::vector<Location>::iterator it;
	size_t i = path.rfind('/');
	std::string newPath = path.substr(0, i - 1);
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (it->path == newPath && !(it->autoindex.empty()))
			return true;
    }
    return false;
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

bool	Serv::CheckAllowed( std::string method, std::string path)
{
	if (serv_info.methods[method])
		return serv_info.methods[method];
	std::string newPath = "";
	int len = 1;
	while (newPath != path)
	{
		size_t i = path.find("/", len);
		if (i == std::string::npos)
			newPath = path;
		else
			newPath = path.substr(0, i - 1);
		std::vector<Location>::iterator it;
		for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
		{
			if (it->path == newPath && it->methods[method])
				return it->methods[method];
		}
		len += i;
	}
    return false;
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
	std::map<std::string, bool>::const_iterator its;
	for (its = serv_info.methods.begin(); its != serv_info.methods.end(); its++)
	{
		std::cout << "Method: " << its->first << " : " << (its->second ? "allowed" : "denied") << std::endl;
	}
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
		std::cout << "cgi-extension: " << it->cgi_extension << std::endl;
		std::map<std::string, bool>::const_iterator it3;
		for (it3 = it->methods.begin(); it3 != it->methods.end(); it3++)
		{
			std::cout << "Method: " << it3->first << " : " << (it3->second ? "allowed" : "denied") << std::endl;
		}
		std::cout << "redirect_status: " << it->redirect_status << std::endl;
		std::cout << "redirect_path: " << it->redirect_path << std::endl;			
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