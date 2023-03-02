#include "session.h"

Session::Session(const std::string& addr, int port, int faml, int type, Processor processor)
	: socket(addr, port, faml, type), processor(processor)
{
	this->socket.connect();
}

Session::Session(int fd, Processor processor)
	: socket(fd), processor(processor)
{}

Session::~Session()
{}

short Session::step(short events) noexcept
{
	short revents = POLLIN;
	if ((events & POLLIN) == POLLIN)
	{
		// request receiving
		size_t received = 0;
		try
		{
			received = this->recv();
		} catch (const std::runtime_error& ex)
		{
			std::cerr << "In Session::step: " << ex.what() << std::endl;
			received = 0;
		}
		if (received == 0)
		{
			this->processor.setupExitState();
			revents = 0;
		}
		
		// request processing
		const void* request = nullptr;
		size_t requestSize = 0;
		const void* response = nullptr;
		size_t responseSize = 0;
		if (this->getRcvPack(&request, &requestSize))
		{
			this->processor((const void*)request, (size_t)requestSize, &response, &responseSize);
			this->eraseRcvPack(requestSize);
			if (responseSize > 0)
			{
				try
				{
					this->pushSendBuffer(response, responseSize, 0);
				} catch (const std::runtime_error& ex)
				{
					std::cerr << "In Session::step: " << ex.what() << std::endl;
					this->processor.setupExitState();
				}
				revents = revents | POLLOUT;
			} else
			{
				if (this->processor.isExit())
				{
					revents = 0;
				}
			}
		}
	}
	
	// TODO process error and exit cases
	
	if ((events & POLLOUT) == POLLOUT)
	{
		// responce sending
		this->sendBuffer();
		if (this->sndHead > 0)
		{
			revents = revents | POLLOUT;
		} else
		{
			if (this->processor.isExit())
			{
				revents = 0;
			}
		}
	}
	return revents;
}

size_t Session::recv(int flags)
{
	size_t rcvSize = 
		RECEIVE_PACKEG_SIZE < (RECEIVE_BUFFER_SIZE - rcvHead) ? 
		RECEIVE_PACKEG_SIZE : RECEIVE_BUFFER_SIZE - rcvHead;
	if (rcvSize == 0)
	{
		throw std::runtime_error("In Session::recv: not enough memory for receive data");
	}
	size_t received = 0;
	try
	{
		received = this->socket.recv(p_rcvBuff + rcvHead, rcvSize, flags);
	} catch (const std::runtime_error& ex)
	{
		std::cerr << "In Session::recv: " << ex.what() << std::endl;
		received = 0;
	}
	rcvHead += received;
	return received;
}

ssize_t Session::checkRecv() noexcept
{
	ssize_t resEndPack = -1;
	for (; this->endPack < this->rcvHead; ++(this->endPack))
	{
		if (this->p_rcvBuff[this->endPack] == '\n')
		{
			resEndPack = (ssize_t)this->endPack + 1;
			this->endPack = 0;
			break;
		}
	}
	return resEndPack;
}

bool Session::getRcvPack(const void** data, size_t* size) noexcept
{
	// TODO what is better?
	// buffer [startPack......endPack|startTail....endTail.................]
	// or
	// buffer [......startPack......endPack|startTail....endTail...........]
	// buffer [...endPack|startTail....endTail.................startPack...]
	//
	ssize_t resEndPack = this->checkRecv();
	if (resEndPack == -1)
	{
		return false;
	} else
	{
		*data = this->p_rcvBuff;
		*size = (size_t)resEndPack;
		return true;
	}
}

void Session::eraseRcvPack(size_t size) noexcept
{
	char* src = this->p_rcvBuff + size;
	size_t newSize = this->rcvHead - size;
	memmove(this->p_rcvBuff, src, newSize);
	this->rcvHead -= size;
}

size_t Session::send(const void* data, size_t size, int flags)
{
	if (this->sndHead > 0)
	{
		this->sendBuffer(flags);
		if (this->sndHead > 0)
		{
			this->pushSendBuffer(data, size, 0);
			return 0;
		}
	}
	if (data == nullptr || size == 0)
	{
		return 0;
	}
	size_t sended = 0;
	try
	{
		sended = this->socket.send(data, size, flags);
	} catch (const std::runtime_error& ex)
	{
		std::cerr << "In Session::send: " << ex.what() << std::endl;
		sended = 0;
	}
	// If sended is not complete then try save unsended data
	if (sended < size)
	{
		this->pushSendBuffer(data, size, sended);
	}
	return sended;
}

size_t Session::sendBuffer(int flags)
{
	size_t sended = 0;
	try
	{
		sended = this->socket.send(this->p_sndBuff, this->sndHead, flags);
	} catch (const std::runtime_error& ex)
	{
		std::cerr << "In Session::sendBuffer: " << ex.what() << std::endl;
		sended = 0;
	}
	if (sended > 0)
	{
		size_t buffSize = this->sndHead - sended;
		char* src = this->p_sndBuff + sended;
		memmove(this->p_sndBuff, src, buffSize);
		this->sndHead = buffSize;
	}
	return sended;
}

void Session::pushSendBuffer(const void* data, size_t size, size_t sended)
{
	size_t sndSize = SEND_BUFFER_SIZE - this->sndHead;
	size_t saveSize = size - sended;
	if (sndSize < saveSize)
	{
		throw std::runtime_error("In Session::pushSendBuffer: not enough memory for save unsended data");
	} else
	{
		char* dest = this->p_sndBuff + sndHead;
		const char* src = (const char*)data + sended;
		memcpy(dest, src, saveSize);
		sndHead += saveSize;
	}
}

int Session::desc() const noexcept
{
	return this->socket.desc();
}

std::string& Session::logSocket(std::string& dest) const noexcept
{
	this->socket.printAddr(dest);
	dest += ':';
	this->socket.printPort(dest);
	return dest;
}

std::string Session::logSocket() const noexcept
{
	std::string dest;
	this->socket.printAddr(dest);
	dest += ':';
	this->socket.printPort(dest);
	return dest;
}

