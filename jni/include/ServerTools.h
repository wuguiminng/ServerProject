/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerTools.h
*				File Identify: some tool functions to help PackageParse to
						       parse data easier
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#ifndef SERVERTOOLS_H_
#define SERVERTOOLS_H_

#include "ServerPublic.h"

class Tool
{
public:

	Tool() 
	{
		
	}
	
	virtual ~Tool()
	{
		
	}

	int CheckPacket(const char caBuff[], int nPackageLength);
	
	int CheckHead(const char caBuff[]);

	int GetCmd(const char caBuff[]);
	
	int GetDataSize(const char caBuff[]);
	
	int BytesToInt(const char caBuff[]);

	char *IntToBytes(int nNum);

	int GetClient_IP_Port(int nSocketFd, char *cpId);
	
private:
	
	int m_nDataSize;
	int m_nNum;
	char *m_cBuff;
};

#endif 
