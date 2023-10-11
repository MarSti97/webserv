#include "includes/webserv.hpp"

std::string filesizeToString(off_t size)
{
	std::string result;
	std::stringstream ss;

	ss << size;
	result = ss.str();

	return result;
}

std::string	makeDirectoryList(std::string dirpath)
{
	struct dirent *entry;
	struct stat fileStat;
	std::string indexpage;

	DIR	*directory = opendir(dirpath.c_str());

	if (directory == NULL)
	{
		perror("opendir");
		return "";
	}

	indexpage.append("<html><head><title>Directory Index</title>");
	indexpage.append("<style> td{font-family: monospace; padding: 0px 5px;} th{text-align: left;  padding: 0px 5px;}</style></head>");
	indexpage.append("<body><h1>Index of " + dirpath + "</h1><table>");
	indexpage.append("<tr><th>Name</th><th>Last modified</th><th>Size</th><th>Description</th></tr>");
	while ((entry = readdir(directory)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		std::string full_path = dirpath + "/" + entry->d_name;

		if (stat(full_path.c_str(), &fileStat) == 0)
		{
			std::string index_item = "<tr>";
			index_item.append("<td><a href='" + std::string(entry->d_name) + "'>" + entry->d_name + "</a></td>");
			index_item.append("<td>" + std::string(ctime(&fileStat.st_mtime)) + "</td>");
			index_item.append("<td>" + filesizeToString(fileStat.st_size) + "</td>");
			index_item.append("</tr>");
			indexpage.append(index_item);
		}
		else
			perror("stat");

	}
	indexpage.append("</table></body></html>");
	return indexpage;
}