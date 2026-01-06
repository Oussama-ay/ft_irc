#include "../../include/Server.hpp"

void Server::handlePass(Client& client, const Command& cmd)
{
	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("461", client.getNickname(), "PASS :Not enough parameters"));
		return ;
	}
	if (client.isRegistered())
	{
		sendTo(client, numeric("462", client.getNickname(), ":Already registered"));
		return ;
	}
	if (cmd.args[0] != m_password)
	{
		sendTo(client, numeric("464", client.getNickname(), ":Password incorrect"));
		return ;
	}
	client.setPassOk(true);
}
