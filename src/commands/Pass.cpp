#include "../../include/Server.hpp"

void Server::handlePass(Client& client, const Command& cmd)
{
	if (client.isPassOk())
		return ;
	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("461", client.getNickname(), "PASS :Not enough parameters"));
		return ;
	}
	if (cmd.args[0] != m_password)
	{
		sendTo(client, numeric("464", client.getNickname(), ":Password incorrect"));
		return ;
	}
	client.setPassOk(true);
}
