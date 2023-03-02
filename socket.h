#ifndef INET_SOCKET_H
#define INET_SOCKET_H

#include "std_afx.h"

static int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	return connect(sockfd, addr, addrlen);
}

static int _listen(int sockfd, int backlog)
{
	return listen(sockfd, backlog);
}

static int _accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return accept(sockfd, addr, addrlen);
}

static ssize_t _send(int sockfd, const void *buf, size_t len, int flags)
{
	return send(sockfd, buf, len, flags);
}

static ssize_t _recv(int sockfd, void *buf, size_t len, int flags)
{
	return recv(sockfd, buf, len, flags);
}

static int _bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	return bind(sockfd, addr, addrlen);
}

class Socket
{
private:
    struct sockaddr addr;
    socklen_t addrLength;
    int descriptor;
public:
	Socket(const std::string& addr, int port, int faml, int type = SOCK_STREAM);
    Socket(int desc);
    ~Socket();

	void bind() const;
    void connect() const;
    void disconnect() const;
    void listen(int queue = 32) const;
    int accept();
    size_t send(const void* data, size_t size, int flags = 0) const;
    size_t recv(void* data, size_t size, int flags = 0) const;
    void setup(int optname, void *optval, socklen_t optlen) const;

    int desc() const;
    void printAddr(std::string& dest) const;
    void printPort(std::string& dest) const;
};

#endif // INET_SOCKET_H
