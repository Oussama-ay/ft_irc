#include "../include/Client.hpp"

#include <unistd.h>

Client::Client(int fd) : _fd(fd), _buffer() {}

Client::~Client() {}

int Client::getFd() const { return _fd; }

std::string &Client::getBuffer() { return _buffer; }

void Client::appendToBuffer(const std::string &data) { _buffer += data; }

void Client::clearBuffer() { _buffer.clear(); }
