#include "client.h"
#include "include/argparser.h"

void stop(){}

int main(int argc, char** argv)
{
	Argparser parser(argc, argv);
	parser.findArg(ArgType::ARGTYPE_STRING, "addr", "a");
	parser.findArg(ArgType::ARGTYPE_INT, "port", "p");
	
	std::string* p_addr = parser.getArg("addr", &p_addr);
	int* p_port = parser.getArg("port", &p_port);

	Client client;
	
	// отправить команду
	// принять запрос на отправку данных указанного размера, выделить буффер
	// отправить подтверждение
	// принять данные
	// завершить работу
	
	const char* cmdBAD = "give --state BAD";
	const char* cmdGet = "get --value arr1";
	const char* cmdOK = "give --state OK";
	typedef uint8_t RequestType;
	
	client.processor.addUnit("start", [cmdGet](UnitBridge& bridge) -> std::string {
			std::cout << "unit start" << std::endl;
			stop();
			std::string str = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << str << std::endl;
			
			Argparser cmdPars(str);
			std::string* p_state = nullptr;
			if (cmdPars.findArg(ArgType::ARGTYPE_STRING, "state", "s"))
			{
				p_state = cmdPars.getArg("state", &p_state);
			}
			
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = 0;
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
					bridge.response = cmdGet;
					bridge.responseSize = InetUtils::cmdSize(cmdGet);
					return "accept";
				} else
				{
					return "error";
				}
			}
		});
	
	client.processor.addUnit("accept", [&cmdOK](UnitBridge& bridge) -> std::string {
			std::cout << "unit accept" << std::endl;
			
			std::string str = InetUtils::requestToStr(bridge.request, bridge.requestSize);
			std::cout << str << std::endl;
			
			Argparser cmdPars(str);
			cmdPars.findArg(ArgType::ARGTYPE_INT, "size", "sz");
			
			int* p_size = cmdPars.getArg("size", &p_size);
			
			if (p_size == nullptr)
			{
				*(bridge.expectedSize) = 0;
				return "error";
			}
			bridge.response = cmdOK;
			bridge.responseSize = InetUtils::cmdSize(cmdOK);
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = *p_size;
			return "receive";
		});
	
	client.processor.addUnit("receive", [](UnitBridge& bridge) -> std::string {
			std::cout << "unit receive" << std::endl;
			RequestType* buf = new RequestType[bridge.requestSize / sizeof(RequestType)];
			memcpy((void*)buf, bridge.request, bridge.requestSize);
			for (int i = 0; i < bridge.requestSize / sizeof(RequestType); ++i)
			{
				std::cout << (int)buf[i] << " ";
			}
			std::cout << std::endl;
			delete buf;
			return "exit";
		});
	
	client.processor.addUnit("error", [cmdBAD](UnitBridge& bridge) -> std::string {
			std::cout << "unit error" << std::endl;
			bridge.response = cmdBAD;
			bridge.responseSize = InetUtils::cmdSize(cmdBAD);
			*(bridge.plannedData) = nullptr;
			*(bridge.plannedSize) = 0;
			*(bridge.expectedSize) = 0;
			return "start";
		});
	
	client.processor.setStartState("start");
	client.processor.setExitState("exit");
	client.processor.setErrorState("error");
	client.processor.setupStartState();
	
	int connection;
	//client.setStartCmd("get --value arr1");
	if ((connection = client.connect(*p_addr, *p_port, AF_INET)) == -1)
	{
		return -1;
	}
	
	short events = POLLIN;
	while ((events = client.step(events)) != 0);
	return 0;
}
