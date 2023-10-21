#include "includes/webserv.hpp"
#include "includes/Request.hpp"

std::string Request::request( void ) const
{
    return _request;
}


char *Request::C_request( void )
{
    return c_request;
}

std::string Request::Get( void ) const
{
    return get;
}

std::string Request::Del( void ) const
{
    return del;
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

int Request::ClientFd( void )
{
    return clientfd;
}

void    Request::SetClientFd( int fd )
{
    clientfd = fd;
}

Request::Request( char *buffer, size_t size )
{
    // char reqqu[size + 1];
    // memcpy(reqqu, buffer, size);
    // delete[] buffer;
    // reqqu[size] = '\0';
    this->c_request = buffer;
    this->_request = std::string(c_request, size);
    content.setContentSize(0);

	this->get = getINFOone(_request, "GET ", 4);
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
	this->post = getINFOone(_request, "POST ", 5);

    this->del = getINFOone(_request, "DELETE ", 7);

	this->host = getINFOtwo(_request, "Host: ", 6);
	// std::cout << "THIS: " << this->host << std::endl;
	size_t splitter = host.find(":");
	if (splitter != std::string::npos)
	{
		this->port = this->host.substr(splitter + 1);
		std::string temp = this->host.substr(0, splitter);
		this->host = temp;
		// std::cout << "THAT: " << this->host << " and " << this->port << std::endl;
	}

	this->useragent = getINFOtwo(_request, "User-Agent: ", 12);
	this->accept = getINFOtwo(_request, "Accept: ", 8);
	this->acceptlanguage = getINFOtwo(_request, "Accept-Language: ", 17);
	this->acceptencoding = getINFOtwo(_request, "Accept-Encoding: ", 17);
	this->xrequestedwith = getINFOtwo(_request, "X-Requested-With: ", 18);
	this->connection = getINFOtwo(_request, "Connection: ", 12);

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

	this->upgradeinsecurerequests = getINFOtwo(_request, "Upgrade-Insecure-Requests: ", 30);
	this->secfetchuser = getINFOtwo(_request, "Sec-Fetch-User: ", 16);
	this->secfetchdest = getINFOtwo(_request, "Sec-Fetch-Dest: ", 16);
	this->secfetchmode = getINFOtwo(_request, "Sec-Fetch-Mode: ", 16);
	this->secfetchsite = getINFOtwo(_request, "Sec-Fetch-Site: ", 16);

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

	this->boundary = getINFOtwo(_request, "boundary=", 9);
	this->contentlength = getINFOtwo(_request, "Content-Length: ", 16);
	this->origin = getINFOtwo(_request, "Origin: ", 8);

    size_t dispositionStart = _request.find(this->boundary, _request.find("\r\n\r\n", 4));
    if (dispositionStart != std::string::npos)
    {
        dispositionStart += boundary.size() + 2;
        // size_t dispositionEnd = _request.find("\r\n\r\n");
        this->contentdisposition = _request.substr(dispositionStart);
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
    if (!boundary.empty())
        this->content = Content(contentdisposition, boundary);
}

int Request::EndBoundary( char *str, size_t len, char *bound)
{
    size_t f = -1;
    while (++f < len)
    {
        if (memcmp(str + f, bound, strlen(bound)))
            return f;
    }
    // std::cout << f << std::endl;
    return 0;
}

void    Request::clean_content()
{
    // fix and make do delete one selected sockets too
    content.clean();
}

Request::~Request() {}

Request::Request() : c_request(NULL), eof(0) {}

//#textmate (mate);
//snippets