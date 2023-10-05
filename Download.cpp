#include "includes/Download.hpp"
#include "includes/webserv.hpp"

Download* Download::instance = NULL;


Download &Download::getInstance()
{
    if (!instance)
        instance = new Download();
    return *instance;
}

void    Download::clean()
{
    // fix and make do delete one selected sockets too
    delete instance;
}

void Download::add_map(int client, imgDown content)
{
    fileMap.insert(std::make_pair(client, content));
}

void Download::append_map(int client, char *buf, int bufsize)
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    if (it != fileMap.end())
    {
        it->second.file = strjoin(it->second.file, buf, it->second.current_len, bufsize);
        it->second.current_len += bufsize;
        // std::cout << "Full-len: " << it->second.content_len << " | Current-len: " << it->second.current_len << std::endl;
    }
}

int removehead(char *file)
{
    std::string str(file);
    size_t i = str.find("\r\n\r\n");
    std::cout << i << std::endl;
    if (i != std::string::npos)
    {
        size_t j = str.find("\r\n\r\n", i + 1);
        std::cout << j << std::endl;
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
        // std::cout << "FOUND IT" << std::endl;
        return true;
    }
    // std::cout << "DIDNT WHY?" << std::endl;file o
    return false;
}

size_t  Download::removeFinalBoundary( char *str, size_t len, Request req )
{
    size_t i = -1;
    while (++i <= len)
    {
        if (str[i] == '\r') {
            if (compare(("\r\n--" + req.Boundary()).c_str(), str + i))
                break ;
        }
    }
    return i;
}

void Download::isitFULL(int client)
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    if (it != fileMap.end())
    {
        if (it->second.content_len <= it->second.current_len)
        {
            // std::cout << "YES: ITS TRUE FUCKING FINALLY" << std::endl;
            int headless = removehead(it->second.file);
            Request req(std::string(it->second.file));
            // int fd = open("dickhead.jpg", O_CREAT | O_RDWR | O_TRUNC);
            // char * startptr = it->second.file + headless;
            // std::cout << it->second.content_len << " " << i << std::endl;
            // write(fd, &startptr, i);
            // close(fd);
            size_t size = removeFinalBoundary(it->second.file + headless, it->second.content_len, req);
            it->second.img = new char[size];
            memcpy(it->second.img, it->second.file + headless, size);
            // std::ofstream outfile("dickhead.jpg", std::ios::binary | std::ios::trunc);
            // if (outfile.is_open())
            // {
            //     outfile.write(it->second.img, size);
            //     outfile.close();
            // }
            // delete it->second.file;
            // printlog("Successfully downloaded file", 0, GREEN);
        }
    }
}

char *strjoin(char *str1, char *str2, int sizestr1, int sizestr2)
{
    int totalLength = sizestr1 + sizestr2;

    char* combinedStr = new char[totalLength]; // +1 for the null-terminator

    memcpy(combinedStr, str1, sizestr1);

    memcpy(combinedStr + sizestr1, str2, sizestr2);

    // delete str1; // fix good

    return combinedStr;
}
