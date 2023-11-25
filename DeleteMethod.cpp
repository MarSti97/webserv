#include "includes/Serv.hpp"

void Serv::deleteMethod(std::string abs, Request req)
{
	if (std::remove(abs.c_str()) == 0)
	{
		parseSend(getResponse(abs, "", getHeader("204 No Content", "", abs)), req.ClientFd(), req);
	}
	else
	{
		printerr("Failed to delete file by", req.ClientFd() - 2, RED);
		errorPageCheck("500", "Internal Server Error", "DefaultError/500.html", req);
	}
}

void Serv::deleteFolderMethod(std::string path, Request req)
{
	if (deleteFolderRecusively(path))
	{
		parseSend("HTTP/1.1 204 No Content\r\nConnection: keep-alive\r\n", req.ClientFd(), req);
	}
	else
	{
		printerr("Failed to delete folder by", req.ClientFd() - 2, RED);
		errorPageCheck("500", "Internal Server Error", "DefaultError/500.html", req);
	}
}

bool Serv::deleteFolderRecusively(std::string path)
{
	struct dirent *entry;
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		printerr("Error: couldnt open dir for delete method: " + path, -1, RED);
	else
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            	continue;
			if (entry->d_type == DT_DIR)
				deleteFolderRecusively(path + "/" + entry->d_name);
			else
			{
				std::string fileToDelete = path + "/" + entry->d_name;
				if (std::remove(fileToDelete.c_str()) != 0)
					printerr("Error: could not delete file within directory: " + fileToDelete, -1, RED);
			}
		}
		closedir(dir);
		if (rmdir(path.c_str()) != 0)
			return false;
		return true;
	}
	return false;
}
