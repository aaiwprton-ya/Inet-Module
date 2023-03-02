#ifndef INET_SERVER_H
#define INET_SERVER_H

#include "std_afx.h"
#include "session.h"
#include "processor.h"

class Server
{
private:
	Socket socket;
	std::vector<struct pollfd> v_fds;
    std::list<Session*> sessions;
    std::list<Session*>::iterator iter;
public:
	Processor processor;
public:
    Server(const std::string& addr, int port, int faml, int type = SOCK_STREAM);
    ~Server();
public:
    int start();
};

#endif // INET_SERVER_H
