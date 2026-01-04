#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "common.hpp"

class Server
{
public:
	Server(int port, const std::string &password);
	~Server();

	void	run();

private:
	void	setupListener(int port);
	void	acceptNewClients();
	void	handleClientEvent(int fd, short events);
	void	removeClient(int fd);
	void	addPollFd(int fd, short events);
	void	removePollFd(int fd);

	int							m_listener;
	std::string					m_password;
	std::vector<struct pollfd>	m_pollfds;
	std::map<int, Client *>		m_clients;
};

#endif // SERVER_HPP
