/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerTools.cpp
*				File Identify: some tool functions to help PackageParse to 
						       parse data easier
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#include "ServerTools.h"


int Tool::CheckPacket(const char caBuff[], int nPackageLength)
{
	if ((NULL == caBuff) || (nPackageLength < 0))
	{
		return -1;
	}
	
	int nRet = -1;
	int nDataSize = -1;
	nRet = CheckHead(caBuff);
	if (-1 == nRet)
	{
		return -1;
	}
	
	if (12 == nPackageLength)
	{
		nDataSize = GetDataSize(caBuff);
		if (nDataSize <= 0)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	cout << "Packet acceptance is incomplete\n";
	
	return -1;
}
	

int Tool::CheckHead(const char caBuff[])
{
	if (NULL == caBuff)
	{
		return -1;
	}
	
	int nRet = -1;
	nRet = strncmp(caBuff, "SDMC", 4);
	cout << "SDMC = " << caBuff << endl;
	if (0 == nRet)
	{
		return 0;
	}
	
	cout << "Package head is not 'SDMC' " << endl;
	
	return -1;
}
	

int Tool::GetCmd(const char caBuff[])
{
	if (NULL == caBuff)
	{
		return -1;
	}
	
	int nCmd = BytesToInt(caBuff + 4);
	
	if ((nCmd >= 0) && (nCmd <= 6))
	{
		return nCmd;
	}
	
	cout << "Package cmd is not correct " << endl;
	return -1;
}


int Tool::GetDataSize(const char caBuff[])
{
	if (NULL == caBuff)
	{
		return -1;
	}
	
	m_nDataSize = BytesToInt(caBuff);
	cout << "m_dataSize = " <<  m_nDataSize << endl;

	if (m_nDataSize > 0)
	{
		return m_nDataSize;
	}
	
	cout << "Package dataSize is not correct " << endl;
	return -1;
}


int Tool::BytesToInt(const char caBuff[])
{
	int nNum = 0;
	int index = 0;

	for (index = 0; index < 4; index++)
	{
		nNum <<= 8;
		nNum |= (caBuff[index] & 0xff);
	}
	
    return nNum;
}
	

char *Tool::IntToBytes(int nNum)
{
	char *cpBytes = (char*)malloc(sizeof(int) + 1);
	int index = 0;

	for(index = 0; index < 4; index++)
	{
		int nSet = 32 - (index + 1) * 8;
		cpBytes[index] = (char) ((nNum >> nSet) & 0xff);
	}
	cpBytes[index] = '\0';
	
	return cpBytes;
}
	

int Tool::GetClient_IP_Port(int nSocketFd, char *cpId)
{
	if ((nSocketFd < 0) || (NULL == cpId))
	{
		perror("function GetClient_IP_Port error");
		return -1;
	}
	
	char caSysCmd[35] = {0};
	struct sockaddr_in ClientAddr;
	
	bzero(&ClientAddr, sizeof(ClientAddr));
	socklen_t Length = sizeof(ClientAddr);
	if(!getpeername(nSocketFd, (struct sockaddr *)&ClientAddr, &Length))
	{
		sprintf(cpId, "%s:%d", inet_ntoa(ClientAddr.sin_addr),\
			ntohs(ClientAddr.sin_port));
	}
	
	bzero(caSysCmd, sizeof(caSysCmd));
	sprintf(caSysCmd, "mkdir %s", cpId);
	if (access(cpId, 0) != 0) 
	{
		system(caSysCmd);
	}
	
	return 0;
}
