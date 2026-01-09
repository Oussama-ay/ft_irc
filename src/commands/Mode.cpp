#include "../../include/Server.hpp"

struct ModeState
{
	Channel*		channel;
	Client&			client;
	const Command&	cmd;
	size_t			argIndex;
	char			currentSign;
	char			lastSignAppended;
	std::string		appliedModes;
	std::vector<std::string> appliedParams;

	ModeState(Channel* ch, Client& cl, const Command& command)
		: channel(ch), client(cl), cmd(command), argIndex(2), currentSign(0), lastSignAppended(0), appliedModes(), appliedParams() {}
};

static bool	isPositiveNumber(const std::string& s)
{
	if (s.empty())
		return false;
	for (size_t i = 0; i < s.size(); ++i)
		if (!isdigit(s[i]))
			return false;
	return true;
}

static void	appendChange(ModeState& st, char modeChar, const std::string* param = NULL)
{
	if (st.lastSignAppended != st.currentSign)
	{
		st.appliedModes += st.currentSign;
		st.lastSignAppended = st.currentSign;
	}
	st.appliedModes += modeChar;
	if (param)
		st.appliedParams.push_back(*param);
}

void	Server::handleMode(Client& client, const Command& cmd)
{
	// MODE <channel> [<modestring> [<params>...]]
	if (!client.isRegistered())
	{
		sendTo(client, numeric("451", client.getNickname(), ":You have not registered"));
		return;
	}

	if (cmd.args.size() < 1)
	{
		sendTo(client, numeric("461", client.getNickname(), "MODE :Not enough parameters"));
		return;
	}

	const std::string& target = cmd.args[0];
	if (target.empty() || target[0] != '#')
	{
		sendTo(client, numeric("403", client.getNickname(), target + " :No such channel"));
		return;
	}

	std::map<std::string, Channel *>::iterator it = m_channels.find(target);
	if (it == m_channels.end())
	{
		sendTo(client, numeric("403", client.getNickname(), target + " :No such channel"));
		return;
	}
	Channel* channel = it->second;

	if (!channel->hasMember(&client))
	{
		sendTo(client, numeric("442", client.getNickname(), channel->getName() + " :You're not on that channel"));
		return;
	}

	// Query current modes
	if (cmd.args.size() == 1)
	{
		std::string modeStr = "+";
		std::vector<std::string> params;
		if (channel->isInviteOnly())
			modeStr += "i";
		if (channel->isTopicProtected())
			modeStr += "t";
		if (channel->hasKey())
		{
			modeStr += "k";
			params.push_back(channel->getKey());
		}
		if (channel->hasUserLimit())
		{
			modeStr += "l";
			std::stringstream ss;
			ss << channel->getUserLimit();
			params.push_back(ss.str());
		}
		std::string payload = channel->getName() + " " + modeStr;
		for (size_t i = 0; i < params.size(); ++i)
			payload += " " + params[i];
		sendTo(client, numeric("324", client.getNickname(), payload));
		return;
	}

	if (!channel->isOperator(&client))
	{
		sendTo(client, numeric("482", client.getNickname(), channel->getName() + " :You're not channel operator"));
		return;
	}

	const std::string& modeSeq = cmd.args[1];
	if (modeSeq.empty())
	{
		sendTo(client, numeric("461", client.getNickname(), "MODE :Not enough parameters"));
		return;
	}

	ModeState st(channel, client, cmd);

	for (size_t i = 0; i < modeSeq.size(); ++i)
	{
		char c = modeSeq[i];
		if (c == '+' || c == '-')
		{
			st.currentSign = c;
			continue;
		}
		if (st.currentSign == 0)
		{
			sendTo(client, numeric("472", client.getNickname(), std::string(1, c) + " :is unknown mode char to me"));
			return;
		}

		switch (c)
		{
			case 'i':
					channel->setInviteOnly(st.currentSign == '+');
					appendChange(st, 'i');
				break;
			case 't':
					channel->setTopicProtected(st.currentSign == '+');
					appendChange(st, 't');
				break;
			case 'k':
				if (st.currentSign == '+')
				{
					if (st.argIndex >= cmd.args.size())
					{
						sendTo(client, numeric("461", client.getNickname(), "MODE :Not enough parameters"));
						return;
					}
					channel->setKey(cmd.args[st.argIndex]);
					appendChange(st, 'k', &cmd.args[st.argIndex]);
					++st.argIndex;
				}
				else
				{
					channel->clearKey();
					appendChange(st, 'k');
				}
				break;
			case 'o':
				if (st.argIndex >= cmd.args.size())
				{
					sendTo(client, numeric("461", client.getNickname(), "MODE :Not enough parameters"));
					return;
				}
				{
					std::string targetNick = cmd.args[st.argIndex++];
					Client* targetClient = findClientByNickname(targetNick);
					if (!targetClient)
					{
						sendTo(client, numeric("401", client.getNickname(), targetNick + " :No such nick/channel"));
						break;
					}
					if (!channel->hasMember(targetClient))
					{
						sendTo(client, numeric("441", client.getNickname(), targetNick + " " + channel->getName() + " :They aren't on that channel"));
						break;
					}
						if (st.currentSign == '+')
							channel->addOperator(targetClient);
						else
							channel->removeOperator(targetClient);
						appendChange(st, 'o', &targetNick);
				}
				break;
			case 'l':
				if (st.currentSign == '+')
				{
					if (st.argIndex >= cmd.args.size())
					{
						sendTo(client, numeric("461", client.getNickname(), "MODE :Not enough parameters"));
						return;
					}
					const std::string& limitStr = cmd.args[st.argIndex];
					if (!isPositiveNumber(limitStr))
					{
						sendTo(client, numeric("461", client.getNickname(), "MODE :Not enough parameters"));
						return;
					}
					size_t limit = static_cast<size_t>(atoi(limitStr.c_str()));
					channel->setUserLimit(limit);
						appendChange(st, 'l', &limitStr);
						++st.argIndex;
				}
				else
				{
					channel->clearUserLimit();
						appendChange(st, 'l');
				}
				break;
			default:
				sendTo(client, numeric("472", client.getNickname(), std::string(1, c) + " :is unknown mode char to me"));
				return;
		}
	}

	if (st.appliedModes.empty())
		return;

	std::string modeMsg = ":" + makePrefix(client) + " MODE " + channel->getName() + " " + st.appliedModes;
	for (size_t i = 0; i < st.appliedParams.size(); ++i)
		modeMsg += " " + st.appliedParams[i];
	modeMsg += "\r\n";
	broadcast(channel, modeMsg);
}
