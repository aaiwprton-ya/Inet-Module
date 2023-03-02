#include "server.h"
#include "include/argparser.h"

void terminate (int signum)
{}

int main(int argc, char** argv)
{
	signal(SIGINT, terminate);
	
	Argparser parser(argc, argv);
	parser.findArg(ArgType::ARGTYPE_STRING, "addr", "a");
	parser.findArg(ArgType::ARGTYPE_INT, "port", "p");
	
	std::string* p_addr = parser.getArg("addr", &p_addr);
	int* p_port = parser.getArg("port", &p_port);

	Server server(*p_addr, *p_port, AF_INET);
	
	char* helloWorld = "Hello world\n";
	char* errorMsg = "Invalid request\n";
	
	server.processor.addUnit("start", [helloWorld](
		const void* const request, 
		const size_t requestSize, 
		const void** response, 
		size_t* responseSize) -> std::string {
			char* match = "hello";
			char req[6];
			memcpy(req, request, 5);
			req[6] = 0;
			if (strcmp(match, req) != 0 || requestSize > 7)
			{
				return "error";
			}
			*response = helloWorld;
			*responseSize = strlen(helloWorld);
			return "exit";
		});
	server.processor.addUnit("error", [errorMsg](
		const void* const request, 
		const size_t requestSize, 
		const void** response, 
		size_t* responseSize) -> std::string {
			*response = errorMsg;
			*responseSize = strlen(errorMsg);
			return "start";
		});
	
	server.processor.setStartState("start");
	server.processor.setExitState("exit");
	server.processor.setErrorState("error");
	server.processor.setupStartState();
	
	return server.start();
}
