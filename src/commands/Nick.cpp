#include "../../include/Server.hpp"

bool	Server::isNicknameInUse(const std::string& nickname) const
{
	return (m_nicknames.find(nickname) != m_nicknames.end());
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
	std::string Nick = cmd.args[0];

	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("461", client.getNickname(), "NICK :Not enough parameters"));
		return ;
	}
	if (!isValidNickname(Nick))
	{
		sendTo(client, numeric("432", client.getNickname(), Nick + " :Erroneous nickname"));
		return ;
	}
	if (isNicknameInUse(Nick))
	{
		sendTo(client, numeric("433", client.getNickname(), Nick + " :Nickname is already in use"));
		return;
	}
	client.setNickname(Nick);
	m_nicknames[Nick] = &client;
	client.setNickOk(true);
	checkRegistration(client);
}
