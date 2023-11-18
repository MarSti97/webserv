#ifndef CONTENT_HPP
#define CONTENT_HPP

#include "webserv.hpp"

class Disposition;

class Content
{
    private:
        std::string content_type;
        char        *_content;
        size_t	content_size;
        bool chunkedData;
    
    public:
		Content();
        Content(std::string content, std::string boundary);
        ~Content();

        Disposition filename;
        void    clean( void );
        size_t  getContentSize( void ) const;
        void    setContentSize( size_t size );
        char    *getContent( void ) const;
        void    setContent( char *newcontent, size_t size );
        std::string getContentType( void ) const;
        void    setChunkedBool(bool change);
        bool    getChunkedBool();
};

#endif