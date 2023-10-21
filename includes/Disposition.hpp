#ifndef DISPOSITION_HPP
#define DISPOSITION_HPP

#include "webserv.hpp"

class Disposition
{
    private:
        std::string type;
        std::string filename;
        std::string contentdisposition;

    public:
        Disposition( void );
        Disposition(std::string disposition);
        ~Disposition();
        
        std::string getType( void ) const;
        std::string getFilename( void ) const;
        std::string getcontentdisposition( void ) const;
};

#endif