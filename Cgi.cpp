#include "includes/webserv.hpp"

int	Serv::cgi_request(Request req, std::string path_info, std::string script_extension)
{
	std::vector<std::string> meta_vars;
	char **cgi_env;
	std::string cmd_name;
	
	init_cgi_meta_vars(req, &meta_vars, path_info);
	cgi_env = create_cgi_env(meta_vars);
	if (!cgi_env)
		return (-1);
	if (script_extension == ".sh")
		cmd_name = "bash";
	else if (script_extension == ".py")
		cmd_name = "python3";
	else
	cmd_name = script_extension.substr(1);
	int	cgi_fd = execute_script(findcommand("/" + cmd_name), path_info, cgi_env, req); 

	int i = -1;
	while (cgi_env[++i])
		delete[] cgi_env[i];
	delete[] cgi_env;
	return (cgi_fd);
}

int	Serv::execute_script(std::string cmd_path, std::string path_info, char **env, Request req)
{
	int		input_fd[2];
	int		output_fd[2];
	pid_t	pid;
    int status = 0;

	if (pipe(input_fd) == -1 || pipe(output_fd) == -1)
	{
		perror("cgi pipe");
		return -1;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("cgi fork");
		return -1;
	}
	if (pid == 0)
	{
		size_t i = path_info.rfind("/");
		std::string script_name = path_info.substr(i + 1);
		std::string path = path_info.substr(0, i);
		if (chdir(path.c_str()) == -1)
		{
			perror("cgi chdir");
			return -1;
		}
		char *argv[3];
        argv[0] = const_cast<char *>((cmd_path).c_str());
		argv[1] = const_cast<char *>((script_name).c_str());
		argv[2] = NULL;

		dup2(input_fd[0], STDIN_FILENO);
		close(input_fd[1]);
		close(input_fd[0]);		
		
		dup2(output_fd[1], STDOUT_FILENO);
		close(output_fd[1]);
		close(output_fd[0]);

		if (execve(cmd_path.c_str(), argv, env) == -1)
		{
            perror("cgi execve");
			return -1;
		}
	}
	else
	{
		time_t	start = std::time(NULL);
		close(input_fd[0]);
		close(output_fd[1]);
		char *content = req.content.getContent();
		for (size_t f = 0; f < req.content.getContentSize(); ++f)
			write(input_fd[1], &content[f], 1);
		req.clean_content();
		close(input_fd[1]);
		while (1)
		{
			time_t current = std::time(NULL);
			if (current - start >= 5)
			{
				kill(pid, SIGKILL);
				return -1;
			}
			pid_t result = waitpid(pid, &status, WNOHANG);
            if (result > 0) {
                return output_fd[0];
            }
		}
	}
	return (-1);
}


void Serv::init_cgi_meta_vars(Request req, std::vector<std::string> *meta_vars, std::string abs)
{
	meta_vars->push_back("AUTH_TYPE=");
	meta_vars->push_back("CONTENT_TYPE=" + req.Contenttype());	
	meta_vars->push_back("CONTENT_LENGTH=" + req.Contentlength());
	meta_vars->push_back("GATEWAY_INTERFACE=CGI/1.1");
	
	std::string path_info;
	std::string req_method;
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
	meta_vars->push_back("REQUEST_METHOD=" + req_method);
	meta_vars->push_back("PATH_INFO=" + path_info);
	meta_vars->push_back("PATH_TRANSLATED=" + abs);
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
	meta_vars->push_back("BODY_FILENAME=" + req.content.filename.getFilename());
}

char	**Serv::create_cgi_env(std::vector<std::string> meta_vars)
{
	char **cgi_env = new char*[18 + 1];
	if (!cgi_env)
	{
		printerr("Error allocating memory for the CGI env", -1, RED);
		return NULL;
	}

	int i = 0;
	std::vector<std::string>::iterator it;
	for (it = meta_vars.begin(); it != meta_vars.end(); it++)
	{
		cgi_env[i] = new char[(*it).length() + 1];
		cgi_env[i][(*it).length()] = '\0';
		memcpy(cgi_env[i++], (*it).c_str(), (*it).length());
	}
	cgi_env[i] = NULL;
	return cgi_env;
}

std::string Serv::findcommand(std::string command)
{
	std::string pathStr = std::getenv("PATH");
    
    if (pathStr.empty())
        return NULL;
    
    char delimiter = ':';
    
    std::vector<std::string> pathDirs;
    size_t start = 0;
    size_t end = pathStr.find(delimiter);
    
    while (end != std::string::npos) {
        std::string dir = pathStr.substr(start, end - start);
        pathDirs.push_back(dir);
        start = end + 1;
        end = pathStr.find(delimiter, start);
    }
    std::string lastDir = pathStr.substr(start);
    pathDirs.push_back(lastDir);
    std::vector<std::string>::iterator it;
    for (it = pathDirs.begin(); it != pathDirs.end(); it++)
    {
        if (access((*it + command).c_str(), F_OK) == 0)
            break ;
    }
	return (*it + command);
}
