#ifndef INET_SERVER_H
#define INET_SERVER_H

#include "std_afx.h"
#include "inetutils.h"
#include "session.h"
#include "processor.h"

class Server
{
private:
	Socket socket;
	std::vector<struct pollfd> v_fds;
    std::list<Session*> sessions;
    std::list<Session*>::iterator iter;
    const char* startCmd = nullptr;
public:
	Processor processor;
	InetUtils utils;
public:
    Server(const std::string& addr, int port, int faml, int type = SOCK_STREAM);
    ~Server();
public:
    int start();
    void setStartCmd(const char* startCmd);
};

#endif // INET_SERVER_H
