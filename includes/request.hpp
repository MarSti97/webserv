#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "webserv.hpp"

class Content
{
    class Disposition
    {
        private:
            std::string type;
            std::string filename;
            std::string contentdisposition;
            Disposition( void );

        public:
            Disposition(std::string disposition);
            std::string getType( void ) const;
            std::string getFilename( void ) const;
            std::string getcontentdisposition( void ) const;
    };

    private:
        std::string content_type;
        std::string content;
        Content( void );
    
    public:
        Content(std::string content, std::string boundary);
        Disposition filename(std::string file);
        std::string getContent( void ) const;
        std::string getContentType( void ) const;

};

class Request
{
    private:
        std::string request;
        std::string get;
        std::string post;
        std::string host;
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
        std::string EOF;
        Request();
    
    public:
        Request( char *buffer );
        Content content(std::string file);
        std::string Request( void ) const;
        std::string get( void ) const;
        std::string post( void ) const;
        std::string host( void ) const;
        std::string useragent( void ) const;
        std::string accept( void ) const;
        std::string acceptencoding( void ) const;
        std::string acceptlanguage( void ) const;
        std::string xrequestedwith( void ) const;
        std::string connection( void ) const;
        std::string referer( void ) const;
        std::string upgradeinsecurerequests( void ) const;
        std::string secfetchdest( void ) const;
        std::string secfetchmode( void ) const;
        std::string secfetchsite( void ) const;
        std::string secfetchuser( void ) const;
        std::string contenttype( void ) const;
        std::string contentlength( void ) const;
        std::string contentdisposition( void ) const;
        std::string origin( void ) const;
        std::string boundary( void ) const;
        std::string EOF( void ) const;
};

#endif