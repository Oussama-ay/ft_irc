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
			return ;
		if (!it->second->hasMember(&client))
			return ;
		notice = ":" + makePrefix(client) + " NOTICE " + target + " :" + message + "\r\n";
		broadcast(it->second, notice, &client);
	}
	else
	{
		targetClient = findClientByNickname(target);
		if (!targetClient)
			return ;
		notice = ":" + makePrefix(client) + " NOTICE " + targetClient->getNickname() + " :" + message + "\r\n";
		sendTo(*targetClient, notice);
	}
}

void	Server::handleNotice(Client& client, const Command& cmd)
{
	std::string					message;
	std::vector<std::string>	targets;
	size_t						size;

	if (!client.isRegistered() || cmd.args.size() < 2)
		return ;

	for (size_t i = 1; i < cmd.args.size(); ++i)
	{
		if (i > 1)
			message += " ";
		message += cmd.args[i];
	}
	if (message.empty())
		return ;

	getNames(cmd.args[0], targets);
	size = targets.size();
	for (size_t i = 0; i < size; ++i)
		sendNoticeToTarget(client, targets[i], message);
}
