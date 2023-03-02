#ifndef INET_SESSION_H
#define INET_SESSION_H

#include "std_afx.h"
#include "socket.h"
#include "processor.h"

#define RECEIVE_BUFFER_SIZE 1048576
#define RECEIVE_PACKEG_SIZE 1024
#define SEND_BUFFER_SIZE 1048576

class Session
{
private:
	Socket socket;
	Processor processor;
	char p_rcvBuff[RECEIVE_BUFFER_SIZE];
	size_t rcvHead = 0;
	size_t endPack = 0; // the end of checked space to not repeat checking
	char p_sndBuff[SEND_BUFFER_SIZE];
	size_t sndHead = 0;
public:
	Session(const std::string& addr, int port, int faml, int type, Processor processor);
	Session(int fd, Processor processor);
	~Session();
public:	
	short step(short events) noexcept;
	int desc() const noexcept;
	size_t recv(int flags = 0);
	ssize_t checkRecv() noexcept;
	bool getRcvPack(const void** data, size_t* size) noexcept;
	void eraseRcvPack(size_t size) noexcept;
	size_t send(const void* data, size_t size, int flags = 0);
	size_t sendBuffer(int flags = 0);
	void pushSendBuffer(const void* data, size_t size, size_t sended);
	std::string& logSocket(std::string& dest) const noexcept;
	std::string logSocket() const noexcept;
};

#endif // INET_SESSION_H
