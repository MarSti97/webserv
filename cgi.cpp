#include "includes/webserv.hpp"

void	Serv::filter_request(Request &req)
{
	int	has_script_extension = 0;
	std::string	path_info;
	std::string extension_string;

	if (!(req.Get().empty()))
		path_info = req.Get();
	else if (!(req.Post().empty()))
		path_info = req.Post();

	size_t	extension_start = path_info.rfind('.');
	if (extension_start != std::string::npos)
	{
		size_t query_start = path_info.rfind('?');
		if (query_start != std::string::npos)
			extension_string = path_info.substr(extension_start, query_start - extension_start);
		else
			extension_string = path_info.substr(extension_start);
		if (extension_string == serv_info.cgi_extension)
			has_script_extension = 1;
		// else if the file is a script but cgi isn't enabled (we can check if the file isn't html, css or common image types)
			// throw error 403
	}

	if (has_script_extension == 1) // || has_query_strings == 1)
	{
		cgi_request(req, path_info, extension_string);
	}
	// else
		// respond to the non-cgi request
}

void	Serv::cgi_request(Request &req, std::string path_info, std::string script_extension)
{
	std::vector<std::string> meta_vars;
	char **cgi_env;
	std::string cmd_name;
	
	init_cgi_meta_vars(req, &meta_vars);
	cgi_env = create_cgi_env(meta_vars);

	int i = -1;
	while (cgi_env[++i])
	{
		std::cout << cgi_env[i] << std::endl;
	}

	if (script_extension == ".sh")
		cmd_name = "bash";
	else if (script_extension == ".py")
		cmd_name = "python3";
	else
		cmd_name = script_extension.substr(1);
	execute_script(findcommand("/" + cmd_name), path_info, cgi_env); 

	i = -1;
	while (cgi_env[++i])
		delete[] cgi_env[i];
	delete[] cgi_env;

}

int	Serv::execute_script(std::string cmd_path, std::string path_info, char **env)
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

		while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
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


void Serv::init_cgi_meta_vars(Request &req, std::vector<std::string> *meta_vars)
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
	meta_vars->push_back("PATH_TRANSLATED=" + serv_info.root + "/" + path_info);
	meta_vars->push_back("QUERY_STRING=" + req.Query());
	meta_vars->push_back("REMOTE_ADDR=");
	meta_vars->push_back("DOMAIN_NAME=");
	meta_vars->push_back("REMOTE_IDENT=");
	meta_vars->push_back("REMOTE_USER=");
	meta_vars->push_back("SCRIPT_NAME=" + path_info);
	meta_vars->push_back("SERVER_NAME=" + serv_info.host);
	meta_vars->push_back("SERVER_PORT=" + serv_info.port);
	meta_vars->push_back("SERVER_PROTOCOL=HTTP/1.1");
	meta_vars->push_back("SERVER_SOFTWARE=");
}

char	**Serv::create_cgi_env(std::vector<std::string> meta_vars)
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
	return cgi_env;
}