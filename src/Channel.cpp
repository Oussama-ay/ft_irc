#include "../include/Channel.hpp"

Channel::Channel(const std::string& name, const std::string& topic) : m_name(name), m_topic(topic) {}

Channel::~Channel() {}

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
