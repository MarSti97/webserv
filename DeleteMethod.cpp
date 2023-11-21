#include "includes/Serv.hpp"

void Serv::deleteMethod(std::string abs, Request req) // need to test this!
{
	if (std::remove(abs.c_str()) == 0) // changed to + 1 because it wasn't getting deleted
	{
		printlog("Succefully deleted file", -1, GREEN); // the 0 for the arguemnt is shit need to fix
		parseSend(getResponse(abs, "", getHeader("204 No Content", "", abs)), req.ClientFd());
	}
	else
	{
		printerr("Failed to delete file", -1, RED);
		errorPageCheck("500", "Internal Server Error", "/500.html", req); // need a page for this
	}
}

void Serv::deleteFolderMethod(std::string path, Request req)
{
	if (deleteFolderRecusively(path))
	{
		printlog("Succefully deleted folder", -1, GREEN);
		// errorPageCheck("204", "No Content", "/204.html", req); I dont know if this works, test
		parseSend("HTTP/1.1 204 No Content\r\nConnection: keep-alive\r\n", req.ClientFd());
	}
	else
	{
		printerr("Failed to delete folder ", -1, RED);
		errorPageCheck("500", "Internal Server Error", "/500.html", req); // need a page for this
	}
}

bool Serv::deleteFolderRecusively(std::string path)
{
	struct dirent *entry;
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		printerr("Error: couldnt open dir for delete method: " + path, 0, RED);
	else
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            	continue;
			if (entry->d_type == DT_DIR)
				deleteFolderRecusively(path + "/" + entry->d_name); // does d_name have / already?
			else
			{
				std::string fileToDelete = path + "/" + entry->d_name;
				if (std::remove(fileToDelete.c_str()) != 0)
					printerr("Error: could not delete file within directory: " + fileToDelete, 0, RED);
			}
		}
		closedir(dir);
		if (rmdir(path.c_str()) != 0)
			return false;
		return true;
	}
	return false;
}
