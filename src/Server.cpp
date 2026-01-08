#include "../include/Server.hpp"
#include "../include/Parser.hpp"

Server::Server(int port, const std::string &password)
	: m_listener(-1), m_password(password), m_pollfds(), m_clients()
{
	setupListener(port);
	commandMap["PASS"] = &Server::handlePass;
	commandMap["NICK"] = &Server::handleNickname;
	commandMap["USER"] = &Server::handleUsername;
	commandMap["JOIN"] = &Server::handleJoin;
	commandMap["PING"] = &Server::handlePing;
	commandMap["CAP"] = &Server::handleCap;
	commandMap["PRIVMSG"] = &Server::handlePrivmsg;
	commandMap["QUIT"] = &Server::handleQuit;
	commandMap["TOPIC"] = &Server::handleTopic;
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
	for (std::map<std::string, Channel *>::iterator it = m_channels.begin(); it != m_channels.end(); ++it)
	{
		delete it->second;
	}
}

void	Server::acceptNewClients()
{
	while (true)
	{
		struct sockaddr_in cli_addr;
		socklen_t len = sizeof(cli_addr);
		int clientm_fd = accept(m_listener, (struct sockaddr *)&cli_addr, &len);
		if (clientm_fd < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN) // This is if there are no more clients waiting to be accepted right now
				break ;
			throw std::runtime_error("accept() failed");
		}

		// set non-blocking
		fcntl(clientm_fd, F_SETFL, O_NONBLOCK);

		Client *c = new Client(clientm_fd);
		m_hostname = inet_ntoa(cli_addr.sin_addr);
		m_clients[clientm_fd] = c;
		addPollFd(clientm_fd, POLLIN);

		std::cout << "Accepted client fd=" << clientm_fd << " from " << m_hostname << ":" << ntohs(cli_addr.sin_port) << "\n";
	}
}

void	Server::removeClient(int fd)
{
	std::map<int, Client *>::iterator it = m_clients.find(fd);
	std::map<std::string, Client *>::iterator it2 = m_nicknames.find(it->second->getNickname());
	if (it != m_clients.end())
	{
		delete it->second;
		m_clients.erase(it);
		m_nicknames.erase(it2);
	}
	removePollFd(fd);
	close(fd);
	std::cout << "Closed client fd=" << fd << "\n";
}

void Server::handleClientEvent(int fd, short events)
{
	std::map<int, Client *>::iterator it;
	
	it = m_clients.find(fd);
	if (it == m_clients.end())
		return ;
	Client &client = *(it->second);
	if (events & (POLLHUP | POLLERR | POLLNVAL)) // for handling abnormal or terminal socket states
	{
		removeClient(fd);
		return ;
	}
	if (events & POLLIN)
		handleReadable(client);
	if (events & POLLOUT)
		handleWritable(client);
}

void	Server::handleReadable(Client &client)
{
	char	buffer[4096];
	ssize_t	ret;

	ret = recv(client.getFd(), buffer, sizeof(buffer), 0);
	if (ret <= 0)
	{
		removeClient(client.getFd());
		return ;
	}
	client.appendToRecv(std::string(buffer, ret));
	if(client.getRecvBuffer().find_first_of("\r\n") == std::string::npos)
			return ;
	std::vector<Command> cmds = Parser::processBuffer(client.getRecvBuffer());
	execute(client, cmds);
}

void	Server::handleWritable(Client &client)
{
	std::string &out = client.getSendBuffer();
	if (out.empty())
	{
		struct pollfd *p = findPollfd(client.getFd());
		if (p)
			p->events &= ~POLLOUT;
		return ;
	}
	ssize_t sent = send(client.getFd(), out.c_str(), out.size(), 0);
	if (sent < 0)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return ;
		removeClient(client.getFd());
		return ;
	}
	out.erase(0, sent);
	if (out.empty())
	{
		struct pollfd *p = findPollfd(client.getFd());
		if (p)
			p->events &= ~POLLOUT;
	}
}

void	Server::run()
{
	size_t			i;
	struct pollfd	*p;

	while (true)
	{
		int nfds = poll(&m_pollfds[0], m_pollfds.size(), -1);
		if (nfds < 0)
			throw std::runtime_error("poll() failed");
		for (i = 0; i < m_pollfds.size(); i++)
		{
			p = &m_pollfds[i];
			if (p->revents == 0)
				continue;
			if (p->fd == m_listener)
				acceptNewClients();
			else
				handleClientEvent(p->fd, p->revents);
		}
	}
}
