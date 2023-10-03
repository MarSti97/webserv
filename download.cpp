#include "includes/download.hpp"

int Download::_eof = 0;
int Download::_reqSize = 0;
int Download::_clientnbr = 0;
char *Download::_file = NULL;

Download &Download::getInstance()
{
    if (!instance)
        instance = new Download();
    return instance;
}

void    Download::clean()
{
    _filename.clear();
    _filelocation.clear();
    _referer.clear();
    _boundary.clear();
    if (_file)
        free (_file);
    _file = NULL;
    _reqSize = 0;
    _clientnbr = 0;
    _eof = 0;
    delete instance;
}

std::string &Download::filename() const
{
    return _filename;
}

std::string &Download::filelocation() const
{
    return _filelocation;
}

std::string &Download::referer() const
{
    return _referer;
}

std::string &Download::boundary() const
{
    return _boundary;
}

char *Download::file() const
{
    return _file;
}

int Download::clientnbr() const
{
    return _clientnbr;
}


int Download::eof() const
{
    return _eof;
}

void Download::Setfilename(std::string newfilename)
{
    _filename = newfilename;
}

void Download::Setfilelocation(std::string newfilelocation)
{
    _filelocation = newfilelocation;
}

void Download::Setreferer(std::string newreferer)
{
    _referer = newreferer;
}

void Download::Setboundary(std::string newboundary)
{
    _boundary = newboundary;
}

void Download::Setfile(char *buf)
{
    if (!_file)
        _file = buf;
    // else
    // {
    //     strjoin()
    // }
}

void Download::Seteof(int neweof)
{
    _eof = neweof;
}

void Download::Setclientnbr(int newnbr)
{
    _clientnbr = newnbr;
}

int Download::clientPostCheck(int client)
{
    std::map<int, const char*>::iterator it; = fileMap.find(client);
    if (it != fileMap.end())
        return 1;
    return 0;
}

void    Download::addClient(int client, char *buf, int bufsize)
{
    if (!clientPostCheck(client))
    {
        fileMap.insert(std::make_pair(client, buf));
        fileSize.insert(std::make_pair(client, bufsize));
    }
}

void    Download::completeFile(int client, char *buf, int bufsize)
{
    char *newbuf = new char[fileSize[client] + bufsize];
    newbuf = strjoin(fileMap[client], buf, fileSize[client], bufsize);
    delete fileMap[client];

}

char *strjoin(char *str1, char *str2, int sizestr1, int sizestr2)
{
    int totalLength = strlen(str1) + strlen(str2);

    char* combinedStr = new char[totalLength + 1]; // +1 for the null-terminator

    memcpy(combinedStr, str1, sizestr1);

    memcpy(combinedStr + sizestr1, str2, sizestr2);

    return combinedStr;
}
