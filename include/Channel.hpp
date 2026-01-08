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
	char				m_mode;

public:
    Channel(const std::string &name);
    ~Channel();
	
    const std::set<Client *>&	getMembers() const;
	const std::string&			getTopic() const;
    const std::string&          getName() const;
    const char&					getMode() const;

	void						setTopic(std::string);
	void						setMode(char);

    void addMember(Client *client);
    void removeMember(Client *client);
    bool hasMember(Client *client) const;

};

#endif
