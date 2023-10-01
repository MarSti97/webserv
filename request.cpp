#include "includes/request.hpp"
#include <string>

Disposition::Disposition(std::string disposition)
{
    if (!disposition.empty())
    {
        // std::cerr << "HERE2" << std::endl;

        size_t typeStart = disposition.find("name=");
        size_t typeEnd = disposition.find("\"", typeStart);
        if (typeEnd != 0)
            typeEnd = disposition.find("\r", typeStart);
        if (typeStart != std::string::npos)
        {
            typeStart += 6;
            type = disposition.substr(typeStart, typeEnd - typeStart);
        }

        size_t nameStart = disposition.find("filename=");
        size_t nameEnd = disposition.find("\"", nameStart);\
        if (nameEnd != 0)
            nameEnd = disposition.find("\r", nameStart);
        if (nameStart != std::string::npos)
        {
            nameStart += 10;
            filename = disposition.substr(nameStart, nameEnd - nameStart);
        }

        size_t contentEnd = disposition.find(";", 0);
        if (contentEnd != std::string::npos)
            contentdisposition = disposition.substr(0, contentEnd);
    }
}

std::string Disposition::getType( void ) const
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

Content::Content(std::string content, std::string boundary)
{
    if (!content.empty() && !boundary.empty())
    {
        // std::cerr << "HERE1" << std::endl;

        size_t dispositionStart = content.find("Content-Disposition: ");
        size_t dispositionEnd = content.find("\r", dispositionStart);
        if (dispositionStart != std::string::npos)
        {
            dispositionStart += 20;
            filename = Disposition(content.substr(dispositionStart, dispositionEnd - dispositionStart));
        }

        size_t typeStart = content.find("Content-Type: ");
        size_t typeEnd = content.find("\r", typeStart + 1);
        if (typeStart != std::string::npos)
        {
            typeStart += 13;
            content_type = content.substr(typeStart, typeEnd - typeStart);
        }

        size_t fileStart = content.find("\r\n\r\n");
        size_t fileEnd = content.find(boundary, fileStart);
        if (fileStart != std::string::npos)
        {
            fileStart += 4;
            _content = content.substr(fileStart, fileEnd - fileStart);
        }

    }
}

std::string Content::getContent( void ) const
{
    return _content;
}

std::string Content::getContentType( void ) const
{
    return content_type;
}

std::string Request::request( void ) const
{
    return _request;
}

std::string Request::Get( void ) const
{
    return get;
}

std::string Request::Query( void ) const
{
    return query;
}

std::string Request::Post( void ) const
{
    return post;
}

std::string Request::Host( void ) const
{
    return host;
}

std::string Request::Port( void ) const
{
    return port;
}

std::string Request::Useragent( void ) const
{
    return useragent;
}

std::string Request::Accept( void ) const
{
    return accept;
}

std::string Request::Acceptencoding( void ) const
{
    return acceptencoding;
}

std::string Request::Acceptlanguage( void ) const
{
    return acceptlanguage;
}

std::string Request::Xrequestedwith( void ) const
{
    return xrequestedwith;
}

std::string Request::Connection( void ) const
{
    return connection;
}

std::string Request::Referer( void ) const
{
    return referer;
}

std::string Request::Upgradeinsecurerequests( void ) const
{
    return upgradeinsecurerequests;
}

std::string Request::Secfetchdest( void ) const
{
    return secfetchdest;
}

std::string Request::Secfetchmode( void ) const
{
    return secfetchmode;
}

std::string Request::Secfetchsite( void ) const
{
    return secfetchsite;
}

std::string Request::Secfetchuser( void ) const
{
    return secfetchuser;
}

std::string Request::Contenttype( void ) const
{
    return contenttype;
}

std::string Request::Contentlength( void ) const
{
    return contentlength;
}

std::string Request::Contentdisposition( void ) const
{
    return contentdisposition;
}

std::string Request::Origin( void ) const
{
    return origin;
}

std::string Request::Boundary( void ) const
{
    return boundary;
}

int Request::Eof( void )
{
    return eof;
}

std::string	getINFOone(std::string request, const char *what, int pos)
{
	size_t getStart = request.find(what);
    if (getStart != std::string::npos)
    {
		// std::cout << "CORRECT" << std::endl;
        getStart += pos;
        size_t getEnd = request.find(" ", getStart);
        size_t getEnd2 = request.find("?", getStart);
        if (getEnd2 < getEnd)
            getEnd = getEnd2;
        return (request.substr(getStart, getEnd - getStart));
    }
	return "";
}

std::string	getINFOtwo(std::string request, const char *what, int pos)
{
    size_t hostStart = request.find(what);
    if (hostStart != std::string::npos)
    {
        hostStart += pos;
        // std::cerr << "CORRECT" << std::endl;
        size_t hostEnd = request.find("\r", hostStart);
        return (request.substr(hostStart, hostEnd - hostStart));
    }
	return "";
}

Request::Request( std::string buffer )
{
    this->_request = buffer;
	this->get = getINFOone(this->_request, "GET ", 4);
    size_t queryStart = _request.find("GET ");
    if (queryStart != std::string::npos)
    {
        queryStart += 4;
		// std::cout << "CORRECT" << std::endl;
        size_t queryEnd = _request.find(" ", queryStart);
        size_t queryEnd2 = _request.find("?", queryStart);
        if (queryEnd2 < queryEnd)
            this->query = _request.substr(queryEnd2, queryEnd - queryEnd2);
    }
	this->post = getINFOone(this->_request, "POST ", 5);

	this->host = getINFOtwo(this->_request, "Host: ", 6);
	// std::cout << "THIS: " << this->host << std::endl;
	size_t splitter = host.find(":");
	if (splitter != std::string::npos)
	{
		this->port = this->host.substr(splitter + 1);
		std::string temp = this->host.substr(0, splitter);
		this->host = temp;
		// std::cout << "THAT: " << this->host << " and " << this->port << std::endl;
	}

	this->useragent = getINFOtwo(this->_request, "User-Agent: ", 12);
	this->accept = getINFOtwo(this->_request, "Accept: ", 8);
	this->acceptlanguage = getINFOtwo(this->_request, "Accept-Language: ", 17);
	this->acceptencoding = getINFOtwo(this->_request, "Accept-Encoding: ", 17);
	this->xrequestedwith = getINFOtwo(this->_request, "X-Requested-With: ", 18);
	this->connection = getINFOtwo(this->_request, "Connection: ", 12);

    size_t refererStart = _request.find("Referer: ");
    if (refererStart != std::string::npos)
    {
        refererStart += 9;
        // std::cerr << "10" << std::endl;
        size_t refererend = _request.find(":", refererStart);
        size_t refererend2 = _request.find(":", refererend + 1);
        size_t refererend3 = _request.find("/", refererend2);
        size_t refererend4 = _request.find("\r", refererend3);
        this->referer = _request.substr(refererend3, refererend4 - refererend3);
    }

	this->upgradeinsecurerequests = getINFOtwo(this->_request, "Upgrade-Insecure-Requests: ", 30);
	this->secfetchuser = getINFOtwo(this->_request, "Sec-Fetch-User: ", 16);
	this->secfetchdest = getINFOtwo(this->_request, "Sec-Fetch-Dest: ", 16);
	this->secfetchmode = getINFOtwo(this->_request, "Sec-Fetch-Mode: ", 16);
	this->secfetchsite = getINFOtwo(this->_request, "Sec-Fetch-Site: ", 16);

    size_t cont_typeStart = _request.find("Content-Type: ") ;
    if (cont_typeStart != std::string::npos)
    {
        cont_typeStart += 14;
        // std::cerr << "16" << std::endl;
        size_t cont_typeEnd = _request.find(";", cont_typeStart);
        size_t cont_typeEnd2 = _request.find("\r", cont_typeStart);
        if (cont_typeEnd2 < cont_typeEnd)
            cont_typeEnd = cont_typeEnd2;
        this->contenttype = _request.substr(cont_typeStart, cont_typeEnd - cont_typeStart);
    }

	this->boundary = getINFOtwo(this->_request, "boundary=", 9);
	this->contentlength = getINFOtwo(this->_request, "Content-Length: ", 16);
	this->origin = getINFOtwo(this->_request, "Origin: ", 8);

    size_t dispositionStart = _request.find("\r\n" + boundary);
    if (dispositionStart != std::string::npos)
    {
        dispositionStart += boundary.size() + 2;
        size_t dispositionEnd = _request.find(boundary + "--", dispositionStart);
        this->contentdisposition = _request.substr(dispositionStart, dispositionEnd - dispositionStart);
    }

    if (!get.empty())
    {
        size_t eofgetStart = _request.find("\r\n\r\n");
        if (eofgetStart != std::string::npos)
            eof = 1;
        else
            eof = 0;
    }
    else
    {
        size_t eofpostStart = _request.find(boundary + "--");
        if (eofpostStart != std::string::npos)
            eof = 1;
        else
            eof = 0;
    }
    if (!contentdisposition.empty() && !boundary.empty())
        this->content = Content(contentdisposition, boundary);
}

Request::Request( const Request &other )
{
    if (this != &other)
        *this = other;  
}

Request &Request::operator=( const Request &other)
{
    this->_request = other._request;
    this->get = other.get;
    this->post = other.post;
    this->host = other.host;
    this->useragent = other.useragent;
    this->accept = other.accept;
    this->acceptencoding = other.acceptencoding;
    this->acceptlanguage = other.acceptlanguage;
    this->xrequestedwith = other.xrequestedwith;
    this->connection = other.connection;
    this->referer = other.referer;
    this->upgradeinsecurerequests = other.upgradeinsecurerequests;
    this->secfetchdest = other.secfetchdest;
    this->secfetchmode = other.secfetchmode;
    this->secfetchsite = other.secfetchsite;
    this->secfetchuser = other.secfetchuser;
    this->contenttype = other.contenttype;
    this->contentlength = other.contentlength;
    this->contentdisposition = other.contentdisposition;
    this->origin = other.origin;
    this->boundary = other.boundary;
    this->content = other.content;
    this->eof = other.eof;
    return *this;
}

Content::Content(const Content &other)
{
    if (this != &other)
        *this = other; 
}

Content &Content::operator=(const Content &other)
{
    this->content_type = other.content_type;
    this->_content = other._content;
    this->filename = other.filename;
    return *this;
}

Disposition::Disposition(const Disposition &other)
{
    if (this != &other)
        *this = other; 
}

Disposition &Disposition::operator=(const Disposition &other)
{
    this->type = other.type;
    this->filename = other.filename;
    this->contentdisposition = other.contentdisposition;
    return *this;
}

Disposition::~Disposition()
{
}
Content::~Content()
{
}
Request::~Request()
{
}

Disposition::Disposition()
{
}
Content::Content()
{
}
Request::Request() : eof(0)
{
}

//#textmate (mate);
//snippets