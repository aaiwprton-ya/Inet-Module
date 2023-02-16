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
	int connection;
	if ((connection = client.connect(*p_addr, *p_port, AF_INET)) == -1)
	{
		return -1;
	}
	std::string msg = "Hello, server!";
	client.send(connection, (void*)msg.c_str(), msg.length());
	char data[256];
	client.recv(connection, data, 256);
	std::cout << "Server response: " << data << std::endl;
	
	std::string msg2 = "Hello, client!";
	client.send(connection, (void*)msg2.c_str(), msg2.length());
	char data2[256];
	client.recv(connection, data2, 256);
	std::cout << "Server response: " << data2 << std::endl;
	
	return 0;
}
