#include "includes/webserv.hpp"

Disposition::Disposition(std::string disposition)
{
    if (!disposition.empty())
    {
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
        size_t nameEnd = disposition.find("\"", nameStart + 10);\
        if (nameStart != std::string::npos)
        {
            nameStart += 10;
            if (nameEnd > 0)
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

Disposition::~Disposition() {}

Disposition::Disposition() {}

