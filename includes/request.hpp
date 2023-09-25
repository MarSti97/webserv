#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "webserv.hpp"

class Disposition
{
    private:
        std::string type;
        std::string filename;
        std::string contentdisposition;

    public:
        Disposition( void );
        Disposition(const Disposition &other);
        Disposition(std::string disposition);
        Disposition &operator=(const Disposition &other);
        std::string getType( void ) const;
        std::string getFilename( void ) const;
        std::string getcontentdisposition( void ) const;
        ~Disposition();
};

class Content
{

    private:
        std::string content_type;
        std::string _content;
    
    public:
        Content( void );
        Content(const Content &other);
        Content(std::string content, std::string boundary);
        Content &operator=(const Content &other);
        Disposition filename;
        std::string getContent( void ) const;
        std::string getContentType( void ) const;
        ~Content();
};

class Request
{
    private:
        std::string _request;
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
        int         eof;
    
    public:
        Request();
        Request( char *buffer );
        Request( const Request &other );
        Request &operator=( const Request &other);
        Content content;
        std::string request( void ) const;
        std::string Get( void ) const;
        std::string Post( void ) const;
        std::string Host( void ) const;
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
        int Eof( void );
        ~Request();
};

#endif