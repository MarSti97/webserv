#include "./includes/webserv.hpp"


int glob_fd;
int cli_glob;

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
        std::cerr << e.what() << std::endl;
        return 0;
    }
	servs.printAll();
	servs.run();
    return 0;
}

int parseSend(std::vector<pollfd> &fds, int pos, Request req, char **env)
{
    std::string response = getResponse(req, "guarder-html", "/index.html", env);
    // std::cout << response << std::endl;

    // std::cout << response.size();
    if (!response.empty())
    {
        ssize_t n = send(fds[pos].fd, response.c_str(), response.size(), 0);
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
        // close(fds[pos].fd);
        // fds.erase(fds.begin() + pos);
        return n;
    }
    else
        std::cout << "post chuncked" << std::endl;
    return 0;
}


std::string getResponse(Request req, std::string path, std::string index, char **env)
{
    std::string filePath;
    std::string mimeType;
    std::string responseHeaders;
    // std::cout << req.request() << std::endl;
    // std::cout << req.Get() << std::endl;

    filePath = req.Get();
    (void)env;
    if (filePath.empty())
	{
        // // std::cout << "here" << std::endl;
        // if (execute_command(findcommand("/bash"), req.Post(), env) != 0)
        // {
        //     std::cout << "here11" << std::endl;
            return "";
        // }
        // else
        // {
        //     // std::cout << "here22" << std::endl;
        //     std::string response = readFile(path + req.Referer());
        //     responseHeaders = "HTTP/1.1 302 Found\r\n";
        //     responseHeaders += "Location: " + filePath + "\r\n\r\n";
        //     return responseHeaders + response;		
        // }
	}
    // std::cout << "here44" << std::endl;

    mimeType = getMimeType(filePath);
    // std::cout << filePath << std::endl;

    // std::cout << mimeType << std::endl;
    responseHeaders = "HTTP/1.1 200 OK\r\n";
    responseHeaders += "Content-Type: " + mimeType + "\r\n";
    responseHeaders += "Connection: keep-alive\r\n";
    if (filePath.empty() || filePath == "/")
        return responseHeaders + readFile(path + index);
    else
    {
        std::string response = readFile(path + filePath);
        // std::cout << path << " " << filePath << std::endl;
        std::stringstream ss;
        ss << response.length();
        responseHeaders += "Content-Length: " + ss.str() + "\r\n\r\n";
        // std::cout << responseHeaders << std::endl;
        // char buf[1024];
        // getcwd(buf, 1023);
        // std::cout << buf << std::endl;
        if (response == "" || response.empty())
            response = readFile(path + "/404.html");
        return responseHeaders + response; //"HTTP/1.1 404 \r\nContent-Type: text/html\r\n\r\nError page, leave now!\r\n";
    }
}

bool headcheck(std::string buf)
{
    if (buf.substr(0, 5) == "POST " || buf.substr(0, 4) == "GET " || buf.substr(0, 7) == "DELETE ")
        return true;
    return false;
}


bool postThings(std::string findbuffer, char *buffer, int fd, int size)
{
    size_t oi = findbuffer.find("POST ");
    bool flag = headcheck(findbuffer);
    if (oi != std::string::npos || !flag)
    {
        char* Str = new char[size];
        memcpy(Str, buffer, size);
        Download &instance = Download::getInstance();
        if (!flag)
        {
            std::cout << "YES: ITS TRUE" << std::endl;
            std::string boundary = getINFOtwo(findbuffer, "boundary=", 9);
	        std::string contentlength = getINFOtwo(findbuffer, "Content-Length: ", 16);
            instance.add_map(fd, imgDown(atoi(contentlength.c_str()), size, buffer, boundary));
            return true;
        }
        instance.append_map(fd, Str, size);
        return true;
    }
    return false;
}

std::string parseRecv(std::vector<pollfd> &fds, int pos)
{
    char buffer[4096];
    char *buf = NULL;
    std::string findbuffer;
    ssize_t n;
    int counter = 0;
    int buf_size = 0;
    while (1)
    {
        bzero(buffer, sizeof(buffer));
        n = recv(fds[pos].fd, buffer, 4096, 0);
        // std::cout << n << " recv"<< std::endl;
        if (n <= 0)
        {
            if (n == 0)
            {
                if (!counter)
                {
                    // Connection closed by the client
                    printlog("LOST CLIENT", fds[pos].fd, RED);
                    close(fds[pos].fd);
                    fds.erase(fds.begin() + pos);
                    return "";
                }
                break;
            }
            else if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // No data available for non-blocking receive
                std::cout << "all recv" << std::endl;
                break;
            } else {
                // Handle other receive errors
                std::cerr << "Error reading from poll" << std::endl;
                perror("read");
                return "";
            }
        }
        else
        {
            if (!buf)
                buf = new char[n];
            else
            {
                char *tmp;
                tmp = buf;
                buf = new char[buf_size + n];
                memcpy(buf, tmp, buf_size);
            }
            memcpy(buf + buf_size, buffer, n);
            if (n < 4096)
                break;
        }
        counter++;
        buf_size += n;
        // std::cout << findbuffer.size() << " " << buf_size << std::endl;
    }
    // for (int i = 0; i < buf_size; ++i)
    //     write(2, &buf[i], 1);
    // std::ofstream img("img", std::ios::trunc);
    // if (img.is_open())
    //     img << buf;
    // std::cout << buf_size << std::endl;
    findbuffer = std::string(buffer);
	// std::cout << findbuffer << std::endl;
    size_t ok = findbuffer.find("\r\n\r\n");
    if (ok == std::string::npos)
    {
        std::cout << "bad request received" << std::endl;
        std::cout << buffer << n << std::endl;
        return "";
    }
    // if (postThings(findbuffer, buffer, fds[pos].fd, n))
    //     return "";
    return findbuffer;
}

void	printlog(std::string msg, int arg, std::string color)
{
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
    int clientsocket = accept(socketfd, (struct sockaddr *)&clientinfo, &size);
    // std::cout << clientsocket << std::endl;
    if (clientsocket != -1)
    {
        printlog("NEW CLIENT", clientsocket - 2, GREEN);
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
        std::cerr << "Client disconnected" << std::endl;
            return 1;
        }
        std::cerr << "Error: client connection failed" << std::endl;
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

int checkAllowGet(std::string folder, std::vector<Location> Locations)
{
    std::vector<Location>::iterator it;

    for (it = Locations.begin(); it != Locations.end(); ++it)
    {
        if (it->root == folder)
            break;
    }
    return it->allow_get;
}

int checkAllowPost(std::string folder, std::vector<Location> Locations)
{
    std::vector<Location>::iterator it;

    for (it = Locations.begin(); it != Locations.end(); ++it)
    {
        if (it->root == folder)
            break;
    }
    return it->allow_post;
}

int checkAllowDelete(std::string folder, std::vector<Location> Locations)
{
    std::vector<Location>::iterator it;

    for (it = Locations.begin(); it != Locations.end(); ++it)
    {
        if (it->root == folder)
            break;
    }
    return it->allow_delete;
}


int failToStart(std::string error, struct addrinfo *addr, int socketfd)
{
    std::cerr << error << std::endl;
    freeaddrinfo(addr);
    if (socketfd > 0)
        close(socketfd);
    return 1;
}

void	ctrlc(int signum) // need to fix this
{
	if (signum == SIGINT)
	{
		close(glob_fd);
		close(cli_glob);
		exit(EXIT_FAILURE);
	}
}