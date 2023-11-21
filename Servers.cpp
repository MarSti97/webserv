#include "./includes/webserv.hpp"

Servers::Servers(std::string file) : config(readFile(file))
{}

void Servers::init()
{
	printlog("Initializing server", -1, YELLOW);
    validate_config();
    std::vector<Serv>::iterator it;
	int i = 0;
    for (it = servs.begin(); it != servs.end(); ++it)
	{
        it->establish_connection(); // FIX:: should program exit if one fails or continue with non failed ones
		fds.push_back(pollfd());
		fds[i].fd = it->getSocket();
		fds[i].events = POLLIN;
		i++;
		printlog("Server: " + it->getServerHostPort() + " connection established", -1, YELLOW);
	}
}

void Servers::run()
{
	printlog("Running webserv... ", -1, PURPLE);
	struct sockaddr_in clientinfo;
    socklen_t size = sizeof(clientinfo);
	int timeout = 0;

	while (!end_loop(0))
    {
        // bzero(buffer, sizeof(buffer));
        // std::cout << fds.size() << std::endl;
        for (size_t i = 0; i < fds.size(); ++i)
        {
			timeout = 0;
			if (i >= servs.size())
				timeout = 10000;
			int ret = poll(&fds[i], 1, timeout);
			if (ret == -1)
			{
				if (errno == EINTR)
					printerr("Poll was interupted by a signal", 0, RED);
				else
					printerr("Error in poll", 0, RED);
				continue;
			}
			if (ret == 0 && !checkSockets(fds[i].fd))
			{
				printlog("TIMEOUT CLIENT", fds[i].fd - 2, RED);
				// close(fds[i].fd);
        		// fds.erase(fds.begin() + i);
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
						printlog("NEW REQUEST FROM CLIENT", fds[i].fd - 2, YELLOW);

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
            if (n <= 0)
            {
                if (!counter) { // Connection closed by the client
                    handleLostClient(fd, pos);
					if (n == -1)
						break;
                    return Request();
                }
                break;
            }
			// Handle other receive errors
			printerr("Error: reading from poll", 0, RED);
			perror("read");
			return Request();
        }
        else
        {
			if (counter == 0)
			{
				Request tempReq(buffer, n);
				//printlog("REQUEST: " + getFirstLine(tempReq.request()) + " FROM CLIENT", fds[pos].fd - 2, CYAN);
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

void	Servers::handleLostClient(std::vector<pollfd> &fd, int pos)
{
	printlog("LOST CLIENT", fd[pos].fd - 2, RED);
	ClientServer(fd[pos].fd, 0, ERASECLIENT);
	close(fd[pos].fd);
	fd.erase(fd.begin() + pos);
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
