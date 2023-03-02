#ifndef INET_CLIENT_H
#define INET_CLIENT_H

#include "std_afx.h"
#include "socket.h"
#include "session.h"
#include "processor.h"

class Client
{
	Session* session = nullptr;
public:
	Processor processor;
public:
	Client();
	~Client();
public:
	int connect(const std::string& addr, int port, int faml, int type = SOCK_STREAM) noexcept;
	void disconnect() noexcept;
	short step(short events) noexcept;
	void pushSendBuffer(const void* data, size_t size) noexcept;
};

#endif // INET_CLIENT_H
