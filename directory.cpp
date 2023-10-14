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

// void	filter_request(){
	// check in what "location" the request is being made and if you 
	// have permission for it (allow GET on the config file, for example)
		// if you do, check if the request is for a file or a directory
			// if it's for a directory, check if it exists
				// if yes, check if there is an index on that location
					// if there is, always serve the index
					// if there isn't, check if autoindex is on and the file has read access
						// if it does, serve the directory listing
						// if it doesn't, return error "403 Forbidden"
				// if it doesn't exist, return "404 Not Found"
			// if it's for a file, check if it exists
				// if yes, check if it has the cgi_extension and is on the cgi_directory
					// if yes, send it through the CGI (already implemented)
					// if not, serve the requested file normally
				// if it doesn't exist, return "404 Not Found"
	// if you don't have permission for that type of request, return "405 Method Not Allowed"
	// if it's a request other than GET, POST or DELETE, return "501 Not Implemented"
	// If when processing a request with body, the content-length is bigger than 
	// max_body_size, return "413 Payload Too Large"
// }

