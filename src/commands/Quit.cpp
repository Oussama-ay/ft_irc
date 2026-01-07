#include "../../include/Server.hpp"

void	Server::notifyQuitFromChannels(Client& client, const std::string& quitMessage)
{
	std::map<std::string, Channel *>::iterator chanIt = m_channels.begin();
	
	while (chanIt != m_channels.end())
	{
		if (chanIt->second->hasMember(&client))
		{
			std::string quitMsg = ":" + makePrefix(client) + " QUIT :" + quitMessage + "\r\n";
			broadcast(chanIt->second, quitMsg);
			chanIt->second->removeMember(&client);
		}
		++chanIt;
	}
}

void	Server::handleQuit(Client& client, const Command& cmd)
{
	std::string quitMessage = "Goodbye";
	
	// if there is optional quit message
	if (cmd.args.size() > 0)
	{
		quitMessage = "";
		for (size_t i = 0; i < cmd.args.size(); ++i)
		{
			if (i > 0)
				quitMessage += " ";
			quitMessage += cmd.args[i];
		}
	}

	notifyQuitFromChannels(client, quitMessage);
	
	removeClient(client.getFd());
}
