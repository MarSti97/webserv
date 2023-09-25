#include "./includes/webserv.hpp"
#include "./includes/Config.hpp"
#include "./includes/Servers.hpp"


std::string	parse_attribute(std::istringstream &iss, std::string token)
{
	std::string parsed;
	
	iss >> token;
	if (*(token.end() - 1) == ';' && !check_new_attribute(token))
		parsed = token.substr(0, token.size() - 1);
	return (parsed);
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
	std::cout << (serv_info.allow_get ? "GET: allowed" : "GET: denied") << std::endl;
	std::cout << (serv_info.allow_post ? "POST: allowed" : "POST: denied") << std::endl;
	std::cout << (serv_info.allow_delete ? "DELETE: allowed" : "DELETE: denied") << std::endl;
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
		std::cout << (it->allow_get ? "GET: allowed" : "GET: denied") << std::endl;
		std::cout << (it->allow_post ? "POST: allowed" : "POST: denied") << std::endl;
		std::cout << (it->allow_delete ? "DELETE: allowed" : "DELETE: denied") << std::endl;
		std::cout << "redirect_status: " << it->redirect_status << std::endl;
		std::cout << "redirect_path: " << it->redirect_path << std::endl;			
		if (!(it->error_pages.empty()))
		{
			std::map<std::string, std::string>::const_iterator it3;
			for (it3 = it->error_pages.begin(); it3 != it->error_pages.end(); it3++)
			{
				std::cout << "error_page " << it3->first << ": " << it3->second << std::endl;
			}
		}
	}
}

bool check_new_attribute(std::string token)
{	
	std::string attributes[] = {"listen", "server_name", "root", "client_max_body_size", "autoindex", "cgi", "index", "location", "allow"};
	
	for (size_t i = 0; i < 6; i++)
	{
		if (token == attributes[i])
			return true;
	}
	return false;
}

void	check_requirements(Config temp, int i)
{
	if (temp.port.empty() || temp.server_name.empty()) {
		std::cerr << "Error: Insufficient information on server configuration " << i << ".\nYou need at least a listen port and a server address.";
		throw InsufficientInformation();
	}
}

bool correctfile(std::string file)
{
	size_t point = file.find(".");
	if (point != std::string::npos)
		if (file.substr(point) != ".config")
			return false;
	return true;
}