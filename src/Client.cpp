#include "../include/Client.hpp"
#include "../include/Parser.hpp"

Client::Client(int fd) : m_fd(fd), m_registered(false), m_passOk(false), m_nickOk(false), m_userOk(false), m_isOperator(false) {}

Client::~Client() {}

int Client::getFd() const { return m_fd; }


std::string &Client::getRecvBuffer() { return m_recvBuffer; }

std::string &Client::getSendBuffer() { return m_sendBuffer; }

void Client::appendToRecv(const std::string &data) { m_recvBuffer += data; }

void Client::clearRecv() { m_recvBuffer.clear(); }

void Client::clearSend() { m_sendBuffer.clear(); }

const std::string& Client::getNickname() const { return m_nickname; }

const std::string& Client::getUsername() const { return m_username; }


void Client::setNickname(const std::string& nickname) { m_nickname = nickname; }

void Client::setUsername(const std::string& username) { m_username = username; }

void Client::setRealname(const std::string& realname) { m_realname = realname; }

void Client::setRegistered(bool registered) { m_registered = registered; }

bool Client::isRegistered() const { return m_registered; }

void	Client::setPassOk(bool ok)
{
	m_passOk = ok;
}

void	Client::setNickOk(bool ok)
{
	m_nickOk = ok;
}

void	Client::setUserOk(bool ok)
{
	m_userOk = ok;
}

void Client::setOperator(bool ok)
{
	m_isOperator = ok;
}

bool	Client::isPassOk() const
{
	return (m_passOk);
}

bool	Client::isNickOk() const
{
	return (m_nickOk);
}

bool	Client::isUserOk() const
{
	return (m_userOk);
}

bool	Client::isOperator() const
{
	return m_isOperator;
}

void	Client::appendToSend(const std::string &data)
{
	m_sendBuffer += data;
}
