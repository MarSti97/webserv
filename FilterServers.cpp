#include "includes/Servers.hpp"

int	Servers::checkSockets(int fd)
{
	std::vector<Serv>::iterator it;
	for (it = servs.begin(); it != servs.end(); ++it)
	{
		if (it->getSocket() == fd)
			return fd;
	}
	return 0;
}

bool	Servers::getCorrectServ(Request req, int clientfd, ServSelect option)
{
	req.SetClientFd(clientfd);
	std::vector<Serv>::iterator it;
	for (it = servs.begin(); it != servs.end(); ++it)
	{
		if (it->getSocket() == ClientServer(clientfd, 0, CLIENTSOCKET))
		{
			if (it->compareHostPort(req.Host(), req.Port()) || it->compareServerName(req.Host()))
			{
				if (option == MAXCHECK)
				{
					if (!checkContentSizeToMax(req, *it))
						return false;
				}
				else if (req.getContinue100())
					it->parseSend("HTTP/1.1 100 Continue\r\nConnection: keep-alive\r\n", clientfd, req);
				else if (headcheck(req.request()))
					it->filterRequest(req);
				else
					it->errorPageCheck("400", "Bad Request", "DefaultError/400.html", req);
				break;
			}
			else
				it->errorPageCheck("400", "Bad Request", "DefaultError/400.html", req);
		}
	}
	return true;
}

bool Servers::checkContentSizeToMax(Request req, Serv serv)
{
	std::string max_string = serv.getMaxBodySize();
	std::string len_string = req.Contentlength();
	if (max_string == "" || len_string == "")
	{
		return true;
	}
	long max;
	long contentlen;

	std::istringstream(max_string) >> max;
	std::istringstream(len_string) >> contentlen;
	if (max < contentlen)
	{
		serv.errorPageCheck("413", "Payload Too Large", "DefaultError/413.html", req);
		return false;
	}
	return true;
}

int Servers::acceptConnection(int socketfd, struct sockaddr_in *clientinfo, socklen_t &size, std::vector<pollfd> *fd)
{
    int servfd = socketfd;
    int clientsocket = accept(socketfd, (struct sockaddr *)&clientinfo, &size);
    if (clientsocket != -1)
    {
        printlog("NEW CONNECTION WITH CLIENT", clientsocket - 2, GREEN);
        ClientServer(clientsocket, servfd, NEWCLIENT);
        pollfd client_pollfd;
        client_pollfd.fd = clientsocket;
        client_pollfd.events = POLLIN | POLLOUT; // Monitor for read and write
        fd->push_back(client_pollfd);
    }
    else if (clientsocket < 0)
    {
        // No incoming connections, continue with the loop
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 1;
        }
        else if (errno == ECONNABORTED) {
            // Handle client disconnect
            printerr("CLIENT DISCONNECTED" , -1, RED);
            return 1;
        }
        printerr("Error: client connection failed", -1, RED);
            return 1;
    }
    return 0;
}

int	ClientServer(int client, int server, ClientHandle locker)
{
	static std::map<int, int> connect;

	if (client && server && (locker == NEWCLIENT))
	{
		if (connect.find(client) == connect.end())
		{
			connect.insert(std::make_pair(client, server));
		}
		else
			printerr("CLIENT ALREADY MAPPED", client - 2, RED);
	}
	else if (client && (locker == CLIENTSOCKET))
	{
		std::map<int, int>::iterator it = connect.find(client);
		if (it != connect.end())
			return it->second;
		else
			printerr("CLIENT NOT MAPPED", client - 2, PURPLE);
	}
	else if (client && (locker == ERASECLIENT))
	{
		if (connect.find(client) != connect.end())
		{
			connect.erase(client);
		}
		else
			printerr("CLIENT ALREADY ERASED FROM MAP", client - 2, RED);
	}
	return 0;
}

void	Servers::printAll() const
{
	for (size_t i = 0; i < servs.size(); i++)
		servs[i].print(i);
}
