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