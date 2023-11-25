#include "includes/webserv.hpp"

std::string filesizeToString(off_t size)
{
	std::string result;
	std::stringstream ss;

	size /= 1000;
	ss << size;
	result = ss.str();

	return result + " kB";
}

std::string	makeDirectoryList(std::string dirpath, std::string rel_path)
{
	struct dirent *entry;
	struct stat fileStat;
	std::string response;
	std::string responseHeaders;

	DIR	*directory = opendir(dirpath.c_str());

	if (directory == NULL)
	{
		perror("opendir");
		return "";
	}

	response.append("<html><head><title>Directory Index</title>");
	response.append("<style> td{font-family: monospace; padding: 0px 5px;} th{text-align: left;  padding: 0px 5px;}</style></head>");
	response.append("<body><h1>Index of " + dirpath + "</h1><table>");
	response.append("<tr><th>Name</th><th>Last modified</th><th>Size</th><th>Description</th></tr>");
	while ((entry = readdir(directory)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		std::string full_path = dirpath + "/" + entry->d_name;

		if (stat(full_path.c_str(), &fileStat) == 0)
		{
			std::string index_item = "<tr>";
			index_item.append("<td><a href='" + rel_path + "/" + std::string(entry->d_name) + "'>" + entry->d_name + "</a></td>");
			index_item.append("<td>" + std::string(ctime(&fileStat.st_mtime)) + "</td>");
			index_item.append("<td>" + filesizeToString(fileStat.st_size) + "</td>");
			index_item.append("</tr>");
			response.append(index_item);
		}
		else
			perror("stat");

	}
	response.append("</table></body></html>");

    std::stringstream ss;
    ss << response.length();

	responseHeaders = "HTTP/1.1 200 OK\r\n";
    responseHeaders += "Content-Type: text/html\r\n";
    responseHeaders += "Connection: keep-alive\r\n";
    responseHeaders += "Content-Length: " + ss.str() + "\r\n\r\n";

	return responseHeaders + response;
}
