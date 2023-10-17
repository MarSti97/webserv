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

int end_loop(int end)
{
    static int loop;
    if (end)
        loop = end;
    return loop;
}

int Serv::parseSend(std::string response, int fd)
{
    
    // std::cout << req.Get() << std::endl;
    

    // std::cout << response.size();
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
        // close(fds[pos].fd);
        // fds.erase(fds.begin() + pos);
        return n;
    }
    else
        std::cout << "post chuncked" << std::endl;
    return 0;
}

std::string getHeader( std::string ARG, std::string extra, std::string filePath )
{
    std::string mimeType;
    std::string responseHeaders;
    mimeType = getMimeType(filePath);
    // std::cout << filePath << std::endl;

    // std::cout << mimeType << std::endl;
    responseHeaders = "HTTP/1.1 " + ARG + "\r\n";
    responseHeaders += "Content-Type: " + mimeType + "\r\n";
    responseHeaders += "Connection: keep-alive\r\n";
    if (!(extra.empty()))
        responseHeaders += extra + "\r\n";
    return responseHeaders;
}

std::string Serv::getResponse(std::string abs, std::string page, std::string responseHeaders)
{
    std::cout << "DEBUG: getResponse " << (abs.substr(1) + page) << std::endl;
    std::string response = readFile(abs.substr(1) + page); // maybe need the substr(1)
    // if (page != "")
    // {
    //     size_t fi = page.rfind('/');
    //     std::string response;
    //     if (fi != std::string::npos)
    //         response = readFile(abs.substr(1) + page.substr(fi));
    //     else
    //         response = readFile(abs.substr(1) + page);
    // }
    // std::cout << abs + page.substr(fi) << std::endl;
    // std::cout << path << " " << filePath << std::endl;
    std::stringstream ss;
    ss << response.length();
    responseHeaders += "Content-Length: " + ss.str() + "\r\n\r\n";
    // std::cout << responseHeaders << std::endl;
    // char buf[1024];
    // getcwd(buf, 1023);
    // std::cout << response << std::endl;
    if (response == "" || response.empty())
        response = readFile(serv_info.root + "/404.html");
    return responseHeaders + response;

}

bool headcheck(std::string buf)
{
    // std::cout << buf << std::endl;
    if (buf.substr(0, 5) == "POST " || buf.substr(0, 4) == "GET " || buf.substr(0, 7) == "DELETE ")
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
        // std::cout << "COME HERE" << std::endl;
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
    // size_t ok = findbuffer.find("GET ");
    // if (ok != std::string::npos)
    //     return true;
    return instance.isitFULL(fd, buffer, size);
    //     return false;
    // return true;
}

Request Servers::parseRecv(std::vector<pollfd> &fd, int pos)
{
    char buffer[4097];
    // buffer[4096] = '\0';
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
            // char buf[n + 1];
            // buf[n] = '\0';

            //     char tmp[buf_size];
            //     memcpy(tmp, buf, buf_size);
            //     delete[] buf;
            //     buf = new char[buf_size + n + 1];
            //     buf[buf_size + n] = '\0';
            //     memcpy(buf, tmp, buf_size);
            // memcpy(buf + buf_size, buffer, n);
        }
        counter++;
        buf_size += n;
        if (n < 4096)
            break;
        // std::cout << findbuffer.size() << " " << buf_size << std::endl;
    }
    // std::ofstream img("img", std::ios::trunc);
    // if (img.is_open())
    //     img << buf;
    char buf[buf_size + 1];
    std::vector<std::pair<char *, int> >::iterator it;
    int f = 0;
    for (it = full_buf.begin(); it != full_buf.end(); ++it)
    {
        int i = -1;
        // std::cout << it->second << std::endl;
        while (++i < it->second)
            buf[f++] = it->first[i];
        delete[] it->first;
    }
    // std::cout << f << " " << buf_size << std::endl;
    buf[buf_size] = '\0';
    // for (int u = 0; u < buf_size; ++u)
    //     write(1, &buf[u], 1);
    std::vector<std::pair<char *, int> >().swap(full_buf);
    findbuffer = std::string(buf, buf_size);
    // std::cout << "BUFSIZE: " << buf_size << std::endl;
    Request req = postThings(findbuffer, buf, fd[pos].fd, buf_size);
    return req;
    // size_t ok = findbuffer.find("\r\n\r\n");
    // if (ok == std::string::npos)
    // {
    //     std::cout << "bad request received" << std::endl;
    //     std::cout << buffer << n << std::endl;
    //     return "";
    // }
    // return findbuffer;
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
        end_loop(1);
        Download &down = Download::getInstance();
        down.clean();
		close(glob_fd);
		close(cli_glob);
		// exit(EXIT_FAILURE);
	}
}