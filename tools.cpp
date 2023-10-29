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
	DIR *dir = opendir(path.c_str() + 1);
	if (dir == NULL)
		std::cout << "Error: couldnt open dir for delete method: " << path << std::endl;
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
				if (std::remove(fileToDelete.c_str() + 1) != 0)
					std::cout << "Error: could not delete file within directory: " << fileToDelete << std::endl;
			}
		}
		closedir(dir);
		if (rmdir(path.c_str() + 1) != 0)
			return false;
		return true;
	}
	return false;
}

// char *joinVector(std::vector<std::pair<char *, int> > full, int size)
// {
//     char buf[size + 1];
//     std::vector<std::pair<char *, int> >::iterator it;
//     int f = 0;
//     for (it = full.begin(); it != full.end(); ++it)
//     {
//         int i = -1;
//         while (++i < it->second)
//             buf[f++] = it->first[i];
//         delete[] it->first;
//     }
//     buf[size] = '\0';
//     return buf;
// }