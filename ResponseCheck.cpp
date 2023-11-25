#include "includes/Serv.hpp"

std::string Serv::createAbsolutePath(std::string path)
{
	if (path == "/")
		return serv_info.root;
	std::vector<Location>::iterator it;
	std::string page;
	std::string newPath;
	size_t pos = 0;
	while ((pos = path.find("%20", pos)) != std::string::npos) {
        path.replace(pos, 3, " ");
        pos += 1;
    }
	size_t i = path.rfind('/');
	if (path.substr(i).find('.') != std::string::npos)
	{
		page = path.substr(i);
		newPath = path.substr(0, i);
	}
	else
	{
		page = "";
		newPath = removeDashIfExists(path);
	}
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (!(it->root.empty()) && newPath == it->path)
		{
			return it->root + page;
		}
    }
	return serv_info.root + path;
}

std::string	Serv::CheckCGI( std::string path)
{
	std::vector<Location>::iterator it;
	size_t i = path.rfind('/');
	size_t j = path.rfind('.');
	std::string extension = path.substr(j);
	std::string newPath = path.substr(0, i);
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (it->path == newPath && it->cgi_extension == extension)
			return extension;
    }
    return "";
}

std::string	Serv::CheckIndex( std::string path)
{
	if (path == "/")
		return serv_info.index;
	std::vector<Location>::iterator it;
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (it->path == path && !(it->index.empty()))
			return it->index;
    }
    return "";
}

bool	Serv::CheckAutoindex(std::string path)
{
	if (path == "/" && serv_info.autoindex != "")
		return true;
	std::vector<Location>::iterator it;
    for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
    {
		if (it->path == path && (it->autoindex == "on"))
			return true;
    }
    return false;
}

Methods	Serv::CheckAllowed( std::string method, std::string path)
{
	size_t i = path.rfind("/");
	std::string newPath;
	if (*(path.end() - 1) != '/' && path.substr(i).find(".") != std::string::npos)
		newPath = path.substr(0, i + 1);
	else
		newPath = path;
	newPath = removeDashIfExists(newPath);
	while (newPath != "")
	{
		std::vector<Location>::iterator it;
		for (it = serv_info.location.begin(); it != serv_info.location.end(); ++it)
		{
			if (it->path == newPath)
			{
				if (it->methods.find(method) != it->methods.end())
				{
					return whatstheMethod(it->methods[method], method);
				}
			}
		}
		i = newPath.rfind("/");
		newPath = newPath.substr(0, i);
	}
	if (serv_info.methods.find(method) != serv_info.methods.end())
		return whatstheMethod(serv_info.methods[method], method);
    return UNDEFINED;
}
