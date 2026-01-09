#include "../../include/Server.hpp"

std::string	Server::makePrefix(const Client &client) const
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
	std::string user = client.getUsername().empty() ? "unknown" : client.getUsername();
	return (nick + "!" + user + "@" + m_hostname);
}

void Server::broadcast(const Channel* channel, const std::string& message, const Client* exclude)
{
	const std::set<Client*>& 			members = channel->getMembers();
	std::set<Client*>::const_iterator	it = members.begin();

	while (it != members.end())
	{
		if (exclude == NULL || *it != exclude)
			sendTo(*(*it), message);
		it++;
	}
}

void	Server::findOrCreateChannel(Client& client, const std::string& channelName, const std::string& keyArg)
{
	std::map<std::string, Channel *>::iterator	it;
	std::string 								joinMsg;
	Channel*									newChannel;

	joinMsg = ":" + makePrefix(client) + " JOIN " + channelName + "\r\n";
	it = m_channels.find(channelName);
	if (it != m_channels.end())
	{
		Channel* channel = it->second;
		if (channel->isInviteOnly())
		{
			sendTo(client, numeric("473", client.getNickname(), channelName + " :Cannot join channel (+i)"));
			return;
		}
		if (channel->hasKey() && channel->getKey() != keyArg)
		{
			sendTo(client, numeric("475", client.getNickname(), channelName + " :Cannot join channel (+k)"));
			return;
		}
		if (channel->hasUserLimit() && channel->getMemberCount() >= channel->getUserLimit())
		{
			sendTo(client, numeric("471", client.getNickname(), channelName + " :Cannot join channel (+l)"));
			return;
		}
		channel->addMember(&client);
		broadcast(channel, joinMsg, NULL);
		if (channel->getTopic().empty())
			sendTo(client, numeric("331", client.getNickname(), channelName + " :No topic is set"));
		else
			sendTo(client, numeric("332", client.getNickname(), channelName + " :" + channel->getTopic()));
	}
	else
	{
		newChannel = new Channel(channelName);
		newChannel->addMember(&client);
		newChannel->addOperator(&client);
		m_channels[channelName] = newChannel;
		sendTo(client, joinMsg);
		sendTo(client, numeric("331", client.getNickname(), channelName + " :No topic is set"));
	}
}

void	Server::handleJoin(Client& client, const Command& cmd)
{
	std::string		channelName;
	std::string		topic;
	size_t			size(cmd.args.size());

	if (!client.isRegistered())
	{
		sendTo(client, numeric("461", client.getNickname(), ":You have not registered"));
		return ;
	}
	if (size == 0)
	{
		sendTo(client, numeric("461", client.getNickname(), "JOIN :Not enough parameters"));
		return;
	}
	if (cmd.args[0].empty() || cmd.args[0][0] != '#')
	{
		sendTo(client, numeric("403", client.getNickname(), channelName + ":No such channel"));
		return ;
	}
	channelName = cmd.args[0];
	std::string keyArg;
	if (size > 1)
		keyArg = cmd.args[1];
	findOrCreateChannel(client, channelName, keyArg);

	const std::set<Client *>&	cl = m_channels[channelName]->getMembers();
	std::string		names("= " + channelName + " ");
	std::string		endMsg(channelName + " :End of /NAMES list");

	for (std::set<Client *>::iterator it = cl.begin(); it != cl.end(); ++it)
	{
		if (m_channels[channelName]->isOperator(*it))
			names += " @" + (*it)->getNickname();
		else
			names += " " + (*it)->getNickname();
	}
	sendTo(client, numeric("353", client.getNickname(), names));
	sendTo(client, numeric("366", client.getNickname(), endMsg));
}
