#include "client.h"
#include "include/argparser.h"

// client side protocol algorithm
// 0. accept server hello
// 1. send request
// 2. get expected data size, confirm readiness to receive
// 4. accept data
// 5. process data, next iteration or closing

int main(int argc, char** argv)
{
	Argparser parser(argc, argv);
	parser.findArg(ArgType::ARGTYPE_STRING, "addr", "a");
	parser.findArg(ArgType::ARGTYPE_INT, "port", "p");
	
	std::string* p_addr = parser.getArg("addr", &p_addr);
	int* p_port = parser.getArg("port", &p_port);

	Client client;
	
	// choose the requested data: uint8_t arr1 or uint32_t arr2, uint8_t vec1 or uint32_t vec2, uint8_t gen1 or uint32_t gen2
	typedef uint32_t RequestType;
	std::string dataName = "gen2";
	
	Processor::UnitTemplateType startUnitTemplate;
	startUnitTemplate.unitTemplate = Processor::UT_STATE_TO_GET;
	startUnitTemplate.nextUnit = "accept";
	startUnitTemplate.backUnit = "start";
	startUnitTemplate.errorUnit = "error";
	startUnitTemplate.getValue = dataName;
	client.processor.makeUnit("start", startUnitTemplate);
	
	Processor::UnitTemplateType acceptUnitTemplate;
	acceptUnitTemplate.unitTemplate = Processor::UT_ACCEPT_TO_READY;
	acceptUnitTemplate.nextUnit = "receive";
	acceptUnitTemplate.errorUnit = "error";
	client.processor.makeUnit("accept", acceptUnitTemplate);
	
	Processor::UnitTemplateType receiveUnitTemplate;
	receiveUnitTemplate.unitTemplate = Processor::UT_RECEIVE;
	receiveUnitTemplate.nextUnit = "exit";
	receiveUnitTemplate.errorUnit = "error";
	Processor::RecvBufferType<RequestType> recvBuffer;
	receiveUnitTemplate.recvBuffer = &recvBuffer;
	client.processor.makeUnit("receive", receiveUnitTemplate);
	
	Processor::UnitTemplateType errorUnitTemplate;
	errorUnitTemplate.unitTemplate = Processor::UT_ERROR;
	errorUnitTemplate.backUnit = "start";
	client.processor.makeUnit("error", errorUnitTemplate);
	
	client.processor.setStartState("start");
	client.processor.setExitState("exit");
	client.processor.setErrorState("error");
	client.processor.setupStartState();
	
	// if the client is the initiator 
	//client.setStartCmd("get --value arr1");
	
	int connection;
	if ((connection = client.connect(*p_addr, *p_port, AF_INET)) == -1)
	{
		return -1;
	}
	
	short events = POLLIN;
	while ((events = client.step(events)) != 0);
	
	// test print
	for (int i = 0; i < recvBuffer.getSize() / sizeof(RequestType); ++i)
	{
		std::cout << (int)((RequestType*)recvBuffer.getBuffer())[i] << " ";
	}
	std::cout << std::endl;
	
	return 0;
}
