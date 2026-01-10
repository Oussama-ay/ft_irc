#include "../include/Channel.hpp"

Channel::Channel(const std::string& name)
	: m_name(name), m_inviteOnly(false), m_topicOpsOnly(false), m_hasKey(false), m_key(), m_hasUserLimit(false), m_userLimit(0) {}

Channel::~Channel() {}

const std::string&			Channel::getTopic() const { return m_topic; }

bool					Channel::isInviteOnly() const { return m_inviteOnly; }
bool					Channel::isTopicProtected() const { return m_topicOpsOnly; }
bool					Channel::hasKey() const { return m_hasKey; }
const std::string&			Channel::getKey() const { return m_key; }
bool					Channel::hasUserLimit() const { return m_hasUserLimit; }
size_t					Channel::getUserLimit() const { return m_userLimit; }
size_t					Channel::getMemberCount() const { return m_members.size(); }
bool					Channel::isOperator(const Client* client) const { return m_operators.find(const_cast<Client*>(client)) != m_operators.end(); }
bool				Channel::isInvited(const Client* client) const { return m_invited.find(const_cast<Client*>(client)) != m_invited.end(); }

void Channel::addMember(Client* client)
{
	m_members.insert(client);
}

void Channel::removeMember(Client* client)
{
	m_operators.erase(client);
	m_members.erase(client);
}

bool Channel::hasMember(Client* client) const
{
	return m_members.find(client) != m_members.end();
}

const std::set<Client*>& Channel::getMembers() const
{
	return m_members;
}

const std::string	&Channel::getName() const
{
	return m_name;
}

void	Channel::setTopic(std::string topic)
{
	m_topic = topic;
}

void	Channel::setInviteOnly(bool enabled)
{
	m_inviteOnly = enabled;
}

void	Channel::setTopicProtected(bool enabled)
{
	m_topicOpsOnly = enabled;
}

void	Channel::setKey(const std::string& key)
{
	m_hasKey = true;
	m_key = key;
}

void	Channel::clearKey()
{
	m_hasKey = false;
	m_key.clear();
}

void	Channel::setUserLimit(size_t limit)
{
	m_hasUserLimit = true;
	m_userLimit = limit;
}

void	Channel::clearUserLimit()
{
	m_hasUserLimit = false;
	m_userLimit = 0;
}

void	Channel::addOperator(Client* client)
{
	m_operators.insert(client);
}

void	Channel::removeOperator(Client* client)
{
	m_operators.erase(client);
}

void	Channel::addInvite(Client* client)
{
	m_invited.insert(client);
}

void	Channel::removeInvite(Client* client)
{
	m_invited.erase(client);
}
