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

std::string Request::TransferEncoding() const
{
    return transferencoding;
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

Request::Request( char *buffer, size_t size ) : continue_100(false)
{
    this->c_request = buffer;
    this->_request = std::string(c_request, size);
    content.setContentSize(0);

	this->get = getINFOone(_request, "GET ", 4);
    size_t queryStart = _request.find("GET ");
    if (queryStart != std::string::npos)
    {
        queryStart += 4;
        size_t queryEnd = _request.find(" ", queryStart);
        size_t queryEnd2 = _request.find("?", queryStart);
        if (queryEnd2 < queryEnd)
            this->query = _request.substr(queryEnd2, queryEnd - queryEnd2);
    }
	this->post = getINFOone(_request, "POST ", 5);

    this->del = getINFOone(_request, "DELETE ", 7);

	this->host = getINFOtwo(_request, "Host: ", 6);
	size_t splitter = host.find(":");
	if (splitter != std::string::npos)
	{
		this->port = this->host.substr(splitter + 1);
		std::string temp = this->host.substr(0, splitter);
		this->host = temp;
	}

	this->useragent = getINFOtwo(_request, "User-Agent: ", 12);
	this->accept = getINFOtwo(_request, "Accept: ", 8);
	this->acceptlanguage = getINFOtwo(_request, "Accept-Language: ", 17);
	this->acceptencoding = getINFOtwo(_request, "Accept-Encoding: ", 17);
	this->xrequestedwith = getINFOtwo(_request, "X-Requested-With: ", 18);
	this->connection = getINFOtwo(_request, "Connection: ", 12);
    this->expect = getINFOtwo(_request, "Expect: ", 8);
	this->transferencoding = getINFOtwo(_request, "Transfer-Encoding: ", 19);

    size_t refererStart = _request.find("Referer: ");
    if (refererStart != std::string::npos)
    {
        refererStart += 9;
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
    else
    {
        if (atoi(contentlength.c_str()) != 0)
            content.setContentSize(atoi(contentlength.c_str()));
    }
}

int Request::EndBoundary( char *str, size_t len, char *bound)
{
    size_t f = -1;
    while (++f < len)
    {
        if (memcmp(str + f, bound, strlen(bound)))
            return f;
    }
    return 0;
}

void    Request::clean_content()
{
    content.clean();
}

int nextSize(std::string str, int pos)
{
    int next = str.find("\r\n", pos);
    if ((size_t)next == std::string::npos)
        return 0;
    char *end_ptr;
    std::string chunk_size = str.substr(pos, pos - next);
    int size = strtol(chunk_size.c_str(), &end_ptr, 16);
    return size;
}

int    Request::processChunked(int current_len, Download &down, int client)
{
    int head = _request.find("\r\n\r\n");
    if ((size_t)head == std::string::npos)
    {
        printerr("Error: No head on request", -1, RED);
        return 3;
    }
    head += 4;
    if (expect == "100-continue")
    {
        if (current_len == head)
        {
            continue_100 = true;
            return 0;
        }
    }
    if (transferencoding == "chunked")
    {
        if (_request.rfind("0\r\n\r\n") == std::string::npos)
            return 2;
        std::vector<std::pair<char *, int> > res;
        int chunk_size = nextSize(_request, head);
        int counter = 0;
        while (chunk_size != 0)
        {
            char *str = new char[chunk_size + 1];
            str[chunk_size] = '\0';
            head += getIntSize(chunk_size) + 2;
            for (int i = 0; i != chunk_size; ++i)
                str[i] = c_request[head + i];
            res.push_back(std::make_pair(str, chunk_size));
            counter += chunk_size;
            head += chunk_size + 2;
            chunk_size = nextSize(_request, head);
        }
        char temp[counter + 1];
        std::vector<std::pair<char *, int> >::iterator it;
        int f = 0;
        for (it = res.begin(); it != res.end(); ++it)
        {
            int i = -1;
            while (++i < it->second)
            {
                temp[f] = it->first[i];
                f++;
            }
            delete[] it->first;
        }
        temp[counter] = '\0';
        content.setContent(temp, counter);
        content.setChunkedBool(true);
        content.setContentSize(counter);
        down.eraseClient(client);
        return 0;
    }
    return 1;
}

bool Request::getContinue100() const
{
    return continue_100;
}

Request::~Request() {}

Request::Request() : c_request(NULL), eof(0), continue_100(false) {}
