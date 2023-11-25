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
		return ALLOWED;
	printlog("METHOD " + word + " NOT ALLOWED", -1, RED);
    return DENIED;
}


std::string getFirstLine(std::string content)
{
	size_t len = content.find("\r\n");
	if (len != std::string::npos)
		return content.substr(0, len);
	return "No Header Found";
}

void	printerr(std::string msg, int arg, std::string color)
{
    if (arg == -1)
	    std::cerr << color << makeStamp() << " " << msg << " " << NOCOLOR << std::endl; 
    else if (arg != -2)
	    std::cerr << color << makeStamp() << " " << msg << " " << arg << NOCOLOR << std::endl;
    else
        std::cerr << color << msg << NOCOLOR << std::endl;
}

void	printlog(std::string msg, int arg, std::string color)
{
    if (arg == -1)
	    std::cout << color << makeStamp() << " " << msg << " " << NOCOLOR << std::endl; 
    else
	    std::cout << color << makeStamp() << " " << msg << " " << arg << NOCOLOR << std::endl; 
}

std::string makeStamp( void )
{
    time_t currentTime;
    struct tm *localTimeInfo;

    time(&currentTime);

    localTimeInfo = localtime(&currentTime);
    std::stringstream hours, minutes, day, month, year;
    std::string lengthStr = hours.str();
    hours << localTimeInfo->tm_hour;
    minutes << localTimeInfo->tm_min;
    day << localTimeInfo->tm_mday;
    month << localTimeInfo->tm_mon + 1;
    year << localTimeInfo->tm_year + 1900;

    return hours.str() + ':' + minutes.str() + '/' + day.str() + '-' + month.str() + '-' + year.str();
}
