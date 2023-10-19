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

	DIR	*directory = opendir(dirpath.c_str() + 1);

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
		size_t i = rel_path.rfind('/');
		std::string rel_folder = rel_path.substr(i);

		if (stat(full_path.c_str() + 1, &fileStat) == 0)
		{
			std::string index_item = "<tr>";
			index_item.append("<td><a href='" + rel_folder + "/" + std::string(entry->d_name) + "'>" + entry->d_name + "</a></td>");
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

