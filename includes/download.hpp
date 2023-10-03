#ifndef DOWNLOAD_HPP
#define DOWNLOAD_HPP

#include "webserv.hpp"

struct imgDown
{
    static std::string _filename;
    static std::string _filelocation;
    static std::string _referer;
    static std::string _boundary;
    static char *_file;
    static int _reqSize;
    static int _clientnbr;
    static int _eof;
};


class Download
{
    private:
        static std::string _filename;
        static std::string _filelocation;
        static std::string _referer;
        static std::string _boundary;
        static char *_file;
        static int _reqSize;
        static int _clientnbr;
        static int _eof;
        Download();
        Download( const Download &other );
        Download &operator=( const Download &other );
        static std::map<int, int> fileSize;
        static std::map<int, char*> fileMap;
        static Download *instance;

    public:
        static Download &getInstance();
        int clientPostCheck(int client);
        void    addClient(int client, char *buf);
        void    completeFile(int client, char *buf);

        std::string &filename() const;
        std::string &filelocation() const;
        std::string &referer() const;
        std::string &boundary() const;
        char    *file() const;
        int     clientnbr() const;
        int     eof() const;
        void    Setfile(char *buf);
        void    Setfilename(std::string newfilename);
        void    Setfilelocation(std::string newfilelocation);
        void    Setreferer(std::string newreferer);
        void    Setboundary(std::string newboundary);
        void    Setclientnbr(int newnbr);
        void    Seteof(int neweof);
        void    clean();
};

#endif