#include "inetutils.h"

InetUtils::InetUtils()
{}

InetUtils::~InetUtils()
{
	this->clearResponseBuffer();
}
	
void InetUtils::makeResponse(const std::string& cmd, const void** response, size_t* responseSize)
{
	char* p_char = new char[cmd.size() + 1];
	memcpy(p_char, cmd.c_str(), cmd.size());
	p_char[cmd.size()] = '\0';
	*response = p_char;
	*responseSize = InetUtils::cmdSize(p_char);
	this->responseBuffer.push_back(p_char);
}

void InetUtils::clearResponseBuffer()
{
	for (const char* p_ch: this->responseBuffer)
	{
		delete p_ch;
	}
	this->responseBuffer.clear();
}

size_t InetUtils::cmdSize(const char* cmd)
{
	return strlen(cmd) + 1;
}

std::string InetUtils::requestToStr(const void* const request, const size_t requestSize)
{
	char* req = new char[requestSize  + 1];
	memcpy(req, request, requestSize);
	req[requestSize] = 0;
	std::string result(req);
	delete[] req;
	return result;
}
