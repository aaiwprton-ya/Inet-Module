#ifndef INET_CLIENT_H
#define INET_CLIENT_H

#include "std_afx.h"
#include "socket.h"

class Client
{
	std::map<int, Socket*> connections;
public:
	Client();
	~Client();
public:
	int connect(const std::string& addr, int port, int faml, int type = SOCK_STREAM);
	void disconnect(int connection);
	void send(int connection, void* data, size_t size, int flags = 0);
	void recv(int connection, void* data, size_t size, int flags = 0);
};

#endif // INET_CLIENT_H
