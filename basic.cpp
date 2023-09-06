#include "webserv.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;
    if (av[1])
        {}
    // struct sockaddr_in addr;
    // addr.sin_family = AF_UNIX;
    // addr.sin_port = htons(80);
    // addr.sin_addr.s_addr = (127U << 24) | (0U << 16) | (0U << 8) | 1U;
    struct addrinfo *addr;
    if (getaddrinfo("127.73.73.7", "7681", NULL, &addr) < 0){ // port 80 to not write everytime the port with the address
        std::cerr << "Error: couldn't get address" << std::endl;
        return 1;
    }
    int socketfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (socketfd < 0)
        return failToStart("Error: socket creation", addr, socketfd);
    int flags = fcntl(socketfd, F_GETFL, 0); // set the socket to non-blocking;
    if (flags == -1)
        return failToStart("Error getting socket flags", addr, socketfd);
    if (fcntl(socketfd, F_SETFL, flags | O_NONBLOCK) == -1)
        return failToStart("Error setting socket to non-blocking", addr, socketfd);
    if (bind(socketfd, addr->ai_addr, addr->ai_addrlen) == -1)
        return failToStart("Error: bind unsuccesful", addr, socketfd);
    if (listen(socketfd, 2) == -1)
        return failToStart("Error: listen unsuccesful", addr, socketfd);
    struct sockaddr_in clientinfo;
    socklen_t size = sizeof(clientinfo);
    char buffer[1024];
    while (1)
    {
        int clientsocket = accept(socketfd, (struct sockaddr*)&clientinfo, &size);
        if (clientsocket < 0){
            // No incoming connections, continue with the loop
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            std::cerr << "Error: client connection failed" << std::endl;
            close(socketfd);
            return 1;
        }
        bzero(buffer, 1024);
        int n = read(clientsocket, buffer, 1023);
        if (n < 0) {
            std::cerr << "Error reading from socket" << std::endl;
            return 1;
        }

        std::string filePath = getURL(buffer);
        
        // Get the MIME type based on the file extension
        std::string mimeType = getMimeType(filePath);

        // Construct the HTTP response headers
        std::string responseHeaders = "HTTP/1.1 200 OK\r\n";
        responseHeaders += "Content-Type: " + getMimeType(filePath) + "\r\n\r\n";

        // Send the response headers
        int f = write(clientsocket, responseHeaders.c_str(), responseHeaders.size());
        if (f < 0) {
            std::cerr << "Error writing to socket" << std::endl;
            return 1;
        }

        std::string response; 
/*         if (filePath.empty() || filePath == "/") */
            response = readFile("guarder-html/index.html");
        /*else
            response = readFile(filePath);//"HTTP/1.1 404 \r\nContent-Type: text/html\r\n\r\nError page, leave now!\r\n";
        if (!response.empty())
        { */
            n = write(clientsocket, response.c_str(), response.size());
            if (n < 0) {
                std::cerr << "Error writing to socket" << std::endl;
                return 1;
            }
/*         } */
		close(clientsocket);
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