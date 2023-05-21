#ifndef INET_INETUTILS_H
#define INET_INETUTILS_H

#include "std_afx.h"

class InetUtils
{
private:
	std::vector<const char*> responseBuffer;
public:
	InetUtils();
	InetUtils(const InetUtils& value) = delete;
	~InetUtils();
public:
	void makeResponse(const std::string& cmd, const void** response, size_t* responseSize);
	void clearResponseBuffer();
	static size_t cmdSize(const char* cmd);
	static std::string requestToStr(const void* const request, const size_t requestSize);
};

#endif // INET_INETUTILS_H
