#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "../include/Client.hpp"

class Channel
{
private:
    std::string			m_name;
    std::set<Client *>	m_members;
    std::set<Client *>	m_operators;
    std::string			m_topic;

public:
    Channel(const std::string &name);
    ~Channel();
	
    const std::set<Client *>&	getMembers() const;
	const std::string&			getTopic() const;

    void addMember(Client *client);
    void removeMember(Client *client);
    bool hasMember(Client *client) const;
};

#endif
