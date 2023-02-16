#include "socket.h"

Socket::Socket(const std::string& addr, int port, int faml, int type)
{
	// Create socket
	if ((this->descriptor = socket(faml, type, 0)) == 0)
	{
		throw std::runtime_error("Socket was not created");
	}
	
	// Create address
	struct sockaddr_in* ip_v4;
	struct sockaddr_in6* ip_v6;
	switch (faml)
	{
	case AF_INET:
		ip_v4 = reinterpret_cast<struct sockaddr_in*>(&(this->addr));
		ip_v4->sin_family = faml;
		ip_v4->sin_port = htons(port);
		if ((inet_pton(faml, addr.c_str(), &(ip_v4->sin_addr))) != 1)
		{
			throw std::runtime_error("Invalid ip_v4 address");
		}
		break;
	case AF_INET6:
		ip_v6 = reinterpret_cast<struct sockaddr_in6*>(&(this->addr));
		ip_v6->sin6_family = faml;
		ip_v6->sin6_port = htons(port);
		if ((inet_pton(faml, addr.c_str(), &(ip_v6->sin6_addr))) != 1)
		{
			throw std::runtime_error("Invalid ip_v6 address");
		}
		break;
	default:
		throw std::runtime_error("Incorrect address family");
	}
	this->addrLength = (socklen_t)sizeof(this->addr);
}

Socket::Socket(int desc)
	: descriptor(desc)
{
	this->addrLength = (socklen_t)sizeof(this->addr);
	if (getpeername(desc, &(this->addr), &(this->addrLength)) != 0)
	{
		throw std::runtime_error("The getsockname function was exit with error");
	}
}

Socket::~Socket()
{
	this->disconnect();
	close(this->descriptor);
}

void Socket::bind() const
{
	if (_bind(this->descriptor, &(this->addr), this->addrLength) != 0)
	{
		throw std::runtime_error("Socket was not binded");
	}
}

void Socket::connect() const
{
	if (_connect(this->descriptor, &(this->addr), addrLength) != 0)
	{
		throw std::runtime_error("Connection failed");
	}
}

void Socket::disconnect() const
{
	shutdown(this->descriptor, SHUT_RDWR);
}

void Socket::listen(int queue) const
{
	if (_listen(this->descriptor, queue) < 0)
	{
		throw std::runtime_error("Listening failed");
	}
}

int Socket::accept()
{
	int acceptedDesc;
	if ((acceptedDesc = _accept(this->descriptor, &(this->addr), &(this->addrLength))) == 0)
	{
		throw std::runtime_error("Accept failed");
	}
	// Set nonblocked socked
	int flags = fcntl(acceptedDesc, F_GETFL);
	fcntl(acceptedDesc, F_SETFL, flags | O_NONBLOCK);
	return acceptedDesc;
}

size_t Socket::send(void* data, size_t size, int flags) const
{
	ssize_t sended;
	if ((sended = _send(this->descriptor, data, size, flags)) <= 0)
	{
		throw std::runtime_error("Send failed");
	}
	return (size_t)sended;
}

size_t Socket::recv(void* data, size_t size, int flags) const
{
	ssize_t received;
	if ((received = _recv(this->descriptor, data, size, flags)) <= 0)
	{
		throw std::runtime_error("Receive failed");
	}
	return (size_t)received;
}

void Socket::setup(int optname, void *optval, socklen_t optlen) const
{
	if (setsockopt(this->descriptor, SOL_SOCKET, optname, optval, optlen) != 0)
	{
		throw std::runtime_error("The setsockopt function was exit with error");
	}
}

int Socket::desc() const
{
	return this->descriptor;
}

void Socket::printAddr(std::string& dest) const
{
	const struct sockaddr_in* ip_v4;
	const struct sockaddr_in6* ip_v6;
	char buf[64];
	switch (this->addr.sa_family)
	{
	case AF_INET:
		ip_v4 =	reinterpret_cast<const struct sockaddr_in*>(&(this->addr));
		dest += inet_ntop(AF_INET, &(ip_v4->sin_addr), buf, 64);
		break;
	case AF_INET6:
		ip_v6 = reinterpret_cast<const struct sockaddr_in6*>(&(this->addr));
		dest += inet_ntop(AF_INET, &(ip_v6->sin6_addr), buf, 64);
		break;
	default:
		throw std::runtime_error("Incorrect address family");
	}
}

void Socket::printPort(std::string& dest) const
{
	const struct sockaddr_in* ip_v4;
	const struct sockaddr_in6* ip_v6;
	uint16_t port;
	switch (this->addr.sa_family)
	{
	case AF_INET:
		ip_v4 = reinterpret_cast<const struct sockaddr_in*>(&(this->addr));
		port = ntohs(ip_v4->sin_port);
		break;
	case AF_INET6:
		ip_v6 = reinterpret_cast<const struct sockaddr_in6*>(&(this->addr));
		port = ntohs(ip_v6->sin6_port);
		break;
	default:
		throw std::runtime_error("Incorrect address family");
	}
	std::stringstream sstream;
	sstream << port;
	dest += sstream.str();
}

