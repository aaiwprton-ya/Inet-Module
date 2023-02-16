#ifndef INET_SERVER_H
#define INET_SERVER_H

#include "std_afx.h"
#include "socket.h"

class Server
{
private:
	//fd_set set;
	std::vector<struct pollfd> v_fds;
    std::list<Socket*> clients;
    std::list<Socket*>::iterator iter;
public:
    Socket socket;
public:
    Server(const std::string& addr, int port, int faml, int type = SOCK_STREAM);
    ~Server();

    int start();
};

#endif // INET_SERVER_H
