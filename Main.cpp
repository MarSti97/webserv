#include "./includes/webserv.hpp"

int main(int ac, char **av)
{
	Servers servs;
    try {
        if (ac == 1)
        {
            printerr("No configuration file provided. Using default one (Config/example.config)", -2, RED);
            servs = Servers(std::string("Config/example.config"));
        }
        else if (ac == 2)
        {
            if (!correctfile(std::string(av[1])))
                throw NotConfigFile();
            servs = Servers(std::string(av[1]));
        }
        else
            throw TooManyArguments();
        servs.init();
    }
    catch (const std::exception& e) {
        printerr(e.what(), -2, RED);
        return 0;
    }
	//servs.printAll();
	servs.run();
    return 0;
}

int end_loop(int end)
{
    static int loop;
    if (end)
        loop = end;
    return loop;
}

void	ctrlc(int signum)
{
	if (signum == SIGINT)
	{
        end_loop(1);
        Download &down = Download::getInstance();
        down.clean();
	}
}