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
        std::cout << "Full-len: " << it->second.content_len << " | Current-len: " << it->second.current_len << std::endl;
        // if (it->second.content_len <= it->second.current_len)
        // {
        //     it->second.eof = true;
        //     std::cout << "YES: ITS TRUE FUCKING FINALLY" << std::endl;
        //     int i = -1;
        //     while (++i < it->second.content_len)
        //         write(1, &it->second.file[i], sizeof(it->second.file[i]));
        //     // file downoad create that shit!
        // }
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
            std::cout << "IS NEW LINE? " << std::string(file + j + i) << std::endl;
        }
        return j + 4;
    }
    return 0;
}

void Download::isitFULL(int client)
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    if (it != fileMap.end())
    {
        if (it->second.content_len <= it->second.current_len)
        {
            std::cout << "YES: ITS TRUE FUCKING FINALLY" << std::endl;
            int headless = removehead(it->second.file);
            Request req(std::string(it->second.file));
            // int newfd = open(, O_CREAT | O_TRUNC);
            std::ofstream outfile("dickhead.jpg", std::ios::binary);
            if (outfile.is_open())
            {
                outfile.write(it->second.file + headless, it->second.content_len);
                outfile.close();
                std::cout << "SOMETHINGA: " << req.content.filename.getFilename() << std::endl;
            }
            // std::cout << "SOMETHINGA MORE: " << req.Boundary() << std::endl;
            // int i = -1;
            // while (++i < it->second.content_len)
            //     write(newfd, &headless[i], sizeof(headless[i]));
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
