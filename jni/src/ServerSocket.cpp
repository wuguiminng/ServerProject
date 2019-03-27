/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerSocket.cpp
*				File Identify: init server socket, bind server's port and ip address, 
							   listen client, accept client's connection and 
							   create a pthread to recv message from client
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#include "ServerSocket.h"
   

void Server::ServerInit(void)
{
	int nRet = -1;
	
	m_nServerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == m_nServerSocketFd)
	{
		perror("create server socket error");
		exit(0);
	}
	
	bzero(&m_ServerAddr, sizeof(m_ServerAddr));
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port = htons(atoi("6000"));;
	m_ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	nRet = bind(m_nServerSocketFd, (struct sockaddr*)&m_ServerAddr,\
		sizeof(m_ServerAddr));
	if (-1 == nRet)
	{
		perror("bind server ip and port error");
		exit(0);
	}
	
	nRet = listen(m_nServerSocketFd, MAX_LINK);
	if (-1 == nRet)
	{
		perror("listen client error");
		exit(0);
	}
}


void Server::AcceptClient(void)
{
	bzero(&m_ClientAddr, sizeof(m_ClientAddr));
	m_Length = sizeof(m_ClientAddr);
	
	while(1)
	{
		m_nClientSocketFd = accept(m_nServerSocketFd,\
			(struct sockaddr*)&m_ClientAddr, &m_Length);
		if (-1 == m_nClientSocketFd)
		{
			perror("accept client error");
			exit(0);
		}
		
		int nPort = 0;
		char caAddr[15] = {0};
		bzero(caAddr, sizeof(caAddr));
		
		nPort = ntohs(m_ClientAddr.sin_port);
		memcpy(caAddr, inet_ntoa(m_ClientAddr.sin_addr),\
			strlen(inet_ntoa(m_ClientAddr.sin_addr)));
		cout << "Clinet " << m_nClientSocketFd << " connected," ;
		cout << " port = " << nPort << " ,addr = " << caAddr << endl;
		
		pthread_create(&m_Thread_Read, NULL, PthreadFunction,\
			(void *)&m_nClientSocketFd);
		
		pthread_detach(m_Thread_Read);
	}
}


void* Server::PthreadFunction(void *arg)
{
	int nClientSocketFd = *((int*)arg);
	RecvData recvData;
	recvData.GetMessageFromClient(nClientSocketFd);
	return NULL;
}

