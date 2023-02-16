#include "server.h"
#include "include/argparser.h"

int main(int argc, char** argv)
{
	Argparser parser(argc, argv);
	parser.findArg(ArgType::ARGTYPE_STRING, "addr", "a");
	parser.findArg(ArgType::ARGTYPE_INT, "port", "p");
	
	std::string* p_addr = parser.getArg("addr", &p_addr);
	int* p_port = parser.getArg("port", &p_port);

	Server server(*p_addr, *p_port, AF_INET);
	return server.start();
}
