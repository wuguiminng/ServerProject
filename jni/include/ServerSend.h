/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerSend.h
*				File Identify: send feed back message to client
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#ifndef SERVERSEND_H_
#define SERVERSEND_H_

#include "ServerTools.h"

class SendData
{
public:
	SendData()
	{
		
	}
	
	virtual ~SendData()
	{
		
	}
	
	char *IntToBytes(int nNum)
	{
		return m_Tool.IntToBytes(nNum);
	}

	int SendMessageToClient(int nClientSocketFd, int nCmd, int nDataSize,\
		const char caDataBuff[]);
	
private:
	Tool m_Tool;
};

#endif
