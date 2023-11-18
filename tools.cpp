#include "./includes/webserv.hpp"

int removehead(char *file)
{
    std::string str(file);
    size_t i = str.find("\r\n\r\n");
    if (i != std::string::npos)
    {
        size_t j = str.find("\r\n\r\n", i + 1);
        if (j != std::string::npos) {
            return j + 4;
        }
    }
    return 0;
}

bool compare(const char *find, char* str)
{
    if (!strncmp(find, str, strlen(find)))
    {
        return true;
    }
    return false;
}

int removeheadnoimg(char *file, int size)
{
    char find[4] = {'\r','\n','\r','\n'};
    for (int f = 0; f < size; ++f)
    {
        if (file[f] && file[f] == '\r')
            for (int i = 0; file[f + i] && i < 4; ++i)
            {
                if (file[f + i] && file[f + i] == find[i])
                    if (i == 3)
                    {
                        return f + i + 1;              
                    }
            }
    }
    return 0;
}

std::string	getINFOone(std::string request, const char *what, int pos)
{
	size_t getStart = request.find(what);
    if (getStart != std::string::npos)
    {
		// std::cout << "CORRECT" << std::endl;
        getStart += pos;
        size_t getEnd = request.find(" ", getStart);
        size_t getEnd2 = request.find("?", getStart);
        if (getEnd2 < getEnd)
            getEnd = getEnd2;
        return (request.substr(getStart, getEnd - getStart));
    }
	return "";
}

std::string	getINFOtwo(std::string request, const char *what, int pos)
{
    size_t hostStart = request.find(what);
    if (hostStart != std::string::npos)
    {
        hostStart += pos;
        // std::cerr << "CORRECT" << std::endl;
        size_t hostEnd = request.find("\r", hostStart);
        return (request.substr(hostStart, hostEnd - hostStart));
    }
	return "";
}

std::string removeDashIfExists(std::string path)
{
    int size = path.length();
    if (path[size - 1] == '/' && size != 1)
        return path.substr(0, size - 1);
    return path;
}

bool deleteFolderRecusively(std::string path)
{
	struct dirent *entry;
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		printerr("Error: couldnt open dir for delete method: " + path, 0, RED);
	else
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            	continue;
			if (entry->d_type == DT_DIR)
				deleteFolderRecusively(path + "/" + entry->d_name); // does d_name have / already?
			else
			{
				std::string fileToDelete = path + "/" + entry->d_name;
				if (std::remove(fileToDelete.c_str()) != 0)
					printerr("Error: could not delete file within directory: " + fileToDelete, 0, RED);
			}
		}
		closedir(dir);
		if (rmdir(path.c_str()) != 0)
			return false;
		return true;
	}
	return false;
}

int getIntSize(int nbr)
{
    if (nbr == 0)
        return 1;
    int res = 0;
    while (nbr)
    {
        nbr /= 10;
        res++;
    }
    return res;
}

Methods whatstheMethod(Methods meth, std::string word)
{
	if (meth == ALLOWED)
	{
		printlog("Method " + word + " allowed.", -1, GREEN);
		return ALLOWED;
	}
	else if (meth == DENIED)
	{
		printlog("Method " + word + " not allowed.", -1, RED);
		return DENIED;
	}
    return UNDEFINED;
}

void	handleLostClient(std::vector<pollfd> &fd, int pos)
{
	printlog("LOST CLIENT", fd[pos].fd - 2, RED);
	ClientServer(fd[pos].fd, 0, ERASECLIENT);
	close(fd[pos].fd);
	fd.erase(fd.begin() + pos);
}

int ClientServer(int client, int server, ClientHandle locker) // 0 to add a new client, 1 for returning the socket that the client is connected to, 2 to erase the client from the map.
{
	static std::map<int, int> connect;

	if (client && server && (locker == NEWCLIENT))
	{
		if (connect.find(client) == connect.end())
		{
			connect.insert(std::make_pair(client, server));
			printlog("CLIENT MAPPED", client - 2, BLUE);
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
			printlog("CLIENT ERASED FROM MAP", client - 2, PURPLE);
		}
		else
			printerr("CLIENT ALREADY ERASED FROM MAP", client - 2, RED);
	}
	return 0;
}

std::string getFirstLine(std::string content)
{
	size_t len = content.find("\r\n");
	if (len != std::string::npos)
		return content.substr(0, len);
	return "No Header Found";
}