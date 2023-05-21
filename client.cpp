#include "client.h"

Client::Client()
{}

Client::~Client()
{
	this->disconnect();
}

int Client::connect(const std::string& addr, int port, int faml, int type) noexcept
{
	try
	{
		this->session = new Session(addr, port, faml, type, this->processor);
		if (this->startCmd != nullptr)
		{
			this->session->pushSendBuffer(this->startCmd, InetUtils::cmdSize(this->startCmd), 0);
			do {} while (session->step(POLLOUT) != POLLIN);
		}
		return this->session->desc();
	}
	catch (const std::runtime_error& ex)
	{
		std::cerr << "In Client::connect: " << ex.what() << std::endl;
	}
	return -1;
}

void Client::disconnect() noexcept
{
	if (this->session != nullptr)
	{
		delete this->session;
	}
	this->session = nullptr;
}

short Client::step(short events) noexcept
{
	if (this->session != nullptr)
	{
		return this->session->step(events);
	} else
	{
		return 0;
	}
}

void Client::pushSendBuffer(const void* data, size_t size) noexcept
{
	if (this->session != nullptr)
	{
		this->session->pushSendBuffer(data, size, 0);
	}
}

void Client::setStartCmd(const char* startCmd)
{
	this->startCmd = startCmd;
}

