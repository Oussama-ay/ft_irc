#include "../../include/Server.hpp"

void	Server::kickFromChannel(Client& client, const std::string& channelName, const std::string& targetNick, const std::string& reason)
{
	Client*		targetClient;
	Channel*	channel;
	std::string	msg;

	if (m_channels.find(channelName) == m_channels.end())
	{
		sendTo(client, numeric("403", client.getNickname(), channelName + " :No such channel"));
		return;
	}
	channel = m_channels[channelName];
	if (!channel->isOperator(&client))
	{
		sendTo(client, numeric("482", client.getNickname(), channel->getName() + " :You're not channel operator"));
		return;
	}
	if (!isNicknameInUse(targetNick))
	{
		sendTo(client, numeric("401", client.getNickname(), targetNick + " :No such nick/channel"));
		return;
	}

	targetClient = m_nicknames[targetNick];
	msg = ":" + makePrefix(client) + " KICK " + channel->getName() + " " + targetNick + " :" + reason + "\r\n";
	broadcastAndRemoveMember(client, channel, channelName, targetClient, msg);
}

void	Server::handleKICK(Client& client, const Command& cmd)
{
	if (!client.isRegistered())
	{
		sendTo(client, numeric("451", client.getNickname(), ":You have not registered"));
		return;
	}
	if (cmd.args.size() < 2)
	{
		sendTo(client, numeric("461", client.getNickname(), "KICK :Not enough parameters"));
		return;
	}

	std::string	channelName = cmd.args[0];
	std::string	targetNick = cmd.args[1];
	std::string	reason = (cmd.args.size() > 2) ? cmd.args[2] : "Bye";

	kickFromChannel(client, channelName, targetNick, reason);
}
