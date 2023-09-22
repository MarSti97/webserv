#include "includes/request.hpp"
#include <string>

Disposition::Disposition(std::string disposition)
{
    size_t typeStart = disposition.find("name=") + 6;
    size_t typeEnd = disposition.find("\"", typeStart);
    if (typeEnd == 0)
        typeEnd = disposition.find("\r", typeStart);

    size_t nameStart = disposition.find("filename=") + 10;
    size_t nameEnd = disposition.find("\"", nameStart);\
    if (nameEnd == 0)
        nameEnd = disposition.find("\r", nameStart);

    size_t contentEnd = disposition.find(";", 0);

    type = disposition.substr(typeStart, typeEnd - typeStart);
    filename = disposition.substr(nameStart, nameEnd - nameStart);
    contentdisposition = disposition.substr(0, contentEnd);
}

std::string Disposition::getName( void ) const
{
    return type;
}

std::string Disposition::getFilename( void ) const
{
    return filename;
}

std::string Disposition::getcontentdisposition( void ) const
{
    return contentdisposition;
}

Content::Content(std::string Content, std::string boundary)
{
    size_t dispositionStart = Content.find("Content-Disposition: ") + 20;
    size_t dispositionEnd = Content.find("\r", dispositionStart);

    size_t typeStart = Content.find("Content-Type: ") + 13;
    size_t typeEnd = Content.find("\r", typeStart + 1);

    size_t fileStart = Content.find("\r\n\r\n") + 4;
    size_t fileEnd = Content.find(boundary, fileStart);

    filename(Content.substr(dispositionStart, dispositionEnd - dispositionStart));
    content_type = Content.substr(typeStart, typeEnd - typeStart);
    content = Content.substr(fileStart, fileEnd - fileStart);
}

std::string Content::getContent( void ) const
{
    return content;
}

std::string Content::getContentType( void ) const
{
    return contenttype;
}

std::string Request::request( void ) const
{
    return request;
}

std::string Request::get( void ) const
{
    return get;
}

std::string Request::post( void ) const
{
    return post;
}

std::string Request::host( void ) const
{
    return host;
}

std::string Request::useragent( void ) const
{
    return useragent;
}

std::string Request::accept( void ) const
{
    return accept;
}

std::string Request::acceptencoding( void ) const
{
    return acceptencoding;
}

std::string Request::acceptlanguage( void ) const
{
    return acceptlanguage;
}

std::string Request::xrequestedwith( void ) const
{
    return xrequestedwith;
}

std::string Request::connection( void ) const
{
    return connection;
}

std::string Request::referer( void ) const
{
    return referer;
}

std::string Request::upgradeinsecurerequests( void ) const
{
    return upgradeinsecurerequests;
}

std::string Request::secfetchdest( void ) const
{
    return secfetchdest;
}

std::string Request::secfetchmode( void ) const
{
    return secfetchmode;
}

std::string Request::secfetchsite( void ) const
{
    return secfetchsite;
}

std::string Request::secfetchuser( void ) const
{
    return secfetchuser;
}

std::string Request::contenttype( void ) const
{
    return contenttype;
}

std::string Request::contentlength( void ) const
{
    return contentlength;
}

std::string Request::contentdisposition( void ) const
{
    return contentdisposition;
}

std::string Request::origin( void ) const
{
    return origin;
}

std::string Request::boundary( void ) const
{
    return boundary;
}

std::string Request::EOF( void ) const
{
    return EOF;
}

Request::Request( char *buffer )
{
    request = request(buffer);

    size_t getStart = request.find("GET ") + 4;
    size_t getEnd = request.find(" ", getStart);
    size_t getEnd2 = buf.find("?", getStart);
	if (end2 < end)
		end = end2;
    get = request.substr(getStart, getEnd - getStart);

    if (get.empty())
    {
        //post here.
    }
    post;
    host;
    useragent;
    accept;
    acceptencoding;
    acceptlanguage;
    xrequestedwith;
    connection;
    referer;
    upgradeinsecurerequests;
    secfetchdest;
    secfetchmode;
    secfetchsite;
    secfetchuser;
    contenttype;
    contentlength;
    contentdisposition;
    origin;
    boundary;
    EOF;
}

#endif

//#textmate (mate);
//snippets