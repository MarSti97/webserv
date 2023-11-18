#include "./includes/webserv.hpp"


// int glob_fd;
// int cli_glob;

int main(int ac, char **av, char **env)
{
    if (ac != 2){
    	return 1;
	}
	Servers servs;
    try {
        if (!correctfile(std::string(av[1])))
		    throw NotConfigFile();
        servs = Servers(std::string(av[1]), env);
        servs.init();
    }
    catch (const std::exception& e) {
        printerr(e.what(), -2, RED);
        return 0;
    }
	servs.printAll();
	servs.run();

    return 0;
}

int end_loop(int end)
{
    static int loop;
    if (end)
        loop = end;
    return loop;
}

std::string getHeader( std::string ARG, std::string extra, std::string filePath )
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

bool headcheck(std::string buf)
{
    if (buf.substr(0, 5) == "POST " || buf.substr(0, 4) == "GET " || buf.substr(0, 7) == "DELETE " || buf.find("HTTP/1.1\r\n") != std::string::npos)
        return true;
    return false;
}

Request postThings(std::string findbuffer, char *buffer, int fd, int size)
{
    Download &instance = Download::getInstance();
    bool flag = headcheck(findbuffer);
    size_t oi = findbuffer.find("POST ");
    if (oi != std::string::npos || !flag)
    {
        // std::cout << "COME HERE " << std::endl;
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

void	printerr(std::string msg, int arg, std::string color)
{
    if (arg == -1)
	    std::cerr << color << makeStamp() << " " << msg << " " << NOCOLOR << std::endl; 
    else if (arg != -2)
	    std::cerr << color << makeStamp() << " " << msg << " " << arg << NOCOLOR << std::endl;
    else
        std::cerr << color << msg << std::endl;
}

void	printlog(std::string msg, int arg, std::string color)
{
    if (arg == -1)
	    std::cout << color << makeStamp() << " " << msg << " " << NOCOLOR << std::endl; 
    else
	    std::cout << color << makeStamp() << " " << msg << " " << arg << NOCOLOR << std::endl; 
}

std::string makeStamp( void )
{
    time_t currentTime;
    struct tm *localTimeInfo;

    // Get the current time
    time(&currentTime);

    // Convert the current time to a local time structure
    localTimeInfo = localtime(&currentTime);
    std::stringstream hours, minutes, day, month, year;
    // ss << length;
    std::string lengthStr = hours.str();
    // Extract the individual components of the time
    hours << localTimeInfo->tm_hour;
    minutes << localTimeInfo->tm_min;
    day << localTimeInfo->tm_mday;
    month << localTimeInfo->tm_mon + 1; // Months are 0-based
    year << localTimeInfo->tm_year + 1900; // Years are counted from 1900

    // Print the formatted timestamp
    return hours.str() + ':' + minutes.str() + '/' + day.str() + '-' + month.str() + '-' + year.str();
}

int acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fds)
{
    // Accept incoming connection and add the client socket to the fds array
    int servfd = socketfd;
    int clientsocket = accept(socketfd, (struct sockaddr *)&clientinfo, &size);
    if (clientsocket != -1)
    {
        printlog("NEW CLIENT", clientsocket - 2, GREEN);
        ClientServer(clientsocket, servfd, NEWCLIENT);
        pollfd client_pollfd;
        client_pollfd.fd = clientsocket;
        client_pollfd.events = POLLIN | POLLOUT; // Monitor for read and write
        fds->push_back(client_pollfd);
    }
    else if (clientsocket < 0)
    {
        // No incoming connections, continue with the loop
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 1;
        }
        else if (errno == ECONNABORTED) {
            // Handle client disconnect gracefully
            // You can log the disconnection and continue with the loop
            printerr("CLIENT DISCONNECTED" , 0, RED);
            return 1;
        }
        printerr("Error: client connection failed", 0, RED);
        // close(socketfd);
            return 1;
    }
    return 0;
}

std::string findcommand(std::string command)
{
	std::string pathStr = std::getenv("PATH");
    
    if (pathStr.empty())
        return NULL;
    
    // Tokenize the PATH variable using ':' (Unix-like systems) or ';' (Windows)
    char delimiter = ':';
    
    std::vector<std::string> pathDirs;
    size_t start = 0;
    size_t end = pathStr.find(delimiter);
    
    while (end != std::string::npos) {
        std::string dir = pathStr.substr(start, end - start);
        pathDirs.push_back(dir);
        start = end + 1;
        end = pathStr.find(delimiter, start);
    }
    std::string lastDir = pathStr.substr(start);
    pathDirs.push_back(lastDir);
    std::vector<std::string>::iterator it;
    for (it = pathDirs.begin(); it != pathDirs.end(); it++)
    {
        if (access((*it + command).c_str(), F_OK) == 0)
            break ;
    }
	return (*it + command);
}

int GetbyUser(std::string buffer)
{
    size_t user = buffer.find("Sec-Fetch-User: ");
    if (user != std::string::npos)
        return 1;
    return 0;
}

int failToStart(std::string error, struct addrinfo *addr, int socketfd)
{
    printerr(error, 0, RED);
    freeaddrinfo(addr);
    if (socketfd > 0)
        close(socketfd);
    return 1;
}

void	ctrlc(int signum) // need to fix this
{
	if (signum == SIGINT)
	{
        end_loop(1);
        Download &down = Download::getInstance();
        down.clean();
		// close(glob_fd);
		// close(cli_glob);
		// exit(EXIT_FAILURE);
	}
}