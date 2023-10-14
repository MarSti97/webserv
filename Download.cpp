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
        // delete[] it->second.file;
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
        // it->second.file = strjoin(it->second.file, buf, it->second.current_len, bufsize);
        int totalLength = it->second.current_len + bufsize;
        char combinedStr[totalLength + 1]; // +1 for the null-terminator
        combinedStr[totalLength] = '\0'; // +1 for the null-terminator
        memcpy(combinedStr, it->second.file, it->second.current_len);
        memcpy(combinedStr + it->second.current_len, buf, bufsize);
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
    // int len = strlen(file);
    // for (int i = 0; i < size; ++i)
    int h = -1;
    while (file[++h])
        write(1, &file[h], 1);

    // std::cout << "THIS len: " << len << " OTHER ln : " << size << std::endl;
    // std::string str((std::string)file, size);
    // size_t i = str.find("\r\n\r\n");
    char find[4] = {'\r','\n','\r','\n'};
    for (int f = 0; f < size; ++f)
    {
        if (file[f] && file[f] == '\r')
            for (int i = 0; file[f + i] && i < 4; ++i)
            {
                if (file[f + i] && file[f + i] == find[i])
                    if (i == 3)
                    {
                        // std::cout << "WORK" << std::endl;
                        return f + i + 1;              
                    }
            }
    }
    // std::cout << i << std::endl;
    // if (i != std::string::npos)
    //     return i + 4;
    return 0;
}


Request Download::isitFULL(int client, char *file, size_t filesize)
{
    std::map<int, imgDown>::iterator it = fileMap.find(client);
    // std::cout << filesize << " " << it->second.current_len << " " << it->second.content_len << " ";
    if (it != fileMap.end())
    {
        // for (size_t f = 0; f < filesize; ++f)
        //     write (1, &it->second.file[f], 1);
        std::cout << " cunt " << std::endl;
        if (it->second.content_len <= it->second.current_len)
        {
            Request reo(it->second.file, it->second.current_len);
            if (!(reo.Boundary().empty()))
            {
                int headless = removehead(reo.C_request());
                Request req(reo.C_request(), it->second.current_len); // TAKE STRDUP
                // std::cout << "HERE" << std::endl;
                size_t size = removeFinalBoundary(reo.C_request() + headless, it->second.content_len, req);
                reo.content.setContent(reo.C_request() + headless, size);
                // it->second.img = new char[size + 1];
                // memcpy(it->second.img, reo.C_request() + headless, size + 1);
                // std::cout << " FUCK THAT SHIIIIIIIT " << size << " " << headless << " " << it->second.content_len << std::endl;
                // std::ofstream outfile("dickhead.jpg", std::ios::binary | std::ios::trunc);
                // if (outfile.is_open())
                // {
                //     outfile.write(it->second.img, size);
                //     outfile.close();
                // }
                // delete it->second.file;
                printlog("Successfully downloaded file", 0, GREEN);
                // reo.content.setContent(it->second.img, size);
                reo.content.setContentSize(size);
                eraseClient(client);
                return reo;
            }
            size_t size = removeheadnoimg(it->second.file, it->second.current_len);
            // std::cout << size << std::endl;
            // it->second.img = new char[it->second.current_len - size];
            // it->second.img[it->second.current_len - size + 1] = '\0';
            // memcpy(it->second.img, reo.C_request() + size, it->second.current_len - size);
            reo.content.setContent(reo.C_request() + size, strlen(reo.C_request() + size));
            // std::cout << " ass "<< reo.Contentlength() << strlen(reo.C_request() + size) << "fucking marcelo" << std::endl;
            reo.content.setContentSize(it->second.current_len - size);
            // std::cout << " fuck you?" << std::endl;
            eraseClient(client);
            return reo;
        }
        return Request();
    }
    Request rek(file, filesize);
    return rek;
}

// char *strjoin(char *str1, char *str2, int sizestr1, int sizestr2)
// {
//     int totalLength = sizestr1 + sizestr2;

//     char combinedStr[totalLength + 1]; // +1 for the null-terminator
//     combinedStr[totalLength] = '\0'; // +1 for the null-terminator

//     memcpy(combinedStr, str1, sizestr1);

//     memcpy(combinedStr + sizestr1, str2, sizestr2);

//     // delete str1; // fix good

//     return combinedStr;
// }
