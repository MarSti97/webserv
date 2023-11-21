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

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[fileSize];
    file.read(buffer, fileSize);
    file.close();
    std::string htmlContent(buffer, fileSize);
    delete[] buffer;

    return htmlContent;
}

// Define a map to associate file extensions with MIME types
void initializeMimeTypes(std::map<std::string, std::string> &mimeTypes) {
    mimeTypes[".html"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".ttf"] = "font/ttf";
    mimeTypes[".scss"] = "text/x-scss";
    mimeTypes[".map"] = "application/octet-stream";
    mimeTypes[".woff"] = "font/woff";
    mimeTypes[".woff2"] = "font/woff2";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".php"] = "application/x-httpd-php";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".ico"] = "image/vnd.microsoft.icon";
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
    return "text/html";
}