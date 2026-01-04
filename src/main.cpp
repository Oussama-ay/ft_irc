#include "../include/Server.hpp"
#include "../include/Parser.hpp"

static void	SignalHandler(int signum)
{
	(void)signum;
	throw	std::runtime_error("Signal Received!");
}

int	main(int ac, char **av)
{
	int			port;
	std::string	password;

	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ft_irc <port> <password>");

		signal(SIGINT, SignalHandler); // (ctrl + c)
		signal(SIGQUIT, SignalHandler); // (ctrl + \)

		Parser::parseArguments(av, port, password);

		Server	srv(port, password); // init the server
		srv.run(); // run the server
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
