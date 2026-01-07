#include "../include/Server.hpp"

void	Server::checkRegistration(Client &client)
{
	std::string nick = client.getNickname();
	if (!client.isRegistered() && client.isPassOk() && client.isNickOk() && client.isUserOk())
	{
		client.setRegistered(true);
		sendTo(client, numeric("001", nick, ":Welcome to the ft_irc network " + nick));
	}
}

std::string	Server::numeric(const std::string &code, const std::string &nick, const std::string &msg) const
{
	std::stringstream ss;
	ss << ":" << m_hostname << " " << code << " " << (nick.empty() ? "*" : nick) << " " << msg << "\r\n";
	return ss.str();
}

void	Server::sendTo(Client &client, const std::string &data)
{
	client.appendToSend(data);
	// std::cout << "buffer = " << client.getSendBuffer() << std::endl;
	struct pollfd *p = findPollfd(client.getFd());
	if (p)
		p->events |= POLLOUT;
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
	m_hostname = inet_ntoa(addr.sin_addr);
	addr.sin_port = htons(port);

	if (bind(m_listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind() failed");

	if (listen(m_listener, 32) < 0)
		throw std::runtime_error("listen() failed");

	// set non-blocking
	fcntl(m_listener, F_SETFL, O_NONBLOCK);

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

struct pollfd	*Server::findPollfd(int fd)
{
	for (std::vector<struct pollfd>::iterator it = m_pollfds.begin(); it != m_pollfds.end(); ++it)
		if (it->fd == fd)
			return &(*it);
	return (NULL);
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
