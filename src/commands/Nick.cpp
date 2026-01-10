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
	std::string nickname;

	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("431", client.getNickname(), ":No nickname given"));
		return ;
	}
	nickname = cmd.args[0];
	if (!isValidNickname(nickname))
	{
		sendTo(client, numeric("432", client.getNickname(), nickname + " :Erroneous nickname"));
		return ;
	}
	if (isNicknameInUse(nickname))
	{
		sendTo(client, numeric("433", client.getNickname(), nickname + " :Nickname is already in use"));
		return;
	}
	client.setNickname(nickname);
	m_nicknames[nickname] = &client;
	client.setNickOk(true);
	checkRegistration(client);
}
