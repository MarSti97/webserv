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
        delete[] it->second.file;
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
        // it->second.file = strjoin(it->second.file, buf, it->second.current_len, bufsize);
        // std::cout << "WTF" << std::endl;
        int totalLength = it->second.current_len + bufsize;
        char *combinedStr = new char[totalLength + 1]; // +1 for the null-terminator
        combinedStr[totalLength] = '\0'; // +1 for the null-terminator
        memcpy(combinedStr, it->second.file, it->second.current_len);
        memcpy(combinedStr + it->second.current_len, buf, bufsize);
        delete[] it->second.file;
        it->second.file = combinedStr;
        it->second.current_len += bufsize;
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

Request Download::isitFULL(int client, char *file, size_t filesize)
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    if (it != fileMap.end())
    {
        if (it->second.content_len <= it->second.current_len)
        {
            Request reo(it->second.file, it->second.current_len);
            int to_do = reo.processChunked(it->second.current_len, *this, client);
            if(to_do == 0)
                return reo;
            else if (to_do == 2)
                return Request();
            // std::cout << "DEBUG: isitFull " << reo.Boundary() << std::endl;
            if (!(reo.Boundary().empty()))
            {
                int headless = removehead(reo.C_request());
                Request req(reo.C_request(), it->second.current_len); // TAKE STRDUP
                size_t size = removeFinalBoundary(reo.C_request() + headless, it->second.content_len, req);
                reo.content.setContent(reo.C_request() + headless, size);
                printlog("Successfully downloaded file", -1, GREEN);
                reo.content.setContentSize(size);
                eraseClient(client);
                return reo;
            }
            size_t size = removeheadnoimg(it->second.file, it->second.current_len);
            reo.content.setContent(reo.C_request() + size, strlen(reo.C_request() + size));
            reo.content.setContentSize(it->second.current_len - size);
            eraseClient(client);
            return reo;
        }
        return Request();
    }
    Request rek(file, filesize);
    return rek;
}
