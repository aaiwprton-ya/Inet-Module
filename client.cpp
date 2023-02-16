#include "client.h"

Client::Client()
{}

Client::~Client()
{
	for (auto pair: this->connections)
	{
		delete pair.second;
	}
}

int Client::connect(const std::string& addr, int port, int faml, int type)
{
	try
	{
		Socket* p_sock = new Socket(addr, port, faml, type);
		p_sock->connect();
		this->connections.insert(std::pair<int, Socket*>(p_sock->desc(), p_sock));
		return p_sock->desc();
	}
	catch (const std::runtime_error& ex)
	{
		std::cerr << "Runtime error: " << ex.what() << std::endl;
	}
	return -1;
}

void Client::disconnect(int connection)
{
	this->connections.erase(connection);
}

void Client::send(int connection, void* data, size_t size, int flags)
{
	try
	{
		this->connections[connection]->send(data, size, flags);
	}
	catch (const std::runtime_error& ex)
	{
		std::cerr << "Runtime error: " << ex.what() << std::endl;
	}
}

void Client::recv(int connection, void* data, size_t size, int flags)
{
	try
	{
		this->connections[connection]->recv(data, size, flags);
	}
	catch (const std::runtime_error& ex)
	{
		std::cerr << "Runtime error: " << ex.what() << std::endl;
	}
}

