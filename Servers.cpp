#include "./includes/webserv.hpp"

Servers::Servers(std::string file) : config(readFile(file))
{
	if (config == "")
		throw NotConfigFile();
}

void Servers::init()
{
	printlog("Initializing server", -1, YELLOW);
    validate_config();
    std::vector<Serv>::reverse_iterator it;
	int i = 0;
    for (it = servs.rbegin(); it != servs.rend();)
	{
        if (it->establish_connection())
			it = std::vector<Serv>::reverse_iterator(servs.erase(--it.base()));
		else
		{
			it->fds = &this->fds;
			fds.push_back(pollfd());
			fds[i].fd = it->getSocket();
			fds[i].events = POLLIN;
			i++;
			printlog("Server: " + it->getServerHostPort() + " connection established", -1, YELLOW);
			++it;
		}
	}
}

void Servers::run()
{
	if (servs.empty())
		return;
	printlog("Running webserv... ", -1, PURPLE);
	struct sockaddr_in clientinfo;
    socklen_t size = sizeof(clientinfo);
	int timeout = 0;

	while (!end_loop(0))
    {
        for (size_t i = 0; i < fds.size(); ++i)
        {
			timeout = 0;
			if (i >= servs.size())
				timeout = 10000;
			int ret = poll(&fds[i], 1, timeout);
			if (ret == -1)
			{
				if (errno == EINTR)
					printerr("Poll was interupted by a signal", -1, RED);
				else
					printerr("Error in poll", -1, RED);
				continue;
			}
			if (ret == 0 && !checkSockets(fds[i].fd))
			{
				printlog("TIMEOUT CLIENT", fds[i].fd - 2, RED);
			}
			else
			{
				if (fds[i].revents & POLLIN)
				{
					int socketfd = checkSockets(fds[i].fd);
					if (socketfd)
					{
						if (acceptConnection(socketfd, &clientinfo, size, &fds))
							fds[i].events &= ~POLLIN;
						continue;
					}
					else if (fds[i].revents & POLLOUT)
					{
						Request req = parseRecv(fds, i);
						if (!(req.request().empty()))
							getCorrectServ(req, fds[i].fd, DEFAULT);
					}
				}
			}
        }
    }
}

Request Servers::parseRecv(std::vector<pollfd> &fd, int pos)
{
    char buffer[4097];
    std::vector<std::pair<char *, int> > full_buf;
    std::string findbuffer;
    ssize_t n;
    int counter = 0;
    int buf_size = 0;
    while (1)
    {
        bzero(buffer, sizeof(buffer));
        n = recv(fd[pos].fd, buffer, 4096, 0);
        if (n <= 0)
        {
			if (!counter)
			{
				if (n == -1) 
				{
					printerr("ERROR: READING FROM CLIENT", fd[pos].fd - 2, RED);
					perror("read");
				}
				handleLostClient(fd, pos);
				return Request();
			}
			break;
        }
        else
        {
			if (counter == 0)
			{
				printlog("NEW REQUEST FROM CLIENT", fd[pos].fd - 2, YELLOW);
				Request tempReq(buffer, n);
				if (tempReq.Post() != "")
					if (!getCorrectServ(tempReq, fd[pos].fd, MAXCHECK))
						return Request();
			}
            char *bug = new char[n + 1];
            for (int o = 0; o < n; ++o)
                bug[o] = buffer[o];
            full_buf.push_back(std::make_pair(bug, n));
        }
        counter++;
        buf_size += n;
        if (n < 4096)
            break;
    }
    char buf[buf_size + 1];
    std::vector<std::pair<char *, int> >::iterator it;
    int f = 0;
    for (it = full_buf.begin(); it != full_buf.end(); ++it)
    {
        int i = -1;
        while (++i < it->second)
            buf[f++] = it->first[i];
        delete[] it->first;
    }
    buf[buf_size] = '\0';
    std::vector<std::pair<char *, int> >().swap(full_buf); // why
    findbuffer = std::string(buf, buf_size);
    Request req = postThings(findbuffer, buf, fd[pos].fd, buf_size);
    return req;
}

void	handleLostClient(std::vector<pollfd> &fd, int pos)
{
	printlog("CONNECTION CLOSED WITH CLIENT", fd[pos].fd - 2, RED);
	ClientServer(fd[pos].fd, 0, ERASECLIENT);
	close(fd[pos].fd);
	fd.erase(fd.begin() + pos);
}

Servers &Servers::operator=(Servers const &other)
{
	if (this != &other)
	{
		servs = other.servs;
		config = other.config;
		fds = other.fds;
	}
	return *this;
}

Servers::~Servers()
{
	std::vector<pollfd>::iterator it;
	for (it = fds.begin(); it != fds.end(); ++it)
	{
		close(it->fd);
	}
	fds.clear();
}
