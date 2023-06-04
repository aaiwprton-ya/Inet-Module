#include "server.h"

static void throwErrno(const std::string& msg)
{
	throw std::runtime_error(msg + ": " + strerror(errno));
}

Server::Server(const std::string& addr, int port, int faml, int type)
	: socket(addr, port, faml, type), processor(&utils)
{
	this->socket.bind();
	int optval = 1;
	this->socket.setup(SO_REUSEADDR, &optval, sizeof(optval));
	this->socket.setup(SO_REUSEPORT, &optval, sizeof(optval));
}

Server::~Server()
{
	for (Session* session: this->sessions)
	{
		delete session;
	}
	std::cerr << std::endl << "Server closed";
}

int Server::start()
{
	int activity;
	this->socket.listen();
	
	// initialize pollfd
	this->v_fds.clear();
	this->v_fds.reserve(this->sessions.size() + 1);
	for (const Session* session: this->sessions)
	{
		this->v_fds.push_back( {session->desc(), POLLIN, 0} );
	}
	this->v_fds.push_back( {this->socket.desc(), POLLIN, 0} );
	
	bool isRemove;
	while (true)
	{
		isRemove = false;
		// Get activity
		if ((activity = poll(this->v_fds.data(), this->v_fds.size(), -1)) == -1)
		{
			if (errno == EINTR)
			{
				return 0;
			} else
			{
				throwErrno("Failed poll");
			}
		}
		
		// Check new connection
		int lastIndex = this->v_fds.size() - 1;
		if ((this->v_fds[lastIndex].revents & POLLIN) == POLLIN)
		{
			try
			{
				int fd = this->socket.accept();
				Session* session = new Session(fd, this->processor);
				this->sessions.push_back(session);
				this->v_fds.push_back(this->v_fds[lastIndex]);
				if (this->startCmd.size() > 0)
				{
					this->v_fds[lastIndex] = {fd, POLLOUT, 0};
					session->pushSendBuffer(this->startCmd.c_str(), InetUtils::cmdSize(this->startCmd.c_str()), 0);
					do {} while (session->step(POLLOUT) != POLLIN);
				}
				this->v_fds[lastIndex] = {fd, POLLIN, 0};
				std::cerr << "New connection: " << session->logSocket() << std::endl;
			}
			catch (const std::runtime_error& ex)
			{
				std::cerr << "New connection error: " << ex.what() << std::endl;
				continue;
			}
		} else
		// Get new request
		{
			int i_fds = 0;
			for (
				this->iter = this->sessions.begin(); 
				this->iter != this->sessions.end(); 
				++(this->iter), ++i_fds)
			{
				Session* session = *(this->iter);
				this->v_fds[i_fds].events = session->step(this->v_fds[i_fds].revents);
				if (this->v_fds[i_fds].events == 0)
				{
					std::cerr << "Connection closed: " << session->logSocket() << std::endl;
					delete *(this->iter);
					this->sessions.erase(this->iter--);
					this->v_fds[i_fds].fd = -1;
					isRemove = true;
					continue;
				}
			}
		}
		// Remove disconnected client descriptors
		if (isRemove)
		{
			this->v_fds.erase(
				std::remove_if(
					this->v_fds.begin(), 
					this->v_fds.end(),
					[](const struct pollfd& fds) { 
						return fds.fd == -1;
					}), 
				this->v_fds.end());
		}
	}
	return 0;
}

void Server::setStartCmd(const char* startCmd)
{
	this->startCmd = startCmd;
}

void Server::setStartCmd(const InetUtils::ResponseTemplateType& cmdTemplate)
{
	this->startCmd = utils.makeCmd(cmdTemplate);
}

