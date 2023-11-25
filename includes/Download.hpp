#ifndef DOWNLOAD_HPP
#define DOWNLOAD_HPP

#include "webserv.hpp"

struct imgDown
{
    int     content_len;
    int     current_len;
    char    *file;

    std::string boundary;
    bool    eof;

    imgDown() : content_len(0), current_len(0), file(NULL), eof(false) {}
    imgDown(int full_len, int size, char *str, std::string _boundary) {
        current_len = size;
        content_len = full_len;
        file = new char[size];
        for (int i = 0; i < size; i++)
            file[i] = str[i];
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
        void    eraseClient( int client );
        void    clean();
        void    add_map(int client, imgDown content);
        void    append_map(int client, char *buf, int bufsize);
        Request isitFULL(int client, char *file, size_t filesize);
        size_t  removeFinalBoundary( char *str, size_t len, Request req );
        imgDown getStruct(int client);
};


#endif