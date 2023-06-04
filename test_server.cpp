#include "server.h"
#include "include/argparser.h"

void terminate (int signum)
{}

// server side protocol algorithm
// 0. send hello
// 1. accept client request, choose sending data, check it planned size and send to client
// 2. accept client confirm, send data
// 4. accept next request...

int main(int argc, char** argv)
{
	signal(SIGINT, terminate);
	
	Argparser parser(argc, argv);
	parser.findArg(ArgType::ARGTYPE_STRING, "addr", "a");
	parser.findArg(ArgType::ARGTYPE_INT, "port", "p");
	
	std::string* p_addr = parser.getArg("addr", &p_addr);
	int* p_port = parser.getArg("port", &p_port);

	Server server(*p_addr, *p_port, AF_INET);

	// test data
	uint8_t arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint32_t arr2[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
	
	Processor::UnitTemplateType startUnitTemplate;
	startUnitTemplate.unitTemplate = Processor::UT_GET_TO_PREPARATION;
	startUnitTemplate.nextUnit = "accepted";
	startUnitTemplate.errorUnit = "error";
	startUnitTemplate.dataPool["arr1"] = std::pair<void*, size_t>((void*)arr1, sizeof(arr1));
	startUnitTemplate.dataPool["arr2"] = std::pair<void*, size_t>((void*)arr2, sizeof(arr2));
	server.processor.makeUnit("start", startUnitTemplate);
	
	Processor::UnitTemplateType acceptedUnitTemplate;
	acceptedUnitTemplate.unitTemplate = Processor::UT_READY_TO_SEND;
	acceptedUnitTemplate.nextUnit = "start";
	acceptedUnitTemplate.backUnit = "start";
	acceptedUnitTemplate.errorUnit = "error";
	server.processor.makeUnit("accepted", acceptedUnitTemplate);
		
	Processor::UnitTemplateType errorUnitTemplate;
	errorUnitTemplate.unitTemplate = Processor::UT_ERROR;
	errorUnitTemplate.backUnit = "start";
	server.processor.makeUnit("error", errorUnitTemplate);
	
	server.processor.setStartState("start");
	server.processor.setExitState("exit");
	server.processor.setErrorState("error");
	server.processor.setupStartState();
	
	// if the server is the initiator 
	server.setStartCmd({InetUtils::RT_CMD_GIVE_OK, ""});
	
	return server.start();
}
