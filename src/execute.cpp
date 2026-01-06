#include "../include/Client.hpp"
#include "../include/Parser.hpp"
#include "../include/Server.hpp"

void Server::execute(Client& client, const std::vector<Command>& inputLines)
{
	std::map<std::string, CommandHandler>::iterator	it;
	std::string										msg;

	for (size_t i = 0; i < inputLines.size(); ++i)
	{
		it = commandMap.find(inputLines[i].command);
		if (it != commandMap.end())
			(this->*(it->second))(client, inputLines[i]);
		else
		{
			// if (client.getStat() != REGISTERED){}
				// send(client.getFd(), "ERROR :You must complete registration first (PASS/NICK/USER)\r\n", 63, 0);
			// else
			// {
				// msg = ":server 421 " + client.getNickname() + " " 
				// 		+ inputLines[i].command + " :Unknown command\r\n";
				// send(client.getFd(), msg.c_str(), msg.size(), 0);
			// }
		}
	}
}
