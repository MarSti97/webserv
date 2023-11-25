#include "./includes/webserv.hpp"

int Serv::establish_connection()
{
    std::istringstream iss(serv_info.port);
    int port = 0;
    iss >> port;
    struct addrinfo *addr;
    if (port == 0 || port > 65535 || getaddrinfo(serv_info.host.c_str(), serv_info.port.c_str(), NULL, &addr) < 0){
        printerr("Error: couldn't get address. Not allowed to use: " + serv_info.host + ":" + serv_info.port, -1, RED);
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
	return 0;
}

int Serv::failToStart(std::string error, struct addrinfo *addr, int socket)
{
    printerr(error, -1, RED);
    freeaddrinfo(addr);
    if (socket > 0)
        close(socket);
    return 1;
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

std::string Serv::getServerHostPort()
{
	return serv_info.host + ":" + serv_info.port;
}