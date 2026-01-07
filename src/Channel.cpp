#include "../include/Channel.hpp"

Channel::Channel(const std::string& name) : m_name(name) {}

Channel::~Channel() {}

const std::string&			Channel::getTopic() const { return m_topic; }

void Channel::addMember(Client* client)
{
	m_members.insert(client);
}

void Channel::removeMember(Client* client)
{
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
