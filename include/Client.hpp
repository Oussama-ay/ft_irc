#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "common.hpp"

class Client
{
public:
	Client(int fd);
	~Client();

	int			getFd() const;
	std::string	&getBuffer();
	void		appendToBuffer(const std::string &data);
	void		clearBuffer();

private:
	int			_fd;
	std::string	_buffer;
};

#endif // CLIENT_HPP
