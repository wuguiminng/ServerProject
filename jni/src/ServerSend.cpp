/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerSend.cpp
*				File Identify: send feed back message to client
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#include "ServerSend.h"


int SendData::SendMessageToClient(int nClientSocketFd, int nCmd,\
	int nDataSize, const char caDataBuff[])
{
	if ((nClientSocketFd < 0) || (nCmd < 0) || (nDataSize < 0) || \
		(NULL == caDataBuff))
	{
		return -1;
	}

	int nRet = -1;
	char caSendBuff[1024] = {0};
	char *cpCmdBuff = IntToBytes(nCmd);
	bzero(caSendBuff, sizeof(caSendBuff));
	char *cpDataSizeBuff = IntToBytes(nDataSize);
	cout << "caDataBuff = " << caDataBuff << endl;
	
	strncpy(caSendBuff, "SDMC", sizeof(char)*4);
	strncpy(caSendBuff + 4, cpCmdBuff, sizeof(char)*4);
	strncpy(caSendBuff + 8, cpDataSizeBuff, sizeof(char)*4);
	strncpy(caSendBuff + 12, caDataBuff, sizeof(char)*nDataSize);

	nRet = send(nClientSocketFd, caSendBuff, 12 + nDataSize, 0);
	if (nRet < (12 + nDataSize))
	{
		perror("send message error");
		return -1;
	}
	
	bzero(caSendBuff, sizeof(caSendBuff));
	return 0;
}
