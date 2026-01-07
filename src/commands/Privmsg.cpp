#include "../../include/Server.hpp"

Client	*Server::findClientByNickname(const std::string& nickname) const
{
    std::map<std::string, Client *>::const_iterator it = m_nicknames.find(nickname);
    if (it != m_nicknames.end())
    {
        return (it->second);
    }
	return (NULL);
}

void	Server::sendPrivmsgToChannel(Client& client, Channel* channel, const std::string& message)
{
	std::string privMsg = ":" + makePrefix(client) + " PRIVMSG " + channel->getName() + " :" + message + "\r\n";
	broadcast(channel, privMsg);
}

void	Server::sendPrivmsgToUser(Client& client, Client* targetClient, const std::string& message)
{
	std::string privMsg = ":" + makePrefix(client) + " PRIVMSG " + targetClient->getNickname() + " :" + message + "\r\n";
	sendTo(*targetClient, privMsg);
}

void	Server::handlePrivmsg(Client& client, const Command& cmd)
{
	if (!client.isRegistered())
	{
		sendTo(client, numeric("451", client.getNickname(), ":You have not registered"));
		return ;
	}
	if (cmd.args.size() < 2)
	{
		sendTo(client, numeric("461", client.getNickname(), "PRIVMSG :Not enough parameters"));
		return ;
	}

	std::string target = cmd.args[0];
	std::string message = "";

	for (size_t i = 1; i < cmd.args.size(); ++i)
	{
		if (i > 1)
			message += " ";
		message += cmd.args[i];
	}

	if (target[0] == '#')
	{
		std::map<std::string, Channel *>::iterator it = m_channels.find(target);
		if (it == m_channels.end())
		{
			sendTo(client, numeric("403", client.getNickname(), target + " :No such channel"));
			return ;
		}

		// Check if client is in the channel
		if (!it->second->hasMember(&client))
		{
			sendTo(client, numeric("404", client.getNickname(), target + " :Cannot send to channel"));
			return ;
		}
		sendPrivmsgToChannel(client, it->second, message);
	}
	else
	{
		Client *targetClient = findClientByNickname(target);
		if (!targetClient)
		{
			sendTo(client, numeric("401", client.getNickname(), target + " :No such nick/channel"));
			return ;
		}
		sendPrivmsgToUser(client, targetClient, message);
	}
}
