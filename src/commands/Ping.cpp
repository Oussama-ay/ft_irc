#include "../../include/Server.hpp"

void	Server::handlePing(Client &client, const Command &cmd)
{
	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("461", client.getNickname(), "PING :Not enough parameters"));
		return;
	}
	sendTo(client, ":" + m_hostname + " PONG " + m_hostname + " :" + cmd.args[0] + "\r\n");
}
