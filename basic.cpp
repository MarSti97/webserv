#include "webserv.hpp"

int main()
{
    // struct sockaddr_in addr;
    // addr.sin_family = AF_UNIX;
    // addr.sin_port = htons(80);
    // addr.sin_addr.s_addr = (127U << 24) | (0U << 16) | (0U << 8) | 1U;
    struct addrinfo *addr;
    if (getaddrinfo("127.0.0.1", "80", NULL, &addr) < 0){
        std::cerr << "Error: couldn't get address" << std::endl;
        return 1;
    }
    int socketfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (socketfd < 0){
        std::cerr << "Error: socket creation" << std::endl;
        freeaddrinfo(addr);
        return 1;
    }
    if (bind(socketfd, addr->ai_addr, addr->ai_addrlen) == -1){
        std::cerr << "Error: bind unsuccesful" << std::endl;
        freeaddrinfo(addr);
        close(socketfd);
        return 1;
    }
    if (listen(socketfd, 5) == -1){
        std::cerr << "Error: listen unsuccesful" << std::endl;
        freeaddrinfo(addr);
        close(socketfd);
        return 1;
    }
    struct sockaddr_in clientinfo;
    socklen_t size = sizeof(clientinfo);
    char buffer[1024];
    while (1)
    {
        int clientsocket = accept(socketfd, (struct sockaddr*)&clientinfo, &size);
        if (clientsocket < 0){
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
        const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nWell hello motherfuckers!";
        n = write(clientsocket, response, std::strlen(response));
        if (n < 0) {
            std::cerr << "Error writing to socket" << std::endl;
            return 1;
        }
    }
    return 0;
}