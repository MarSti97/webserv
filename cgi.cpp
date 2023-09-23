#include "includes/webserv.hpp"

void init_cgi_meta_vars(Request &req, Config &conf)
{
	std::vector<std::string> env;
	env.push_back("AUTH_TYPE=");
	env.push_back("CONTENT_TYPE=" + req.Contenttype());
	env.push_back("CONTENT_LENGTH=" + req.Contentlength());
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	
	std::string path_info = "";
	std::string req_method = "";
	if (!(req.Get().empty()))
	{
		path_info = req.Get();
		req_method = "GET";
	}
	else if (!(req.Post().empty()))
	{	
		path_info = req.Post();
		req_method = "POST";
	}
	else if (!(req.Delete().empty()))
	{
		path_info = req.Delete();
		req_method = "DELETE";
	}
	env.push_back("REQUEST_METHOD=" + req_method);
	env.push_back("PATH_INFO=" + path_info);
	// Confirm with the tester
	env.push_back("PATH_TRANSLATED=" + conf.root + "/" + path_info);
	
	// All of this to protect against substr out of bounds
	std::string query_string = "";
	int			query_start = path_info.rfind('?');
	if (query_start != std::string::npos && query_start != path_info.length() - 1)
		query_string = path_info.substr(query_start + 1);
	
	env.push_back("QUERY_STRING=" + query_string);
	env.push_back("REMOTE_ADDR=");
	env.push_back("DOMAIN_NAME=");
	env.push_back("REMOTE_IDENT=");
	env.push_back("REMOTE_USER=");
	env.push_back("SCRIPT_NAME=" + path_info.erase(path_info.rfind('?')));
	env.push_back("SERVER_NAME=" + conf.host);
	env.push_back("SERVER_PORT=" + conf.port);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=");
}

void	create_cgi_env(std::vector<std::string> env)
{
	char **cgi_env = (char **)malloc(sizeof(char*) * 17 + 1);
	if (!cgi_env)
	{
		std::cerr << "Error allocating memory for the CGI env" << std::endl;
		return ;
	}

	int i = 0;
	std::vector<std::string>::iterator it;
	for (it = env.begin(); it != env.end(); it++)
	{
		cgi_env[i++] = strdup((*it).c_str());
	}
	cgi_env[i] = '\0';
	
}