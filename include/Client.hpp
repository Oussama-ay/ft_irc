#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "common.hpp"
#include "Parser.hpp"

class Client
{
public:
	Client(int fd);
	~Client();

	int						getFd() const;
	int						getStat() const;
	std::string&			getRecvBuffer();
	std::string&			getSendBuffer();
	const std::string&		getNickname() const;
	const std::string&		getUsername() const;

	bool					isRegistered() const;
	bool					isPassOk() const;
	bool					isNickOk() const;
	bool					isUserOk() const;

	void					setPassOk(bool ok);
	void					setNickOk(bool ok);
	void					setUserOk(bool ok);

	void					setNickname(const std::string& nickname);
	void					setUsername(const std::string& username);
	void					setRealname(const std::string& realname);
	void					setRegistered(bool registered);

	void					appendToRecv(const std::string &data);
	void					appendToSend(const std::string &data);
	void					clearRecv();
	void					clearSend();

private:
	int						m_fd;

	bool					m_registered;
	bool					m_passOk;
	bool					m_nickOk;
	bool					m_userOk;

	std::string				m_nickname;
	std::string				m_username;
	std::string				m_realname;
	std::string				m_recvBuffer;
	std::string				m_sendBuffer;
};

#endif // CLIENT_HPP
