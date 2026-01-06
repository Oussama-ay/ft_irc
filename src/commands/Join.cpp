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

void	Server::findOrCreateChannel(Client& client, const std::string& channelName, const std::string& topic)
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
	}
	else
	{
		newChannel = new Channel(channelName, topic);
		newChannel->addMember(&client);
		m_channels[channelName] = newChannel;
		sendTo(client, joinMsg);
	}
}

void	Server::handleJoin(Client& client, const Command& cmd)
{
	if (!client.isRegistered())
	{
		// std::cout << "You must complete registration first (PASS/NICK/USER)\r\n";
		return ;
	}
	if (cmd.args.empty())
	{
		sendTo(client, numeric("461", client.getNickname(), "JOIN :Not enough parameters"));
		return;
	}
	const	std::string		channelName(cmd.args[0]);
	if (channelName.empty() || channelName[0] != '#')
	{
		sendTo(client, numeric("403", client.getNickname(), channelName + " :No such channel"));
		return ;
	}
	std::string		topic;
	size_t	i, size;
	size = cmd.args.size();
	if (size > 1)
	{
		for (i = 2; i < size; i++)
			topic += " " + cmd.args[i];
	}
	findOrCreateChannel(client, channelName, topic);
	// send topic
	// if (cmd.args.size() > 1)
		// sendTopic(client, cmd.args);

	// send NAMES list
}
