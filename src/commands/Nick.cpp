#include "../../include/Server.hpp"

bool	Server::isNicknameInUse(Client& client, const std::string& inputNick)
{
	std::map<std::string, Client *>::iterator it = m_nicknames.find(inputNick);
	return (it != m_nicknames.end() && it->second != &client);
}

static bool	isValidNickname(const std::string& nickname)
{
	size_t	size(nickname.size());

	if (size == 0 || size > 9)
		return false;
	for (size_t i = 0; i < size; i++)
		if (!isalnum(nickname[i]) && nickname[i] != '_' && nickname[i] != '-')
			return false;
	return true;
}

void	Server::handleNickname(Client& client, const Command& cmd)
{
	std::string inputNick;
	std::string currentNick;

	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("431", client.getNickname(), ":No nickname given"));
		return ;
	}
	if (!client.isPassOk())
	{
		sendTo(client, numeric("464", client.getNickname(), ":Password incorrect"));
		return ;
	}
	inputNick = cmd.args[0];
	currentNick = client.getNickname();
	if (!currentNick.empty() && inputNick == currentNick)
		return;
	if (!isValidNickname(inputNick))
	{
		sendTo(client, numeric("432", client.getNickname(), inputNick + " :Erroneous nickname"));
		return ;
	}
	if (isNicknameInUse(client, inputNick))
	{
		sendTo(client, numeric("433", client.getNickname(), inputNick + " :Nickname is already in use"));
		return ;
	}
	if (!currentNick.empty())
		m_nicknames.erase(currentNick);
	client.setNickname(inputNick);
	m_nicknames[inputNick] = &client;
	client.setNickOk(true);
	checkRegistration(client);
}
