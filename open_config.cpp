#include "./includes/webserv.hpp"
#include <map>
#include <iostream>
#include <fstream>
#include <string>

std::string readFile( std::string filePath ) {
    std::ifstream file(filePath.c_str());

    if (!file) {
        return "";//readFile("404.html");
    }

    // Get the size of the file
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a buffer to hold the file content
    char* buffer = new char[fileSize];

    // Read the file content into the buffer
    file.read(buffer, fileSize);

    // Close the file
    file.close();

    // Now, 'buffer' contains the content of the HTML file

    // Optionally, you can convert it to a C++ string
    std::string htmlContent(buffer, fileSize);

    // Remember to free the allocated memory when done
    delete[] buffer;

    // Use 'htmlContent' or 'buffer' as needed
    // ...

    return htmlContent;
}

// Define a map to associate file extensions with MIME types
void initializeMimeTypes(std::map<std::string, std::string> &mimeTypes) {
    mimeTypes[".html"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".ttf"] = "font/ttf";
    mimeTypes[".scss"] = "text/x-scss"; // SCSS (Sass) file
    mimeTypes[".map"] = "application/octet-stream"; // CSS map file (default)
    mimeTypes[".woff"] = "font/woff";
    mimeTypes[".woff2"] = "font/woff2";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".php"] = "application/x-httpd-php";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".ico"] = "image/vnd.microsoft.icon";
    // Add more mappings as needed
}

// Function to get the MIME type based on the file extension
std::string getMimeType(const std::string& filePath)
{
    std::map<std::string, std::string> mimeTypes;

    initializeMimeTypes(mimeTypes);
    size_t dotPos = filePath.rfind(".");
    if (dotPos != std::string::npos) {
        std::string extension = filePath.substr(dotPos);
        std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }
    return "text/html"; // Default MIME type
}

// int	execute_command(std::string pathcmd, std::string pathfile, char **env)
// {
// 	int		output_fd[2];
// 	pid_t	pid;
//     int status = 0;

// 	if (pipe(output_fd) == -1)
// 		perror("pipe");
// 	pid = fork();
// 	if (pid == -1)
// 		perror("fork");
// 	if (pid == 0)
// 	{
//         char *argv[2];
//         argv[0] = const_cast<char *>((pathcmd).c_str());
// 		// temporary fix just to test, replace with cgi-path of the config file
// 		pathfile = "guarder-html" + pathfile;
//         argv[1] = const_cast<char *>((pathfile).c_str());
// 		close(output_fd[0]);
//         int fd = open("/dev/null", O_RDWR);
//         dup2(fd, STDIN_FILENO);
//         dup2(fd, STDOUT_FILENO);
//         dup2(fd, STDERR_FILENO);
//         close(fd);
// 		if (execve(pathcmd.c_str(), argv, env) == -1)
//             perror("execve");
// 	}
// 	else
// 	{
// 		close(output_fd[0]);
// 		close(output_fd[1]);
// 		wait(&status);
// 	}
// 	return (status);
// }
