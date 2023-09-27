#include "includes/webserv.hpp"

void	filter_request(Request &req, Config &conf)
{
	int	has_query_strings = 0;
	int	has_script_extension = 0;
	std::string	path_info;
	std::string extension_string;

	if (!(req.Get().empty()))
		path_info = req.Get();
	else if (!(req.Post().empty()))
		path_info = req.Post();

	size_t	query_start = path_info.rfind('?');
	if (query_start != std::string::npos)
		has_query_strings = 1;

	size_t	extension_start = path_info.rfind('.');
	//std::cout << conf.cgi_extension << std::endl;
	if (extension_start != std::string::npos)
	{
		extension_string = path_info.substr(query_start);

		//std::cout << extension_string << std::endl;
		if (extension_string == conf.cgi_extension)
			has_script_extension = 1;
		// else if the file is a script but cgi isn't enabled (we can check if the file isn't html, css or common image types)
			// throw error 403
	}

	if (has_script_extension == 1 || has_query_strings == 1)
		cgi_request(req, conf, path_info, extension_string);
	// else
		// respond to the non-cgi request
}

void	cgi_request(Request &req, Config &conf, std::string path_info, std::string script_extension)
{
	std::vector<std::string> meta_vars;
	char **cgi_env;
	std::string cmd_name;
	
	init_cgi_meta_vars(req, conf, &meta_vars);
	cgi_env = create_cgi_env(meta_vars);
	// trying to print to test
	// int i = -1;
	// while (cgi_env[++i])
	// {
	// 	std::cout << cgi_env[i] << std::endl;
	// }

	if (script_extension == ".sh")
		cmd_name = "bash";
	else if (script_extension == ".py")
		cmd_name = "python";
	else
		cmd_name = script_extension.substr(1);
	execute_script(findcommand("/" + cmd_name), path_info, cgi_env); 

	int i = -1;
	while (cgi_env[++i])
		delete[] cgi_env[i];
	delete[] cgi_env;

}

int	execute_script(std::string cmd_path, std::string path_info, char **env)
{
	int		pipe_fd[2];
	pid_t	pid;
    int status = 0;

	if (pipe(pipe_fd) == -1)
		perror("pipe");
	pid = fork();
	if (pid == -1)
		perror("fork");
	if (pid == 0)
	{
        char *argv[2];
        argv[0] = const_cast<char *>((cmd_path).c_str());
		// temporary fix just to test, replace with cgi-path of the config file
		path_info = "guarder-html" + path_info;
        argv[1] = const_cast<char *>((path_info).c_str());
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		close(pipe_fd[0]);

		if (execve(cmd_path.c_str(), argv, env) == -1)
            perror("execve");
	}
	else
	{
		close(pipe_fd[1]);

		char buffer[4096];
        ssize_t bytes_read;

		while (bytes_read = read(pipe_fd[0], buffer, sizeof(buffer)) > 0)
		{
			// this is the output of the executed script, it should inclue a header as well
			// will need to send the contents of buffer (should include a header)
			// to the parseSend function, like the getResponse function does
		}

		// if the script has no output, we should redirect to 
		// the same page where the script was called ("Referer" attribute)

		close(pipe_fd[0]);
		wait(&status);
	}
	return (status);
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