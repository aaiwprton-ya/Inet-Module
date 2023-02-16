#include "server.h"

Server::Server(const std::string& addr, int port, int faml, int type)
	: socket(addr, port, faml, type)
{
	this->socket.bind();
	int optval = 1;
	this->socket.setup(SO_REUSEADDR, &optval, sizeof(optval));
}

Server::~Server()
{
	for (Socket* p_sock: this->clients)
	{
		delete p_sock;
	}
}

int Server::start()
{
	this->socket.listen();
	int activity;;
	v_fds.push_back( {this->socket.desc(), POLLIN, 0} );
	while (true)
	{
		// Get activity
		if ((activity = poll(v_fds.data(), v_fds.size(), -1)) == -1)
		{
			throw std::runtime_error("Failed poll");
		}
		
		// Check new connection
		if ((v_fds[0].revents & POLLIN) == POLLIN)
		{
			try
			{
				v_fds.push_back( {this->socket.accept(), POLLIN, 0} );
				std::cerr << "New connection" << std::endl;
			}
			catch (const std::runtime_error& ex)
			{
				std::cerr << "Runtime error: " << ex.what() << std::endl;
				continue;
			}
		} else
		// Get new request
		{
			for (struct pollfd& fds: v_fds)
			{
				if ((fds.revents & POLLIN) == POLLIN)
				{
					try
					{
						// TODO process request
						char data[1024];
						memset(data, '\0', 1024);
						int received = recv(fds.fd, &data, 1024, 0);
						if (received > 0)
						{
							send(fds.fd, data, 1024, 0);
						} else
						{
							std::cerr << "Connection closed" << std::endl;
							fds.fd = -1;
						}
					}
					catch (const std::runtime_error& ex)
					{
						std::cerr << "Runtime error: " << ex.what() << std::endl;
						std::cerr << "Connection closed" << std::endl;
						fds.fd = -1;
						continue;
					}
				}
			}
		}
		// remove unused fds
		v_fds.erase(
			std::remove_if(
				v_fds.begin(), 
				v_fds.end(),
				[](const struct pollfd& fds) { 
					return fds.fd == -1;
				}), 
			v_fds.end());
	}
	return 0;
}
