#include "client.h"
#include "include/argparser.h"

int main(int argc, char** argv)
{
	Argparser parser(argc, argv);
	parser.findArg(ArgType::ARGTYPE_STRING, "addr", "a");
	parser.findArg(ArgType::ARGTYPE_INT, "port", "p");
	
	std::string* p_addr = parser.getArg("addr", &p_addr);
	int* p_port = parser.getArg("port", &p_port);

	Client client;
	client.processor.addUnit("start", [](
		const void* const request, 
		const size_t requestSize, 
		const void** response, 
		size_t* responseSize) -> std::string {
			char* data[requestSize + 1];
			memcpy(data, request, requestSize);
			((char*)data)[requestSize] = 0;
			std::cout << "Server response: " << (char*)data << std::endl;
			return "exit";
		});
	
	client.processor.setStartState("start");
	client.processor.setExitState("exit");
	client.processor.setErrorState("error");
	client.processor.setupStartState();
	
	int connection;
	if ((connection = client.connect(*p_addr, *p_port, AF_INET)) == -1)
	{
		return -1;
	}
	
	const char* msg = "hello\r\n";
	client.pushSendBuffer(msg, strlen(msg));
	do {} while (client.step(POLLOUT) != POLLIN);
	client.step(POLLIN);
	return 0;
}
