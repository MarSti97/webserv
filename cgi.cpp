#include "includes/webserv.hpp"

void	filter_request(Request &req, Config &conf)
{
	int	has_query_strings = 0;
	int	has_script_extension = 0;
	std::string	path_info;

	if (!(req.Get().empty()))
		path_info = req.Get();
	else if (!(req.Post().empty()))
		path_info = req.Post();

	size_t	query_start = path_info.rfind('?');
	if (query_start != std::string::npos)
		has_query_strings = 1;

	size_t	extension_start = path_info.rfind('.');
	std::cout << conf.cgi_extension << std::endl;
	if (extension_start != std::string::npos)
	{
		std::string extension_string = path_info.substr(query_start);

		std::cout << extension_string << std::endl;
		if (extension_string == conf.cgi_extension)
			has_script_extension = 1;
		// else if the file is a script but cgi isn't enabled (we can check if the file isn't html, css or common image types)
			// throw error 403
	}

	if (has_script_extension == 1 || has_query_strings == 1)
		cgi_request(req, conf);
	// else
		// respond to the non-cgi request
}

void	cgi_request(Request &req, Config &conf)
{
	std::vector<std::string> meta_vars;
	char **cgi_env;
	
	init_cgi_meta_vars(req, conf, &meta_vars);
	cgi_env = create_cgi_env(meta_vars);

	int i = -1;
	while (cgi_env[++i])
	{
		std::cout << cgi_env[i] << std::endl;
	}
}

void init_cgi_meta_vars(Request &req, Config &conf, std::vector<std::string> *meta_vars)
{
	meta_vars->push_back("AUTH_TYPE=");
	meta_vars->push_back("CONTENT_TYPE=" + req.Contenttype());
	meta_vars->push_back("CONTENT_LENGTH=" + req.Contentlength());
	meta_vars->push_back("GATEWAY_INTERFACE=CGI/1.1");
	
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
	// else if (!(req.Delete().empty()))
	// {
	// 	path_info = req.Delete();
	// 	req_method = "DELETE";
	// }
	meta_vars->push_back("REQUEST_METHOD=" + req_method);
	meta_vars->push_back("PATH_INFO=" + path_info);
	// Confirm with the tester
	meta_vars->push_back("PATH_TRANSLATED=" + conf.root + "/" + path_info);
	
	// All of this to protect against substr out of bounds
	std::string query_string = "";
	size_t		query_start = path_info.rfind('?');
	if (query_start != std::string::npos && query_start != path_info.length() - 1)
		query_string = path_info.substr(query_start + 1);
	
	meta_vars->push_back("QUERY_STRING=" + query_string);
	meta_vars->push_back("REMOTE_ADDR=");
	meta_vars->push_back("DOMAIN_NAME=");
	meta_vars->push_back("REMOTE_IDENT=");
	meta_vars->push_back("REMOTE_USER=");
	meta_vars->push_back("SCRIPT_NAME=" + path_info.erase(path_info.rfind('?')));
	meta_vars->push_back("SERVER_NAME=" + conf.host);
	meta_vars->push_back("SERVER_PORT=" + conf.port);
	meta_vars->push_back("SERVER_PROTOCOL=HTTP/1.1");
	meta_vars->push_back("SERVER_SOFTWARE=");
}

char	**create_cgi_env(std::vector<std::string> meta_vars)
{
	char **cgi_env = (char **)malloc(sizeof(char*) * 17 + 1);
	if (!cgi_env)
	{
		std::cerr << "Error allocating memory for the CGI env" << std::endl;
		return NULL;
	}

	int i = 0;
	std::vector<std::string>::iterator it;
	for (it = meta_vars.begin(); it != meta_vars.end(); it++)
	{
		cgi_env[i++] = strdup((*it).c_str());
	}
	cgi_env[i] = NULL;
	// don't forget to free at the end!
	return cgi_env;
}