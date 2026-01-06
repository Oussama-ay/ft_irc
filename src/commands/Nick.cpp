#include "../../include/Server.hpp"

bool	Server::isNicknameInUse(const std::string& nickname) const
{
	std::map<int, Client *>::const_iterator it(m_clients.begin());

	while (it != m_clients.end())
	{
		if (it->second->getNickname() == nickname)
			return (true);
		it++;
	}
	return (false);
}

static bool	isValidNickname(const std::string& nickname)
{
	size_t	size(nickname.size());

	if (size == 0 || size > 9)
		return (false);
	for (size_t i = 0; i < size; i++)
		if (!isalnum(nickname[i]) && nickname[i] != '_' && nickname[i] != '-')
			return (false);
	return (true);
}

void	Server::handleNickname(Client& client, const Command& cmd)
{
	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("461", client.getNickname(), "NICK :Not enough parameters"));
		return ;
	}
	std::string Nick = cmd.args[0];
	if (!isValidNickname(Nick))
	{
		sendTo(client, numeric("432", client.getNickname(), Nick + " :Erroneous nickname"));
		return ;
	}
	if (isNicknameInUse(cmd.args[0]))
	{
		sendTo(client, numeric("433", client.getNickname(), Nick + " :Nickname is already in use"));
		return;
	}
	client.setNickname(cmd.args[0]);
	client.setNickOk(true);
	checkRegistration(client);
}
