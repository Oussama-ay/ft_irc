#include "../../include/Server.hpp"

std::string	Server::makePrefix(const Client &client) const
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
	std::string user = client.getUsername().empty() ? "unknown" : client.getUsername();
	return (nick + "!" + user + "@" + m_hostname);
}

void Server::broadcast(Channel* channel, const std::string& message)
{
	const std::set<Client*>& 			members = channel->getMembers();
	std::set<Client*>::const_iterator	it = members.begin();

	while (it != members.end())
	{
		sendTo(*(*it), message);
		it++;
	}
}

void	Server::findOrCreateChannel(Client& client, const std::string& channelName)
{
	std::map<std::string, Channel *>::iterator	it;
	std::string 								joinMsg;
	Channel*									newChannel;

	joinMsg = ":" + makePrefix(client) + " JOIN " + channelName + "\r\n";
	it = m_channels.find(channelName);
	if (it != m_channels.end())
	{
		it->second->addMember(&client);
		broadcast(it->second, joinMsg);
		if (it->second->getTopic().empty())
			sendTo(client, numeric("331", client.getNickname(), channelName + " :No topic is set"));
		else
			sendTo(client, numeric("332", client.getNickname(), channelName + " :" + it->second->getTopic()));
	}
	else
	{
		newChannel = new Channel(channelName);
		client.setOperator(true);
		newChannel->addMember(&client);
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
	findOrCreateChannel(client, channelName);

	// send NAMES list
	const std::set<Client *>&	cl = m_channels[channelName]->getMembers();
	std::string		names("= " + channelName + " ");
	std::string		endMsg(channelName + " :End of /NAMES list");

	for (std::set<Client *>::iterator it = cl.begin(); it != cl.end(); ++it)
	{
		if ((*it)->isOperator())
			names += ":@" + (*it)->getNickname();
		else
			names += " " + (*it)->getNickname();
	}
	sendTo(client, numeric("353", client.getNickname(), names));
	sendTo(client, numeric("366", client.getNickname(), endMsg));
}
