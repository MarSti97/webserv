#include "./includes/webserv.hpp"

Request Servers::postThings(std::string findbuffer, char *buffer, int fd, int size)
{
    Download &instance = Download::getInstance();
    bool flag = headcheck(findbuffer);
    size_t oi = findbuffer.find("POST ");
    if (oi != std::string::npos || !flag)
    {
        if (oi != std::string::npos)
        {
            std::string boundary = getINFOtwo(findbuffer, "boundary=", 9);
	        std::string contentlength = getINFOtwo(findbuffer, "Content-Length: ", 16);
	        size_t headerlength = findbuffer.find( "\r\n\r\n");
            instance.add_map(fd, imgDown((atoi(contentlength.c_str()) + (headerlength + 4)), size, buffer, boundary));
        }
        else
            instance.append_map(fd, buffer, size);
    }
    return instance.isitFULL(fd, buffer, size);
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
		errorPageCheck("501", "Not Implemented", "DefaultError/501.html", req);
	}
}

void	Serv::PrepareResponse( std::string method, std::string path, Request req )
{
	printlog("REQUEST: " + getFirstLine(req.request()) + " FROM CLIENT", req.ClientFd() - 2, CYAN);
	if (CheckAllowed(method, path) == ALLOWED)
	{
		std::string abs = createAbsolutePath(path);
		if (*(abs.begin()) == '/')
			abs = abs.substr(1);
		if (req.TransferEncoding() == "chunked")
			return chunkedResponse(req);
		if (redirection(path, req))
			return ;
		struct stat path_stat;
		if (access(abs.c_str(), F_OK) != -1)
		{
			if (stat(abs.c_str(), &path_stat) != 0)
				perror("Error getting file status");
			int check = S_ISDIR(path_stat.st_mode);
			if (check == 0) // it is a file
			{
				if (!access(abs.c_str(), R_OK))
				{
					std::string theExtension = CheckCGI(path);
					if (method == "DELETE")
						deleteMethod(abs, req);
					else if (theExtension != "") // it is a CGI script
						parseSend(sendby_CGI(cgi_request(req, abs, theExtension)), req.ClientFd(), req);
					else if (method == "POST")
						parseSend(getResponse(abs, "", getHeader("204 No Content", "", abs)), req.ClientFd(), req);
					else // "normal" request
						parseSend(getResponse(abs, "", getHeader("200 OK", "", abs)), req.ClientFd(), req);
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
				if (!(index.empty()))
					parseSend(getResponse(abs, index, getHeader("200 OK", "", index)), req.ClientFd(), req);
				else
				{
					if (CheckAutoindex(path) && !access(abs.c_str(), R_OK))
							parseSend(makeDirectoryList(abs, path), req.ClientFd(), req);
					else
						errorPageCheck("403", "Forbidden", "DefaultError/403.html", req);
				}
			}
		}
		else
		{
			errorPageCheck("404", "Not Found", "DefaultError/404.html", req);
		}
	}
	else
	{
		errorPageCheck("405", "Method Not Allowed", "DefaultError/405.html", req);
		if (req.content.getChunkedBool())
			req.content.clean();
	}
}

void Serv::errorPageCheck(std::string code, std::string message, std::string defaultError, Request req)
{
	std::string page = "/" + code + ".html";
	std::string error = code + " " + message;
	if (*(serv_info.error_pages[code].begin()) != '/' && !(serv_info.error_pages[code].empty()) && *(serv_info.root.end() - 1) != '/')
		serv_info.root = serv_info.root + "/";
	if (serv_info.error_pages[code].empty() || (access(((serv_info.root + serv_info.error_pages[code]).substr(1)).c_str(), F_OK)) == -1)
		parseSend(getResponse("DefaultError", page, getHeader(error, "", defaultError)), req.ClientFd(), req);
	else
		parseSend(getResponse(serv_info.root.substr(1), serv_info.error_pages[code], getHeader(error, "", serv_info.error_pages[code])), req.ClientFd(), req);
}
