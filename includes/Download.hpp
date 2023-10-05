#ifndef DOWNLOAD_HPP
#define DOWNLOAD_HPP

#include "webserv.hpp"

struct imgDown
{
    int content_len;
    int current_len;
    char *file;
    std::string boundary;
    bool eof;
    // maybe find file type!

    imgDown(int full_len, int size, char *str, std::string _boundary) {
        current_len = size;
        content_len = full_len;
        file = str;
        boundary = _boundary;
    }
};


class Download
{
    private: 
        Download() {}
        Download( const Download &other );
        Download &operator=( const Download &other );
        ~Download() {}
        std::map<int, imgDown> fileMap;
        static Download *instance;

    public:
        static Download &getInstance();
        // void    map_manage(int client, char *buf, int bufsize);
        void    add_map(int client, imgDown content);
        void    append_map(int client, char *buf, int bufsize);
        void    isitFULL(int client);


        // std::string &filename() const;
        // std::string &filelocation() const;
        // std::string &referer() const;
        // std::string &boundary() const;
        // char    *file() const;
        // int     clientnbr() const;
        // int     eof() const;
        // void    Setfile(char *buf);
        // void    Setfilename(std::string newfilename);
        // void    Setfilelocation(std::string newfilelocation);
        // void    Setreferer(std::string newreferer);
        // void    Setboundary(std::string newboundary);
        // void    Setclientnbr(int newnbr);
        // void    Seteof(int neweof);
        void    clean();
};


#endif