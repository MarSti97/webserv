#include "includes/webserv.hpp"
#include "includes/Content.hpp"

Content::~Content() {}

Content::Content() : _content(NULL), content_size(0) {chunkedData = false;}

Content::Content(std::string content, std::string boundary)
{
    chunkedData = false;
    if (!content.empty() && !boundary.empty())
    {
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

        _content = NULL;
    }
}

char *Content::getContent( void ) const
{
    return _content;
}

void Content::setContent( char *newcontent, size_t size )
{
    char *hass = new char[size + 1];
    hass[size] = '\0';
    for (size_t i = 0; i < size; ++i)
    {
        if (newcontent != NULL)
            hass[i] = newcontent[i];
    }
    _content = hass;
}

size_t Content::getContentSize( void ) const
{
    return content_size;
}

void Content::setContentSize( size_t size )
{
    content_size = size;
}


std::string Content::getContentType( void ) const
{
    return content_type;
}

void    Content::clean()
{
    delete[] _content;
}

void    Content::setChunkedBool(bool change)
{
    this->chunkedData = change;
}

bool    Content::getChunkedBool()
{
    return chunkedData;
}

