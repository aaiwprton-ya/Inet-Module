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

	// === TEST DATA ===
	// --static--
	uint8_t arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint32_t arr2[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
	
	// --mutable--
	std::vector<uint8_t> vec1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::vector<uint32_t> vec2 = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
	
	// --generative--
	struct SupplySources
	{
		uint8_t* gSource = nullptr;
		size_t size = 0;
	};
	SupplySources gSources;
	uint8_t p_gSource[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	gSources.gSource = p_gSource;
	gSources.size = sizeof(p_gSource) / sizeof(uint8_t);
	std::vector<uint8_t> gvec1;
	std::vector<uint32_t> gvec2;
	DataGeneratorType gen1 = [](RecvPoolMutableEntityType targetBuffer, const void* anySources, size_t sourcesSize) -> RecvPoolMutableEntityType
	{
		((std::vector<uint8_t>*)targetBuffer)->clear();
		SupplySources* gSources = (SupplySources*)anySources;
		for (int i = 0; i < gSources->size; ++i)
		{
			((std::vector<uint8_t>*)targetBuffer)->push_back(gSources->gSource[i]);
		}
		return targetBuffer;
	};
	DataGeneratorType gen2 = [](RecvPoolMutableEntityType targetBuffer, const void* anySources, size_t sourcesSize) -> RecvPoolMutableEntityType
	{
		((std::vector<uint32_t>*)targetBuffer)->clear();
		SupplySources* gSources = (SupplySources*)anySources;
		for (int i = 0; i < gSources->size; ++i)
		{
			((std::vector<uint32_t>*)targetBuffer)->push_back(gSources->gSource[i] * 10);
		}
		return targetBuffer;
	};
	
	// === TEST UNITS ===
	// --static--
	/*Processor::UnitTemplateType startUnitTemplate;
	startUnitTemplate.unitTemplate = Processor::UT_GET_TO_AGREEMENT_STATIC;
	startUnitTemplate.nextUnit = "accepted";
	startUnitTemplate.errorUnit = "error";
	startUnitTemplate.recvPoolStatic["arr1"] = RecvPoolStaticEntity((void*)arr1, sizeof(arr1));
	startUnitTemplate.recvPoolStatic["arr2"] = RecvPoolStaticEntity((void*)arr2, sizeof(arr2));
	server.processor.makeUnit("start", startUnitTemplate);*/
	
	// --mutable--
	/*Processor::UnitTemplateType startUnitTemplate;
	startUnitTemplate.unitTemplate = Processor::UT_GET_TO_AGREEMENT_MUTABLE;
	startUnitTemplate.nextUnit = "accepted";
	startUnitTemplate.errorUnit = "error";
	startUnitTemplate.recvPoolMutable["vec1"] = &vec1;
	startUnitTemplate.recvPoolMutable["vec2"] = &vec2;
	server.processor.makeUnit("start", startUnitTemplate);*/
	
	// --generative--
	/*Processor::UnitTemplateType startUnitTemplate;
	startUnitTemplate.unitTemplate = Processor::UT_GET_TO_AGREEMENT_GENERATIVE;
	startUnitTemplate.nextUnit = "accepted";
	startUnitTemplate.errorUnit = "error";
	startUnitTemplate.recvPoolGenerative["gen1"] = RecvPoolGenerativeEntity(&gvec1, gen1);
	startUnitTemplate.recvPoolGenerative["gen2"] = RecvPoolGenerativeEntity(&gvec2, gen2);
	startUnitTemplate.generativeSources = (void*)&gSources;
	startUnitTemplate.generativeSourcesSize = sizeof(gSources);
	server.processor.makeUnit("start", startUnitTemplate);*/
	
	// --universal--
	Processor::UnitTemplateType startUnitTemplate;
	startUnitTemplate.unitTemplate = Processor::UT_GET_TO_AGREEMENT_UNIVERSAL;
	startUnitTemplate.nextUnit = "accepted";
	startUnitTemplate.errorUnit = "error";
	startUnitTemplate.recvPoolStatic["arr1"] = RecvPoolStaticEntity((void*)arr1, sizeof(arr1));
	startUnitTemplate.recvPoolStatic["arr2"] = RecvPoolStaticEntity((void*)arr2, sizeof(arr2));
	startUnitTemplate.recvPoolMutable["vec1"] = &vec1;
	startUnitTemplate.recvPoolMutable["vec2"] = &vec2;
	startUnitTemplate.recvPoolGenerative["gen1"] = RecvPoolGenerativeEntity(&gvec1, gen1);
	startUnitTemplate.recvPoolGenerative["gen2"] = RecvPoolGenerativeEntity(&gvec2, gen2);
	startUnitTemplate.generativeSources = (void*)&gSources;
	startUnitTemplate.generativeSourcesSize = sizeof(gSources);
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
