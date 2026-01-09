#include "../../include/Server.hpp"

void	Server::viewOrWriteTopic(Client& client, const Channel* channel)
{
	const	std::string		chName(channel->getName());
	const std::string&		chTopic(channel->getTopic());

	if (chTopic.empty())
		sendTo(client, numeric("331", client.getNickname(), chName + " :No topic is set"));
	else
		sendTo(client, numeric("332", client.getNickname(), chName + " :" + chTopic));
}

void	Server::checkPermisionAndBroadcast(Client& client, const std::vector<std::string>& args)
{
	size_t		size(args.size());
	std::map<std::string, Channel *>::iterator it = m_channels.find(args[0]);
	if (it == m_channels.end())
	{
		sendTo(client, numeric("403", client.getNickname(), args[0] + " :No such channel"));
		return ;
	}
	Channel* channel = it->second;
	std::string	newTopic;

	if (channel->isTopicProtected() && !channel->isOperator(&client))
	{
		sendTo(client, numeric("482", client.getNickname(), channel->getName() + " :You're not channel operator"));
		return ;
	}
	for (size_t i = 1; i < size; i++)
	{
		if (i > 1)
			newTopic += " ";
		newTopic += args[i];
	}
	channel->setTopic(newTopic);
	newTopic = numeric("332", client.getNickname(), channel->getName() + " :" + newTopic);
	broadcast(channel, newTopic);
}

void	Server::handleTopic(Client& client, const Command& cmd)
{
	size_t		size(cmd.args.size());

	if (!client.isRegistered())
	{
		sendTo(client, numeric("461", client.getNickname(), ":You have not registered"));
		return ;
	}
	if (size < 1)
	{
		sendTo(client, numeric("461", client.getNickname(), "TOPIC :Not enough parameters"));
		return;
	}
	std::map<std::string, Channel *>::iterator it = m_channels.find(cmd.args[0]);
	if (it == m_channels.end())
	{
		sendTo(client, numeric("403", client.getNickname(), cmd.args[0] + " :No such channel"));
		return ;
	}
	Channel* channel = it->second;
	if (!channel->hasMember(&client))
	{
		sendTo(client, numeric("442", client.getNickname(), channel->getName() + " :You're not on that channel"));
		return ;
	}
	if (size == 1)
		viewOrWriteTopic(client, channel);
	else
		checkPermisionAndBroadcast(client, cmd.args);
}
