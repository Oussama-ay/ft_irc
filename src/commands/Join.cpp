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

bool Server::broadcastAndRemoveMember(Client& sender, Channel* channel, const std::string& channelName, Client* clientToRemove, const std::string& message)
{
	if (!channel->hasMember(clientToRemove))
	{
		sendTo(sender, numeric("442", sender.getNickname(), channel->getName() + " :You're not on that channel"));
		return false;
	}

	broadcast(channel, message);
	channel->removeMember(clientToRemove);
	
	if (channel->getMembers().empty())
	{
		delete channel;
		m_channels.erase(channelName);
	}
	return true;
}

void	Server::joinSingleChannel(Client& client, const std::string& channelName, const std::string& keyArg)
{
	std::string names;

	if (channelName.empty() || channelName[0] != '#')
	{
		sendTo(client, numeric("403", client.getNickname(), channelName + " :No such channel"));
		return;
	}
	findOrCreateChannel(client, channelName, keyArg);
	if (m_channels.find(channelName) != m_channels.end() && m_channels[channelName]->hasMember(&client))
	{
		const std::set<Client *>& cl = m_channels[channelName]->getMembers();
		names = "= " + channelName + " ";

		for (std::set<Client *>::iterator it = cl.begin(); it != cl.end(); ++it)
		{
			if (m_channels[channelName]->isOperator(*it))
				names += " @" + (*it)->getNickname();
			else
				names += " " + (*it)->getNickname();
		}
		sendTo(client, numeric("353", client.getNickname(), names));
		sendTo(client, numeric("366", client.getNickname(), channelName + " :End of /NAMES list"));
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
		bool invited = channel->isInvited(&client);
		if (channel->isInviteOnly() && !invited)
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
		if (invited)
			channel->removeInvite(&client);
		broadcast(channel, joinMsg);
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
	size_t			size(cmd.args.size());

	if (!client.isRegistered())
	{
		sendTo(client, numeric("451", client.getNickname(), ":You have not registered"));
		return ;
	}
	if (size == 0)
	{
		sendTo(client, numeric("461", client.getNickname(), "JOIN :Not enough parameters"));
		return;
	}
	if (cmd.args[0].empty())
	{
		sendTo(client, numeric("461", client.getNickname(), "JOIN :Not enough parameters"));
		return;
	}

	std::vector<std::string> channels;
	getChannelNames(cmd.args[0], channels);

	std::vector<std::string> keys;
	if (size > 1)
		getChannelNames(cmd.args[1], keys);

	size = channels.size();
	for (size_t i = 0; i < size; i++)
	{
		std::string keyArg = (i < keys.size()) ? keys[i] : "";
		joinSingleChannel(client, channels[i], keyArg);
	}
}
