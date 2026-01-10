#include "../../include/Server.hpp"

void	Server::handleInvite(Client& client, const Command& cmd)
{
	if (!client.isRegistered())
	{
		sendTo(client, numeric("461", client.getNickname(), ":You have not registered"));
		return;
	}
	if (cmd.args.size() < 2)
	{
		sendTo(client, numeric("461", client.getNickname(), "INVITE :Not enough parameters"));
		return;
	}

	const std::string& targetNick = cmd.args[0];
	const std::string& channelName = cmd.args[1];

	std::map<std::string, Channel *>::iterator it = m_channels.find(channelName);
	if (it == m_channels.end())
	{
		sendTo(client, numeric("403", client.getNickname(), channelName + " :No such channel"));
		return;
	}

	Channel* channel = it->second;

	if (!channel->hasMember(&client))
	{
		sendTo(client, numeric("442", client.getNickname(), channelName + " :You're not on that channel"));
		return;
	}

	if (channel->isInviteOnly() && !channel->isOperator(&client))
	{
		sendTo(client, numeric("482", client.getNickname(), channelName + " :You're not channel operator"));
		return;
	}

	Client* targetClient = findClientByNickname(targetNick);
	if (!targetClient)
	{
		sendTo(client, numeric("401", client.getNickname(), targetNick + " :No such nick/channel"));
		return;
	}

	if (channel->hasMember(targetClient))
	{
		sendTo(client, numeric("443", client.getNickname(), targetNick + " " + channelName + " :is already on channel"));
		return;
	}

	channel->addInvite(targetClient);

	sendTo(client, numeric("341", client.getNickname(), targetNick + " " + channelName));

	std::string inviteMsg = ":" + makePrefix(client) + " INVITE " + targetNick + " :" + channelName + "\r\n";
	sendTo(*targetClient, inviteMsg);
}
