#ifndef STD_AFX_H
#define STD_AFX_H

#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <fcntl.h>
#include <signal.h>

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <signal.h>
#include <poll.h>

#endif // STD_AFX_H
