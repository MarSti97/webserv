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
    fileMap.clear();
    if (instance)
        delete instance;
}

void    Download::eraseClient( int client )
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    if (it != fileMap.end())
    {
        // delete it->second.file;
        // delete it->second.img;
        fileMap.erase(client);
    }
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

imgDown Download::getStruct(int client)
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    if (it != fileMap.end())
        return it->second;
    imgDown ass(0,0,0, "");
    return ass;
}

int removehead(char *file)
{
    std::string str(file);
    size_t i = str.find("\r\n\r\n");
    // std::cout << i << std::endl;
    if (i != std::string::npos)
    {
        size_t j = str.find("\r\n\r\n", i + 1);
        // std::cout << j << std::endl;
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

int removeheadnoimg(char *file, int size)
{
    std::string str(file, size);
    size_t i = str.find("\r\n\r\n");
    // std::cout << i << std::endl;
    if (i != std::string::npos)
        return i + 4;
    return 0;
}


Request Download::isitFULL(int client, char *file, size_t filesize)
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    if (it != fileMap.end())
    {
        // for (size_t f = 0; f < filesize; ++f)
        //     write (1, &it->second.file[f], 1);
        // std::cout << filesize << " " << it->second.current_len << " " << it->second.content_len << " ";
        if (it->second.content_len <= it->second.current_len)
        {
            Request reo(it->second.file, it->second.current_len);
            if (!(reo.Boundary().empty()))
            {
                int headless = removehead(reo.C_request());
                Request req(strdup(reo.C_request()), it->second.current_len);
                std::cout << "HERE" << std::endl;
                size_t size = removeFinalBoundary(reo.C_request() + headless, it->second.content_len, req);
                it->second.img = new char[size];
                memcpy(it->second.img, reo.C_request() + headless, size);
                // std::cout << " FUCK THAT SHIIIIIIIT " << size << " " << headless << " " << it->second.content_len << std::endl;
                // std::ofstream outfile("dickhead.jpg", std::ios::binary | std::ios::trunc);
                // if (outfile.is_open())
                // {
                //     outfile.write(it->second.img, size);
                //     outfile.close();
                // }
                // delete it->second.file;
                printlog("Successfully downloaded file", 0, GREEN);
                reo.content.setContent(it->second.img);
                reo.content.setContentSize(size);
                eraseClient(client);
                return reo;
            }
            size_t size = removeheadnoimg(reo.C_request(), it->second.current_len);
            // std::cout << size << std::endl;
            it->second.img = new char[it->second.current_len - size];
            memcpy(it->second.img, reo.C_request() + size, it->second.current_len - size);
            reo.content.setContent(it->second.img);
            reo.content.setContentSize(it->second.current_len - size);
            eraseClient(client);
            return reo;
        }
        return Request();
    }
    Request rek(file, filesize);
    return rek;
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
