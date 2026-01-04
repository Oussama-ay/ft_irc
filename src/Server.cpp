#include "../include/Server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <cstdlib>

Server::Server(int port, const std::string &password)
	: m_listener(-1), m_password(password), m_pollfds(), m_clients()
{
	setupListener(port);
}

Server::~Server()
{
	if (m_listener != -1)
		close(m_listener);
	for (std::map<int, Client *>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
}

void	Server::setupListener(int port)
{
	m_listener = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listener < 0)
		throw std::runtime_error("socket() failed");

	int on = 1;
	if (setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		throw std::runtime_error("setsockopt() failed");

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (bind(m_listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind() failed");

	if (listen(m_listener, 32) < 0)
		throw std::runtime_error("listen() failed");

	// set non-blocking
	int flags = fcntl(m_listener, F_GETFL, 0);
	if (flags == -1)
		flags = 0;
	if (fcntl(m_listener, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl() failed");

	// add listener to pollfds
	addPollFd(m_listener, POLLIN);
	std::cout << "Server listening on port " << ntohs(addr.sin_port) << std::endl;
}

void	Server::addPollFd(int fd, short events)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;
	m_pollfds.push_back(pfd);
}

void	Server::removePollFd(int fd)
{
	for (std::vector<struct pollfd>::iterator it = m_pollfds.begin(); it != m_pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			m_pollfds.erase(it);
			return;
		}
	}
}

void	Server::acceptNewClients()
{
	while (true)
	{
		struct sockaddr_in cli_addr;
		socklen_t len = sizeof(cli_addr);
		int client_fd = accept(m_listener, (struct sockaddr *)&cli_addr, &len);
		if (client_fd < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN) // This is if there are no more clients waiting to be accepted right now
				break ;
			throw std::runtime_error("accept() failed");
			break ;
		}

		// set non-blocking
		int flags = fcntl(client_fd, F_GETFL, 0);
		if (flags == -1)
			flags = 0;
		fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

		Client *c = new Client(client_fd);
		m_clients[client_fd] = c;
		addPollFd(client_fd, POLLIN);

		std::cout << "Accepted client fd=" << client_fd << " from " << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port) << "\n";
		send(client_fd, "Welcome to ft_irc\r\n", 19, 0);
	}
}

void	Server::removeClient(int fd)
{
	std::map<int, Client *>::iterator it = m_clients.find(fd);
	if (it != m_clients.end())
	{
		delete it->second;
		m_clients.erase(it);
	}
	removePollFd(fd);
	close(fd);
	std::cout << "Closed client fd=" << fd << "\n";
}

void Server::handleClientEvent(int fd, short events)
{
	if (events & (POLLHUP | POLLERR | POLLNVAL)) // for handling abnormal or terminal socket states
		removeClient(fd);
	else if (events & POLLIN) // means there is data to read
	{
		char cmd[4096];
		ssize_t ret = recv(fd, cmd, sizeof(cmd) - 1, 0);
		if (ret <= 0)
		{
			removeClient(fd);
			return ;
		}
		cmd[ret] = '\0';
		Client *ptr = m_clients[fd];
		ptr->appendToBuffer(cmd);
		// parce here the cmd
			// !!
			// !!
		// and execute
		ptr->clearBuffer();
	}
}

void	Server::run()
{
	size_t	n, i;

	while (true)
	{
		int nfds = poll(&m_pollfds[0], m_pollfds.size(), -1);
		if (nfds < 0)
			throw std::runtime_error("poll() failed");
		n = m_pollfds.size();
		for (i = 0; i < n; i++)
		{
			struct pollfd p = m_pollfds[i];
			if (p.revents == 0)
				continue;
			if (p.fd == m_listener)
				acceptNewClients();
			else
				handleClientEvent(p.fd, p.revents);
		}
	}
}
