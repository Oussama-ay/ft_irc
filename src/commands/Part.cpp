#include "../../include/Server.hpp"

void	Server::getNames(std::string	input, std::vector<std::string>& container)
{
	std::string::size_type del;
	std::string		token;

	while ((del = input.find(',')) != std::string::npos)
	{
		token = input.substr(0, del);
		if (!token.empty())
			container.push_back(token);
		input = input.substr(del + 1);
	}
	token = input;
	if (!token.empty())
		container.push_back(token);
}

void	Server::partFromChannel(Client& client, const std::string& channelName, const std::string& reason)
{
	Channel*		channel;
	std::string		msg;

	if (m_channels.find(channelName) == m_channels.end())
	{
		sendTo(client, numeric("403", client.getNickname(), channelName + " :No such channel"));
		return;
	}

	channel = m_channels[channelName];
	msg = ":" + makePrefix(client) + " PART " + channel->getName() + " :" + reason + "\r\n";
	broadcastAndRemoveMember(client, channel, channelName, &client, msg);
}

void	Server::handlePart(Client& client, const Command& cmd)
{
	std::vector<std::string>	channels;
	std::string					reason;

	if (!client.isRegistered())
	{
		sendTo(client, numeric("451", client.getNickname(), ":You have not registered"));
		return;
	}
	if (cmd.args.empty())
	{
		sendTo(client, numeric("461", client.getNickname(), "PART :Not enough parameters"));
		return;
	}
	getNames(cmd.args[0], channels);

	reason = (cmd.args.size() > 1) ? cmd.args[1] : "Leaving";
	for (size_t i = 0; i < channels.size(); i++)
		partFromChannel(client, channels[i], reason);
}
