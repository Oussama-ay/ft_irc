#include "../include/Server.hpp"

static void	SignalHandler(int signum)
{
	(void)signum;
	throw	std::runtime_error("Signal Received!");
}

int	main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ft_irc <port> <password>");

		signal(SIGINT, SignalHandler); // (ctrl + c)
		signal(SIGQUIT, SignalHandler); // (ctrl + \)

		int port = std::atoi(av[1]); // check if the number and pass is valid or not !!
		if (port <= 0)
			throw std::runtime_error("Invalid port");

		Server srv(port, av[2]); // init the server
		srv.run(); // run the server
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
