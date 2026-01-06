#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "common.hpp"
#include "Channel.hpp"

class Server
{
public:
	Server(int port, const std::string &password);
	~Server();

	void	run();

private:
	typedef void (Server::*CommandHandler)(Client&, const Command&);

	std::map<std::string, CommandHandler>	commandMap;

	void	setupListener(int port);
	void	acceptNewClients();
	void	handleClientEvent(int fd, short events);
	void	removeClient(int fd);
	void	addPollFd(int fd, short events);
	void	removePollFd(int fd);
	struct pollfd	*findPollfd(int fd);

	void	handleReadable(Client &client);
	void	handleWritable(Client &client);
	void	sendTo(Client &client, const std::string &data);
	void	checkRegistration(Client &client);
	void 	execute(Client& client, const std::vector<Command>& input);
	std::string	numeric(const std::string &code, const std::string &nick, const std::string &msg) const;
	void	sendTopic(Client& client, const std::vector<std::string>& topic);

	void	handlePass(Client& client, const Command& cmd);
	void	handleNickname(Client& client, const Command& cmd);
	void	handleUsername(Client& client, const Command& cmd);
	void	handleJoin(Client& client, const Command& cmd);
	void	handlePing(Client &client, const Command &cmd);
	void	handleCap(Client& client, const Command& cmd);

	bool	isNicknameInUse(const std::string& nickname) const;
	void	broadcast(Channel* channel, const std::string& message);
	void	findOrCreateChannel(Client& client, const std::string& channelName, const std::string&	topic);
	std::string	makePrefix(const Client &client) const;

	int							m_listener;
	std::string					m_hostname;
	std::string					m_password;
	std::vector<struct pollfd>	m_pollfds;
	std::map<int, Client *>		m_clients;
	std::map<std::string, Channel *>		m_channels;
};

#endif // SERVER_HPP
