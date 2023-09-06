#include "webserv.hpp"
#include <map>
#include <iostream>
#include <fstream>
#include <string>

std::string readFile( std::string filePath ) {
    std::ifstream file(filePath.c_str());

    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        return NULL;
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

std::string getURL(char *buffer)
{
    std::string buf(buffer);

    size_t start = buf.find("GET ");
    if (start == std::string::npos)
        return "";
    
    size_t end = buf.find(" ", start + 4);
    if (start == std::string::npos)
        return "";

    return buf.substr(start + 4, end - start - 4);
}