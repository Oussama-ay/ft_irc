#include "../../include/Server.hpp"

void Server::handleUsername(Client& client, const Command& cmd)
{
	if (cmd.args.size() < 4)
	{
		sendTo(client, numeric("461", client.getNickname(), "USER :Not enough parameters"));
		return ;
	}
	if (client.isRegistered())
	{
		sendTo(client, numeric("462", client.getNickname(), ":Already registered"));
		return ;
	}
	if (!client.isPassOk())
	{
		sendTo(client, numeric("462", client.getNickname(), ":Enter Password first"));
		return ;
	}
	client.setUsername(cmd.args[0]);
	client.setRealname(cmd.args[3]);
	client.setUserOk(true);
	checkRegistration(client);
}
