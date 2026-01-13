#include "../../include/Server.hpp"

void	Server::sendNoticeToTarget(Client& client, const std::string& target, const std::string& message)
{
	std::string									notice;
	std::map<std::string, Channel *>::iterator	it;
	Client										*targetClient;

	if (target.empty())
		return ;
	if (target[0] == '#')
	{
		it = m_channels.find(target);
		if (it == m_channels.end())
		{
			sendTo(client, numeric("403", client.getNickname(), target + " :No such channel"));
			return ;
		}
		if (!it->second->hasMember(&client))
		{
			sendTo(client, numeric("404", client.getNickname(), target + " :Cannot send to channel"));
			return ;
		}
		notice = ":" + makePrefix(client) + " NOTICE " + target + " :" + message + "\r\n";
		broadcast(it->second, notice, &client);
	}
	else
	{
		targetClient = findClientByNickname(target);
		if (!targetClient)
		{
			sendTo(client, numeric("401", client.getNickname(), target + " :No such nick/channel"));
			return ;
		}
		notice = ":" + makePrefix(client) + " NOTICE " + targetClient->getNickname() + " :" + message + "\r\n";
		sendTo(*targetClient, notice);
	}
}

void	Server::handleNotice(Client& client, const Command& cmd)
{
	std::string					message;
	std::vector<std::string>	targets;

	if (!client.isRegistered())
	{
		sendTo(client, numeric("451", client.getNickname(), ":You have not registered"));
		return ;
	}
	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("411", client.getNickname(), "NOTICE :No recipient given"));
		return ;
	}
	if (cmd.args.size() < 2)
	{
		sendTo(client, numeric("412", client.getNickname(), "NOTICE :No text to send"));
		return ;
	}

	for (size_t i = 1; i < cmd.args.size(); ++i)
	{
		if (i > 1)
			message += " ";
		message += cmd.args[i];
	}
	if (message.empty())
		return ;

	getChannelNames(cmd.args[0], targets);
	for (size_t i = 0; i < targets.size(); ++i)
		sendNoticeToTarget(client, targets[i], message);
}
