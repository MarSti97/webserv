#include "./includes/webserv.hpp"


int Serv::parseSend(std::string response, int fd)
{
    if (!response.empty())
    {
		size_t res = response.find("\r\n");
		if (res != std::string::npos)
		{
			printlog("RESPONSE: " + response.substr(0, res) + " TO CLIENT", fd - 2, GREEN); // http = 9;
		}
        ssize_t n = send(fd, response.c_str(), response.size(), 0);
        if (n < 0)
        {
            printerr("Error writing to socket", 0, RED);
            return 1;
        }
        if ((unsigned long)n != response.size())
        {
            printerr("bad response sent", 0, YELLOW);
            return 1;
        }
        return n;
    }
    return 0;
}

std::string Serv::getResponse(std::string abs, std::string page, std::string responseHeaders)
{
	if (*(page.begin()) != '/' && !(page.empty()) && *(abs.end() - 1) != '/')
		abs = abs + "/";
	// std::cout << abs + page << std::endl;
    std::string response = readFile(abs + page);
	// std::cout << responseHeaders + response << std::endl;
    std::stringstream ss;
    ss << response.length();
    responseHeaders += "Content-Length: " + ss.str() + "\r\n\r\n";
    if (response == "" || response.empty()){
        response = readFile(serv_info.root + "/404.html");
	}
    return responseHeaders + response;

}

void	Serv::filterRequest( Request req )
{
	// std::cout << req.request() << std::endl;
	if (!(req.Get().empty()))
		PrepareResponse("GET", req.Get(), req);
	else if (!(req.Post().empty()))
		PrepareResponse("POST", req.Post(), req);
	else if (!(req.Del().empty()))
		PrepareResponse("DELETE", req.Del(), req);
	else
	{
		printlog("NOT RECOGNIZED METHOD.", 0, YELLOW);
		errorPageCheck("501", "Not Implemented", "/501.html", req);
	}
}

std::string	Serv::findFolder( std::string path ) //parse the path and if called with check it will return the previous folder.
{
    size_t i = path.rfind('/');
	if (path.substr(i).rfind('.') != std::string::npos)
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
	std::string newPath;
	// std::cout << path << std::endl;
	size_t i = path.rfind('/');
	if (path.substr(i).find('.') != std::string::npos)
	{
		page = path.substr(i);
		newPath = path.substr(0, i);
	}
	else
	{
		page = "";
		newPath = removeDashIfExists(path);
	}
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (!(it->root.empty()) && newPath == it->path)
		{
			// std::cout << it->root + page << std::endl;
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
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (it->path == path && !(it->index.empty()))
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
		if (it->path == newPath && it->cgi_extension == extension)
			return extension;
    }
    return "";
}

void Serv::errorPageCheck(std::string code, std::string message, std::string defaultError, Request req)
{
	std::string page = "/" + code + ".html";
	std::string error = code + " " + message;
	if (serv_info.error_pages[code].empty())
		parseSend(getResponse("DefaultError", page, getHeader(error, "", defaultError)), req.ClientFd());
	else
		parseSend(getResponse(serv_info.root.substr(1), serv_info.error_pages[code], getHeader(error, "", serv_info.error_pages[code])), req.ClientFd());
}

void Serv::deleteMethod(std::string abs, Request req) // need to test this!
{
	// std::cout << "File to be deleted: " << abs << std::endl;
	if (std::remove(abs.c_str()) == 0) // changed to + 1 because it wasn't getting deleted
	{
		printlog("Succefully deleted file", -1, GREEN); // the 0 for the arguemnt is shit need to fix
		parseSend(getResponse(abs, "", getHeader("204 No Content", "", abs)), req.ClientFd());
	}
	else
	{
		printerr("Failed to delete file", -1, RED);
		errorPageCheck("500", "Internal Server Error", "/500.html", req); // need a page for this
	}
}

void Serv::deleteFolderMethod(std::string path, Request req)
{
	if (deleteFolderRecusively(path))
	{
		printlog("Succefully deleted folder", -1, GREEN);
		// errorPageCheck("204", "No Content", "/204.html", req); I dont know if this works, test
		parseSend("HTTP/1.1 204 No Content\r\nConnection: keep-alive\r\n", req.ClientFd());
	}
	else
	{
		printerr("Failed to delete folder ", -1, RED);
		errorPageCheck("500", "Internal Server Error", "/500.html", req); // need a page for this
	}
}

void Serv::chunkedResponse(Request req)
{
	// std::cout << "ENTER CHUNKED RESPONSE!" << std::endl;
	size_t max;
	std::istringstream(serv_info.max_body_size) >> max;
	if (req.content.getContentSize() > max)
		errorPageCheck("413", "Payload Too Large", "/413.html", req);
	else
	{
		std::string content(req.content.getContent());
		if (content != "")
		{
			// std::cout << "Are we here?" << std::endl;
			std::stringstream ss;
    		ss << req.content.getContentSize();
			std::string response = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\nContent-Length: " + ss.str() + "\r\n\r\n";
			// std::cout << "CONTENT: " << response << content << std::endl;
			parseSend(response + content, req.ClientFd());
		}
		else
			errorPageCheck("404", "Not Found", "DefaultError/404.html", req);
	}

}

bool Serv::redirection(std::string path, Request req)
{
	std::vector<Location>::iterator it;
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		// std::cout << "HERE: " << path << " | " << it->redirect_path << std::endl;
		if (it->path == path && !(it->redirect_path.empty()))
		{
			parseSend("HTTP/1.1 301 Moved Permanently\r\nLocation:" + it->redirect_path + "\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n", req.ClientFd());
			return true;
		}
    }
    return false;
}

void	Serv::PrepareResponse( std::string method, std::string path, Request req )
{
	// std::cout << method << std::endl;
	if (CheckAllowed(method, path) == ALLOWED)
	{
		std::string abs = createAbsolutePath(path);
		// std::cout << abs << std::endl;
		if (*(abs.begin()) == '/')
			abs = abs.substr(1);
		if (req.TransferEncoding() == "chunked")
			return chunkedResponse(req);
		if (redirection(path, req))
			return ;
		struct stat path_stat;
		// std::cout << " aabs" << abs << std::endl;
		if (access(abs.c_str(), F_OK) != -1)
		{ // need forbidden for no access.
			if (stat(abs.c_str(), &path_stat) != 0)
				perror("Error getting file status");
			int check = S_ISDIR(path_stat.st_mode);
			if (check == 0) // it is a file
			{
				if (!access(abs.c_str(), R_OK))
				{
					// std::cout << "FUCK" << std::endl;
					std::string theExtension = CheckCGI(path);
					if (method == "DELETE")
						deleteMethod(abs, req);
					else if (theExtension != "") // it is a CGI script
						parseSend(sendby_CGI(cgi_request(req, abs, theExtension)), req.ClientFd());
					// else if (method == "POST")
					// 	errorPage()
					else // "normal" request
						parseSend(getResponse(abs, "", getHeader("200 OK", "", abs)), req.ClientFd());
				}
				else
					errorPageCheck("403", "Forbidden", "DefaultError/403.html", req);
			}
			else // it is a folder
			{
				abs = removeDashIfExists(abs);
				path = removeDashIfExists(path);
				if (method == "DELETE")
					deleteFolderMethod(abs, req);
				std::string index = CheckIndex(path);
				// std::cout << "index " << index << std::endl;
				if (!(index.empty()))
					parseSend(getResponse(abs, index, getHeader("200 OK", "", index)), req.ClientFd());
				else
				{
					if (CheckAutoindex(path) && !access(abs.c_str(), R_OK))
							parseSend(makeDirectoryList(abs, path), req.ClientFd());
					else
						errorPageCheck("403", "Forbidden", "DefaultError/403.html", req); // changed to 403, confirm with nginx
				}
			}
		}
		else // if it does not exist, error 404
		{
			// std::cout << "IT'S HERE!!!!" << std::endl;
			errorPageCheck("404", "Not Found", "DefaultError/404.html", req);
		}
	}
	else
	{
		printerr("METHOD NOT ALLOWED.", 0,YELLOW);
		errorPageCheck("405", "Method Not Allowed", "/405.html", req); // need to do the 405 page.
	}
}

bool	Serv::CheckAutoindex(std::string path)
{
	if (path == "/" && serv_info.autoindex != "")
		return true;
	std::vector<Location>::iterator it;
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (it->path == path && !(it->autoindex.empty()))
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
	int error_flag = 0;

	if (cgi_fd > 2)
	{	
		while ((bytesRead = read(cgi_fd, buffer, sizeof(buffer))) > 0)
			response.append(buffer, bytesRead);

		if (bytesRead < 0)
		{
			perror("Error reading from file descriptor");
			error_flag = 500;
		}
		if (response.empty())
			error_flag = 204;
		else
		{
			responseHeaders = "HTTP/1.1 200 OK\r\n";
			responseHeaders += "Content-Type: text/html\r\n";
			responseHeaders += "Connection: keep-alive\r\n";
			std::stringstream ss;
			ss << response.length();
			responseHeaders += "Content-Length: " + ss.str() + "\r\n\r\n";
			close(cgi_fd);
		}
	}
	if (error_flag == 500 || cgi_fd <= 2)
	{
		if (serv_info.error_pages["500"].empty())
			response = getResponse(serv_info.root, "/500.html", getHeader("500 Internal Server Error", "", "/500.html"));
		else
			response = getResponse(serv_info.root, serv_info.error_pages["500"], getHeader("500 Internal Server Error", "", serv_info.error_pages["500"]));
	}
	else if (error_flag == 204)
		responseHeaders = "HTTP/1.1 204 No Content\r\nConnection: keep-alive\r\n";
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

void printMethods(std::map<std::string, Methods> map)
{
	std::map<std::string, Methods>::const_iterator its;
	std::cout << "PRINTING: methods" << std::endl;
	for (its = map.begin(); its != map.end(); its++)
	{
		std::cout << "Method: " << its->first << " : "; 
		switch (its->second)
		{
			case ALLOWED :
				std::cout << "allowed";
				break;
			case DENIED :
				std::cout << "denied";
				break;
			default :
				std::cout << "undefined";
				break;
		}
		std::cout << std::endl;
	}
}

Methods	Serv::CheckAllowed( std::string method, std::string path)
{
	//if (*(path.end() - 1) == '/')
	// std::cout << path << std::endl;
	size_t i = path.rfind("/");
	std::string newPath;
	if (*(path.end() - 1) != '/' && path.substr(i).find(".") != std::string::npos)
		newPath = path.substr(0, i + 1);
	else
		newPath = path;
	newPath = removeDashIfExists(newPath);
	// std::cout << newPath << std::endl;
	while (newPath != "")
	{
		// std::cout << newPath << std::endl;
		std::vector<Location>::iterator it;
		for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
		{
			if (it->path == newPath)
			{
				if (it->methods.find(method) != it->methods.end())
				{
					// std::cout << "NEWPATH: " << newPath << " PATH: " << it->path << " METHOD: " << it->methods[method] << std::endl; 
					return whatstheMethod(it->methods[method], method);
				}
			}
		}
		i = newPath.rfind("/");
		newPath = newPath.substr(0, i);
	}
	if (serv_info.methods.find(method) != serv_info.methods.end())
		return whatstheMethod(serv_info.methods[method], method);
    return UNDEFINED;
}

int Serv::establish_connection()
{
    struct addrinfo *addr;
    if (getaddrinfo(serv_info.host.c_str(), serv_info.port.c_str(), NULL, &addr) < 0){ // port 80 to not write everytime the port with the address
        std::cerr << "Error: couldn't get address" << std::endl;
        return 1;
    }
    this->socketfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (socketfd < 0) {
        return failToStart("Error: socket creation", addr, socketfd);
	}
	int opt = 1;
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) // ignores wait time for rebinding
		return failToStart("Error: socket optimise", addr, socketfd);
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
	std::map<std::string, Methods>::const_iterator its;
	for (its = serv_info.methods.begin(); its != serv_info.methods.end(); its++)
	{
		std::cout << "Method: " << its->first << " : " << (its->second ? "denied" : "allowed") << std::endl;
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
		std::map<std::string, Methods>::const_iterator it3;
		for (it3 = it->methods.begin(); it3 != it->methods.end(); it3++)
		{
			std::cout << "Method: " << it3->first << " : " << (it3->second ? "denied" : "allowed") << std::endl;
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

bool Serv::compareServerName(std::string ServName)
{
	std::vector<std::string>::iterator name;

	for (name = serv_info.server_name.begin(); name != serv_info.server_name.end(); ++name)
	{
		if (ServName == *name)
			return true;
	}
	return false;
}

int Serv::getSocket()
{
	return socketfd;
}

std::string Serv::getMaxBodySize()
{
	return serv_info.max_body_size;
}