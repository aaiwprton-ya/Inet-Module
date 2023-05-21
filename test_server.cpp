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
	
	const char* cmdOK = "give --state OK";
	const char* cmdBAD = "give --state BAD";
	
	// принять команду
	// запрос на передачу данных с указанием размера
	// прием подверждения
	// отправка данных
	// принять комманду
	
	uint8_t arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint32_t arr2[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
	
	server.processor.addUnit("start", [&arr1, &arr2, &server](UnitBridge& bridge) -> std::string {
			std::cout << "unit start" << std::endl;
			
			std::string str = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << str << std::endl;
			
			Argparser cmdPars(str);
			std::string* p_value = nullptr;
			if (cmdPars.findArg(ArgType::ARGTYPE_STRING, "value", "v"))
			{
				p_value = cmdPars.getArg("value", &p_value);
			}
			
			if (p_value == nullptr)
			{
				*(bridge.plannedData) = nullptr;
				*(bridge.plannedSize) = 0;
				*(bridge.expectedSize) = 0;
				return "error";
			} else
			{
				if ((*p_value).compare("arr1") == 0)
				{
					*(bridge.plannedData) = arr1;
					*(bridge.plannedSize) = sizeof(arr1);
				} else
				if ((*p_value).compare("arr2") == 0)
				{
					*(bridge.plannedData) = arr2;
					*(bridge.plannedSize) = sizeof(arr2);
				} else
				{
					*(bridge.plannedData) = nullptr;
					*(bridge.plannedSize) = 0;
					*(bridge.expectedSize) = 0;
					return "error";
				}
			}
			std::string cmdAccept = "accept --size " + std::to_string(*(bridge.plannedSize));
			server.utils.makeResponse(cmdAccept, &(bridge.response), &(bridge.responseSize));
			*(bridge.expectedSize) = 0;
			return "accepted";
		});
	
	server.processor.addUnit("accepted", [](UnitBridge& bridge) -> std::string {
			std::cout << "unit accepted" << std::endl;
			
			std::string str = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << str << std::endl;
			
			Argparser cmdPars(str);
			std::string* p_state = nullptr;
			if (cmdPars.findArg(ArgType::ARGTYPE_STRING, "state", "s"))
			{
				p_state = cmdPars.getArg("state", &p_state);
			}
			
			if (p_state == nullptr)
			{
				return "error";
			} else
			{
				if ((*p_state).compare("BAD") == 0)
				{
					return "start";
				} else
				if ((*p_state).compare("OK") == 0)
				{
					bridge.response = *(bridge.plannedData);
					bridge.responseSize = *(bridge.plannedSize);
				} else
				{
					return "error";
				}
			}
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = 0;
			return "start";
		});
		
	server.processor.addUnit("error", [cmdBAD](UnitBridge& bridge) -> std::string {
			std::cout << "unit error" << std::endl;
			bridge.response = cmdBAD;
			bridge.responseSize = InetUtils::cmdSize(cmdBAD);
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = 0;
			return "start";
		});
	
	server.processor.setStartState("start");
	server.processor.setExitState("exit");
	server.processor.setErrorState("error");
	server.processor.setupStartState();
	
	server.setStartCmd(cmdOK);
	
	return server.start();
}
