#include "./includes/webserv.hpp"

int glob_fd;
int cli_glob;

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;
    if (av[1])
        {}
    struct addrinfo *addr;
    if (getaddrinfo("127.73.73.7", "7681", NULL, &addr) < 0){ // port 80 to not write everytime the port with the address
        std::cerr << "Error: couldn't get address" << std::endl;
        return 1;
    }
    int socketfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (socketfd < 0) {
        return failToStart("Error: socket creation", addr, socketfd);
	}
	int opt = 1;
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) // ignores wait time for rebinding
		return failToStart("Error: socket optimise", addr, socketfd);
	// printlog("socket created", socketfd);
	glob_fd = socketfd;
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
    struct sockaddr_in clientinfo;
    socklen_t size = sizeof(clientinfo);
    char buffer[1024];


    freeaddrinfo(addr);

    std::vector<pollfd> fds;
    fds.push_back(pollfd());
    fds[0].fd = socketfd;
    fds[0].events = POLLIN; // Monitor server socket for incoming connections

    while (1)
    {
        bzero(buffer, 1024);
        // std::cout << fds.size() << std::endl;
        int ret = poll(&fds[0], fds.size(), -1);
        if (ret == -1)
        {
            std::cerr << "Error in poll" << std::endl;
            continue;
        }

        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == socketfd)
                {
                    if (acceptConnection(socketfd, &clientinfo, size, &fds))
                        continue;
                }
                else if (fds[i].revents & POLLOUT)
                {
                    // std::cout << "request received from client " << (struct sockaddr *)clientinfo.sin_addr.s_addr << std::endl;
                    if (!parseRecv(fds, i, buffer))
                        parseSend(fds, i, buffer);
                    // std::cout << " " << n << std::endl;
                }
            }
        }
    }
    return 0;
}

int parseSend(std::vector<pollfd> &fds, int pos, char *buffer)
{
    std::string response = getResponse(buffer, "guarder-html", "/index.html");
    // std::cout << response << std::endl;

    // std::cout << response.size();
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
    close(fds[pos].fd);
    fds.erase(fds.begin() + pos);
    return n;
}


std::string getResponse(char *buffer, std::string path, std::string index)
{
    std::string filePath;
    std::string mimeType;
    std::string responseHeaders;

    filePath = getURL(buffer);
    mimeType = getMimeType(filePath);
    responseHeaders = "HTTP/1.1 200 OK\r\n";
    responseHeaders += "Content-Type: " + getMimeType(filePath) + "\r\n\r\n";
    if (filePath.empty() || filePath == "/")
        return responseHeaders + readFile(path + index);
    else
    {
        std::string response = readFile(path + filePath);
        if (response == "" || response.empty())
            response = readFile(path + "/404.html");
        return responseHeaders + response;//"HTTP/1.1 404 \r\nContent-Type: text/html\r\n\r\nError page, leave now!\r\n";
    }
}

int GetbyUser(std::string buffer)
{
    size_t user = buffer.find("Sec-Fetch-User:");
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

int parseRecv(std::vector<pollfd> &fds, int pos, char *buffer)
{
    ssize_t n = recv(fds[pos].fd, buffer, 1023, 0);

    std::string findbuffer(buffer);
    // std::cout << findbuffer << std::endl;
    // std::cout << GetbyUser(findbuffer) << std::endl;
    // if (GetbyUser(findbuffer)) // check if USER can get the page that he wrote.
    // {
    //     if (checkAllowGet(getURL(buffer), 0))
    // }
    if (n <= 0)
    {
        if (n == 0)
        {
            // Connection closed by the client
            close(fds[pos].fd);
            fds.erase(fds.begin() + pos);
            return 1;
        }
        std::cerr << "Error reading from poll" << std::endl;
        perror("read");
        return 1;
    }
    size_t ok = findbuffer.find("\r\n\r\n");
    if (ok == std::string::npos)
    {
        std::cout << "bad request received" << std::endl;
        std::cout << buffer << n << std::endl;
    }
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

void	ctrlc(int signum)
{
	if (signum == SIGINT)
	{
		close(glob_fd);
		close(cli_glob);
		exit(EXIT_FAILURE);
	}
}

void	printlog(std::string msg, int arg)
{
	std::cout << "server log [8-9-23] " << msg << " " << arg << std::endl; 
}

int acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fds)
{
    // Accept incoming connection and add the client socket to the fds array
    int clientsocket = accept(socketfd, (struct sockaddr *)&clientinfo, &size);
    // std::cout << clientsocket << std::endl;
    if (clientsocket != -1)
    {
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