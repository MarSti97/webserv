#include "includes/Serv.hpp"
#include "includes/Servers.hpp"

bool Servers::headcheck(std::string buf)
{
    if (buf.substr(0, 5) == "POST " || buf.substr(0, 4) == "GET " || buf.substr(0, 7) == "DELETE " || buf.find("HTTP/1.1\r\n") != std::string::npos)
        return true;
    return false;
}

void	Serv::print(int counter) const
{
	std::cout << std::endl;
	std::cout << "Server Configuration " << counter << std::endl;
	std::cout << "host: " << serv_info.host << std::endl;	
	std::cout << "port: " << serv_info.port << std::endl;
	std::cout << "root: " << serv_info.root << std::endl;
	std::cout << "index: " << serv_info.index << std::endl;
	std::cout << "server_name: ";
	for (size_t j = 0; j < serv_info.server_name.size(); j++)
	{
		std::cout << serv_info.server_name[j] << " ";
	}
	std::cout << std::endl;
	std::cout << "max_body_size: " << serv_info.max_body_size << std::endl;
	std::map<std::string, Methods>::const_iterator its;
	for (its = serv_info.methods.begin(); its != serv_info.methods.end(); its++)
	{
		std::cout << "Method: " << its->first << " : " << (its->second ? "denied" : "allowed") << std::endl;
	}
	std::cout << "autoindex: " << serv_info.autoindex << std::endl;
	std::cout << "cgi_extension: " << serv_info.cgi_extension << std::endl;
	std::cout << "cgi_directory: " << serv_info.cgi_directory << std::endl;
	std::map<std::string, std::string>::const_iterator it2;
	for (it2 = serv_info.error_pages.begin(); it2 != serv_info.error_pages.end(); it2++)
	{
		std::cout << "error_page " << it2->first << ": " << it2->second << std::endl;
	}
	std::vector<Location>::const_iterator it;
	for (it = serv_info.location.begin(); it != serv_info.location.end(); it++)
	{
		std::cout << std::endl;
		std::cout << "Location " << it->path << std::endl;
		std::cout << "root: " << it->root << std::endl;
		std::cout << "index: " << it->index << std::endl;
		std::cout << "cgi-extension: " << it->cgi_extension << std::endl;
		std::map<std::string, Methods>::const_iterator it3;
		for (it3 = it->methods.begin(); it3 != it->methods.end(); it3++)
		{
			std::cout << "Method: " << it3->first << " : " << (it3->second ? "denied" : "allowed") << std::endl;
		}
		std::cout << "redirect_status: " << it->redirect_status << std::endl;
		std::cout << "redirect_path: " << it->redirect_path << std::endl;			
	}
}

void printMethods(std::map<std::string, Methods> map)
{
	std::map<std::string, Methods>::const_iterator its;
	for (its = map.begin(); its != map.end(); its++)
	{
		switch (its->second)
		{
			case ALLOWED :
				std::cout << "allowed";
				break;
			case DENIED :
				std::cout << "denied";
				break;
			default :
				std::cout << "undefined";
				break;
		}
		std::cout << std::endl;
	}
}

std::string	Serv::findFolder( std::string path )
{
    size_t i = path.rfind('/');
	if (path.substr(i).rfind('.') != std::string::npos)
		return path;
	else
		return "";
}

bool	Serv::ext_CGI(std::string path_info)
{
	std::string extension_string;

	size_t	extension_start = path_info.rfind('.');
	if (extension_start != std::string::npos)
	{
		size_t query_start = path_info.rfind('?');
		if (query_start != std::string::npos)
			extension_string = path_info.substr(extension_start, query_start - extension_start);
		else
			extension_string = path_info.substr(extension_start);
		if (extension_string == serv_info.cgi_extension)
			return 1;
	}
	return 0;
}
