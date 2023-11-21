#include "./includes/webserv.hpp"

std::string	Serv::getResponse(std::string abs, std::string page, std::string responseHeaders)
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

std::string	Serv::getHeader( std::string ARG, std::string extra, std::string filePath )
{
    std::string mimeType;
    std::string responseHeaders;
    mimeType = getMimeType(filePath);

    responseHeaders = "HTTP/1.1 " + ARG + "\r\n";
    responseHeaders += "Content-Type: " + mimeType + "\r\n";
    responseHeaders += "Connection: keep-alive\r\n";
    if (!(extra.empty()))
        responseHeaders += extra + "\r\n";
    return responseHeaders;
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
		else if (response.empty())
		{
			// std::cout << "EMPTY RESPONSE!!!!" << std::endl;
			error_flag = 204;
		}
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
		responseHeaders = "HTTP/1.1 204 No Content\r\nConnection: keep-alive\r\n\r\n";
	return responseHeaders + response;
}

int Serv::parseSend(std::string response, int fd)
{
    if (!response.empty())
    {
		size_t res = response.find("\r\n");
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
		if (res != std::string::npos)
			printlog("RESPONSE: " + response.substr(0, res) + " TO CLIENT", fd - 2, GREEN); // http = 9;
        return n;
    }
    return 0;
}

void Serv::chunkedResponse(Request req)
{
	size_t max;
	std::istringstream(serv_info.max_body_size) >> max;
	if (req.content.getContentSize() > max)
		errorPageCheck("413", "Payload Too Large", "/413.html", req);
	else
	{

		std::string content;
		if (req.content.getContent())
			content = req.content.getContent();
		if (content != "" || !(content.empty()))
		{
			std::stringstream ss;
    		ss << req.content.getContentSize();
			std::string response = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\nContent-Length: " + ss.str() + "\r\n\r\n";
			// std::cout << "CONTENT: " << response << content << std::endl;
			parseSend(response + content, req.ClientFd());
		}
		else
			errorPageCheck("404", "Not Found", "DefaultError/404.html", req);
	}
	if (req.content.getChunkedBool())
		req.content.clean();
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
