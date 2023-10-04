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
    std::cout << "YES: ITS TRUE" << std::endl;
    if (it != fileMap.end())
    {
        it->second.file = strjoin(it->second.file, buf, it->second.current_len, bufsize);
        it->second.current_len += bufsize;
        std::cout << "Full-len: " << it->second.content_len << " | Current-len: " << it->second.current_len << std::endl;
        if (it->second.content_len == it->second.current_len)
        {
            it->second.eof = true;
            // file downoad create that shit!
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
