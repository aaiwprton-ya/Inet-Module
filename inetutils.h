#ifndef INET_INETUTILS_H
#define INET_INETUTILS_H

#include "std_afx.h"

#define IU_RESPONSE_BUFFER_SIZE 1024

class InetUtils
{
public:
	enum ResponseTemplate {RT_CMD_GET_VALUE, RT_CMD_GIVE_OK, RT_CMD_GIVE_BAD, RT_CMD_ACCEPT_SIZE};
	struct ResponseTemplateType
	{
		ResponseTemplate responseTemplate;
		std::string additionalValue;
	};
private:
	std::vector<const char*> responseBuffer;
	const char* const IUP_CMD_GET_VALUE = "get --value ";
	const char* const IUP_CMD_GIVE_OK = "give --state OK";
	const char* const IUP_CMD_GIVE_BAD = "give --state BAD";
	const char* const IUP_CMD_ACCEPT_SIZE = "accept --size ";
public:
	InetUtils();
	InetUtils(const InetUtils& value) = delete;
	~InetUtils();
public:
	std::string makeCmd(const ResponseTemplateType& cmdTemplate);
	void makeResponse(const std::string& cmd, const void** response, size_t* responseSize);
	void makeResponse(const ResponseTemplateType& responseTemplate, const void** response, size_t* responseSize);
	void clearResponseBuffer();
	static size_t cmdSize(const char* cmd);
	static std::string requestToStr(const void* const request, const size_t requestSize);
};

#endif // INET_INETUTILS_H
