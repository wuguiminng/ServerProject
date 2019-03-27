/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerSocket.h
*				File Identify: init server socket, bind server's port and ip address,
							   listen client, accept client's connection and
							   create a pthread to recv message from client
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "ServerRecv.h"

#define MAX_LINK 10

class Server
{
public:
	Server() {}
	
	virtual ~Server() {}
	
	void ServerInit(void);

	void AcceptClient(void);
	
	static void* PthreadFunction(void *arg);

private:
	socklen_t m_Length;
	pthread_t m_Thread_Read;
	
	int m_nServerSocketFd;
	int m_nClientSocketFd;
	
	struct sockaddr_in m_ServerAddr;
	struct sockaddr_in m_ClientAddr;

};

#endif
