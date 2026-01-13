#include "../include/Client.hpp"
#include "../include/Parser.hpp"
#include "../include/Server.hpp"

void Server::execute(Client& client, const std::vector<Command>& inputLines)
{
	std::map<std::string, CommandHandler>::iterator	it;
	std::string										msg;
	size_t											size;

	size = inputLines.size();
	for (size_t i = 0; i < size; ++i)
	{
		it = commandMap.find(inputLines[i].command);
		if (it != commandMap.end())
			(this->*(it->second))(client, inputLines[i]);
		else
		{
			msg = numeric("421", client.getNickname(), inputLines[i].command + " :Unknown command");
			sendTo(client, msg);
		}
	}
}
