#ifndef INET_CLIENT_H
#define INET_CLIENT_H

#include "std_afx.h"
#include "inetutils.h"
#include "socket.h"
#include "session.h"
#include "processor.h"

class Client
{
private:
	Session* session = nullptr;
	const char* startCmd = nullptr;
public:
	Processor processor;
	InetUtils utils;
public:
	Client();
	~Client();
public:
	int connect(const std::string& addr, int port, int faml, int type = SOCK_STREAM) noexcept;
	void disconnect() noexcept;
	short step(short events) noexcept;
	void pushSendBuffer(const void* data, size_t size) noexcept;
    void setStartCmd(const char* startCmd);
};

#endif // INET_CLIENT_H
