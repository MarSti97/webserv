#include "includes/webserv.hpp"
#include <sys/prctl.h>

int	Serv::cgi_request(Request req, std::string path_info, std::string script_extension)
{
	std::vector<std::string> meta_vars;
	char **cgi_env;
	std::string cmd_name;
	
	init_cgi_meta_vars(req, &meta_vars);
	cgi_env = create_cgi_env(meta_vars);

	// int i = -1;
	// while (cgi_env[++i])
	// {
	// 	std::cout << cgi_env[i] << std::endl;
	// }

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

// void	childSigHandler(int signum)
// {
// 	if (signum == SIGALRM)
// 	{
// 		std::cerr << "TIMEOUT ON CGI EXECUTION!!" << std::endl;
// 		exit(1);
// 	}
// }

// void	parentSigHandler(int signum)
// {
// 	if (signum == SIGALRM)
// 	{
// 		std::cerr << "PARENT PROCESS HERE AFTER THE ALARM!!" << std::endl;
// 		//exit(1);
// 	}
// }

// int	Serv::execute_script(std::string cmd_path, std::string path_info, char **env, Request req)
// {
// 	int		input_fd[2];
// 	int		output_fd[2];
// 	pid_t	pid;
//     int status = 0;

// 	if (pipe(input_fd) == -1 || pipe(output_fd) == -1)
// 	{
// 		perror("cgi pipe");
// 		return -1;
// 	}
// 	pid = fork();
// 	if (pid == -1)
// 	{
// 		perror("cgi fork");
// 		return -1;
// 	}
// 	if (pid == 0)
// 	{
// 		signal(SIGALRM, childSigHandler);
// 		// if (prctl(PR_SET_PDEATHSIG, SIGALRM) == -1) {
//         // 	perror("prctl");
//         // 	exit(EXIT_FAILURE);
//     	// }
// 		size_t i = path_info.rfind("/");
// 		std::string script_name = path_info.substr(i + 1);
// 		std::string path = path_info.substr(0, i);
// 		if (chdir(path.c_str() + 1) == -1)
// 		{
// 			perror("cgi chdir");
// 			return -1;
// 		}
// 		char *argv[3];
//         argv[0] = const_cast<char *>((cmd_path).c_str());
// 		argv[1] = const_cast<char *>((script_name).c_str());
// 		argv[2] = NULL;

// 		dup2(input_fd[0], STDIN_FILENO);
// 		close(input_fd[1]);
// 		close(input_fd[0]);		
		
// 		dup2(output_fd[1], STDOUT_FILENO);
// 		close(output_fd[1]);
// 		close(output_fd[0]);

// 		if (execve(cmd_path.c_str(), argv, env) == -1)
// 		{
//             perror("cgi execve");
// 			return -1;
// 		}
// 	}
// 	else
// 	{
// 		signal(SIGALRM, parentSigHandler);
// 		close(input_fd[0]);
// 		close(output_fd[1]);
// 		char *content = req.content.getContent();
// 		for (size_t f = 0; f < req.content.getContentSize(); ++f)
// 			write(input_fd[1], &content[f], 1);
// 		req.clean_content();
// 		close(input_fd[1]);

// 		int timeout_seconds = 5;
// 		alarm(timeout_seconds);
// 		if (wait(&status) == -1)
// 		{
// 			std::cout << "READY TO KILL" << std::endl;
// 			if (errno == ECHILD) {
// 			// The child process didn't exit within the timeout.
// 			std::cout << "Child process timed out" << std::endl;
// 			return (-1);
// 			// You can take appropriate action here.
// 				} else {
// 			perror("waitpid");
// 			// Handle other errors as needed.
// 			}
// 		}

// 		if (WIFEXITED(status)) {
//             std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
//         } else if (WIFSIGNALED(status)) {
//             std::cout << "Child process terminated by signal: " << WTERMSIG(status) << std::endl;
//         }
// 		return (output_fd[0]);
// 	}
// 	return (status);
// }

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
			if (current - start >= 5) // timeout of 5 seconds to stop infinite loops
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


void Serv::init_cgi_meta_vars(Request req, std::vector<std::string> *meta_vars)
{
	// std::cout << req.request() << "fuck?" << std::endl;
	meta_vars->push_back("AUTH_TYPE=");
	meta_vars->push_back("CONTENT_TYPE=" + req.Contenttype() + "; boundary=" + req.Boundary());
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
	meta_vars->push_back("PATH_TRANSLATED=" + serv_info.root + path_info);
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
		printerr("Error allocating memory for the CGI env", 0, RED);
		return NULL;
	}

	int i = 0;
	std::vector<std::string>::iterator it;
	for (it = meta_vars.begin(); it != meta_vars.end(); it++) // leaaaaaaaaaaaaaaaaks; need to fix line 167.
	{
		cgi_env[i] = new char[(*it).length() + 1];
		cgi_env[i][(*it).length()] = '\0';
		memcpy(cgi_env[i++], (*it).c_str(), (*it).length());
	}
	cgi_env[i] = NULL;
	return cgi_env;
}