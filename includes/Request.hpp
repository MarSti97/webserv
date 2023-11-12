#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"

class Content;
class Download;

class Request
{
    private:
        char        *c_request;
        std::string _request;
        std::string get;
        std::string del;
        std::string query;
        std::string post;
        std::string host;
		std::string port;
        std::string useragent;
        std::string accept;
        std::string acceptencoding;
        std::string acceptlanguage;
        std::string xrequestedwith;
        std::string connection;
        std::string referer;
        std::string upgradeinsecurerequests;
        std::string secfetchdest;
        std::string secfetchmode;
        std::string secfetchsite;
        std::string secfetchuser;
        std::string contenttype;
        std::string contentlength;
        std::string contentdisposition;
        std::string origin;
        std::string boundary;
        std::string expect;
        std::string transferencoding;
        int         clientfd;
        int         eof;
        bool        continue_100;
    
    public:
        Request();
        Request( char *buffer, size_t size );
        ~Request();

        Content content;

        void    clean_content( void );
        char    *C_request( void );
        int     EndBoundary( char *str, size_t len, char *boundary );
        int     ClientFd( void );
        void    SetClientFd( int fd );
        int     Eof( void );
        int     processChunked(int current_len, Download &down, int client);
        bool    getContinue100() const;

        std::string request( void ) const;
        std::string Del( void ) const;
        std::string Get( void ) const;
        std::string Query( void ) const;
        std::string Post( void ) const;
        std::string Host( void ) const;
        std::string Port( void ) const;
        std::string Useragent( void ) const;
        std::string Accept( void ) const;
        std::string Acceptencoding( void ) const;
        std::string Acceptlanguage( void ) const;
        std::string Xrequestedwith( void ) const;
        std::string Connection( void ) const;
        std::string Referer( void ) const;
        std::string Upgradeinsecurerequests( void ) const;
        std::string Secfetchdest( void ) const;
        std::string Secfetchmode( void ) const;
        std::string Secfetchsite( void ) const;
        std::string Secfetchuser( void ) const;
        std::string Contenttype( void ) const;
        std::string Contentlength( void ) const;
        std::string Contentdisposition( void ) const;
        std::string Origin( void ) const;
        std::string Boundary( void ) const;
        std::string TransferEncoding() const;
};

#endif