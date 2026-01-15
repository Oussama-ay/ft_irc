#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "Client.hpp"

class Channel
{
private:
    std::string			m_name;
    std::set<Client *>	m_members;
	std::set<Client *>	m_invited;
    std::set<Client *>	m_operators;
    std::string			m_topic;
	bool				m_inviteOnly;
	bool				m_topicOpsOnly;
	bool				m_hasKey;
	std::string			m_key;
	bool				m_hasUserLimit;
	size_t				m_userLimit;

public:
    Channel(const std::string &name);
    ~Channel();

    const std::set<Client *>&	getMembers() const;
	const std::string&			getTopic() const;
    const std::string&          getName() const;
	bool						isInviteOnly() const;
	bool						isTopicProtected() const;
	bool						hasKey() const;
	const std::string&			getKey() const;
	bool						hasUserLimit() const;
	size_t						getUserLimit() const;
	size_t						getMemberCount() const;
	bool						isOperator(const Client* client) const;
	bool			isInvited(const Client* client) const;

	void					setTopic(std::string);
	void					setInviteOnly(bool enabled);
	void					setTopicProtected(bool enabled);
	void					setKey(const std::string& key);
	void					clearKey();
	void					setUserLimit(size_t limit);
	void					clearUserLimit();
	void					addOperator(Client* client);
	void					removeOperator(Client* client);
	void			addInvite(Client* client);
	void			removeInvite(Client* client);

    void addMember(Client *client);
    void removeMember(Client *client);
    bool hasMember(Client *client) const;

};

#endif
