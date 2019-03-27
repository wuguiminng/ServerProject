/*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerRecv.h
*				File Identify: make a private protocol between server and client,
*						       then get message from client,
							   and check package is correct or not ,
							   then do some operations
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#ifndef SERVERRECV_H_
#define SERVERRECV_H_

#include "ServerSend.h"
#include "ServerFile.h"

class RecvData
{
public:
	RecvData()
	{
		
	}
	

	virtual ~RecvData()
	{
		
	}
	

	int CheckPacket(const char caBuff[], int nPackageLength)
	{
		return m_Tool.CheckPacket(caBuff, nPackageLength);
	}


	int GetCmd(const char caBuff[])
	{
		return m_Tool.GetCmd(caBuff);
	}


	int GetDataSize(const char caBuff[])
	{
		return m_Tool.GetDataSize(caBuff);
	}


	int GetClient_IP_Port(int nSocketFd, char *cpId)
	{
		return m_Tool.GetClient_IP_Port(nSocketFd, cpId);
	}

	void SendMessageToClient(int nClientSocketFd, int nCmd, int nDataSize,\
		const char caDataBuff[])
	{
		m_SendData.SendMessageToClient(nClientSocketFd, nCmd, nDataSize, caDataBuff);
	}

	void GetMessageFromClient(int nClientSocketFd);

	int RecvUrl(int nClientSocketFd, const char *caId, int nDataSize);

private:
	Xml m_Xml;
	Json m_Json;
	Url m_Url;
	Tool m_Tool;
	SendData m_SendData;
	OtherFile m_OtherFile;
	
};

#endif
