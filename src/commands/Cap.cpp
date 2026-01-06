#include "../../include/Server.hpp"

void	Server::handleCap(Client& client, const Command& cmd)
{
	if (cmd.args.empty())
		return;

	const std::string &subcmd = cmd.args[0];

	if (subcmd == "LS")
		sendTo(client, "CAP * LS :\r\n");
	else if (subcmd == "END") {}
}
