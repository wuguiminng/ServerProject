/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerRecv.cpp
*				File Identify: make a private protocol between server and client,
*						       then get message from client,
							   and check package is correct or not , 
							   then do some operations
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#include "ServerRecv.h"


void RecvData::GetMessageFromClient(int nClientSocketFd)
{
	int nFlag = -2;
	int nAgree = -1;
	int nPackageLength = 0;
	char caId[25] = {0};
	char caBuff[MAX_SIZE];
	bzero(caBuff, sizeof(caBuff));
	
	while(1)
	{
		if (-2 == nFlag)
		{
			nFlag = -1;
			bzero(caId, sizeof(caId));
			bzero(caBuff, MAX_SIZE);
			nPackageLength = recv(nClientSocketFd, caBuff, 12, 0);
			
			if ((0 == GetClient_IP_Port(nClientSocketFd, caId)))
			{
				cout << "Client's IP and port is " << caId << endl;
			}
			
			if ((CheckPacket(caBuff, nPackageLength)) > 0)
			{
				int nCmd = GetCmd(caBuff);
				int nDataSize = GetDataSize(caBuff+8);
				cout << "nDataSize = " << nDataSize << endl;
				
				switch(nCmd)
				{
					case 0: //request connection
					{
						cout << "Client " << nClientSocketFd;
						cout << "(disagree:0)-(no encryption:1)-(encryption:2):" << endl;
						cin >> nAgree;
						char caAgreeBuff[2] = {0};
						sprintf(caAgreeBuff, "%d", nAgree);
						SendMessageToClient(nClientSocketFd, nCmd, 1, caAgreeBuff);
						break;
					}
					case 1:
					{
						cout << "Client " << nClientSocketFd << " disconnected\n";
						pthread_exit(NULL); 
						break;
					}
					case 2: //xml
					case 3: //json
					{
						if (2 == nCmd)
						{
							nFlag = m_Xml.ParseXML(nClientSocketFd, caId, nDataSize, nAgree);
						}
						else if (3 == nCmd)
						{
							nFlag = m_Json.ParseJSON(nClientSocketFd, caId, nDataSize, nAgree);
						}
						break;
					}
					case 4: // bmp
					case 5: // txt
					case 6: // url
					{
						if ((4 == nCmd) || (5 == nCmd))
						{
							nFlag = m_OtherFile.SaveFile(nClientSocketFd, caId, nCmd,\
									nDataSize, nAgree);
						}
						else if (6 == nCmd)
						{
							nFlag = RecvUrl(nClientSocketFd, caId, nDataSize);
						}
						
						if (-1 == nFlag)
						{
							char caStrcmd[2] = "2";
							SendMessageToClient(nClientSocketFd, nCmd, 1, caStrcmd);
						}
						else if(0 == nFlag)
						{
							char caStrcmd[2] = "1";
							SendMessageToClient(nClientSocketFd, nCmd, 1, caStrcmd);
						}
						break;
					}
					default:
					break;
				}
				bzero(caBuff, sizeof(caBuff));
				nFlag = -2;
			}
			else
			{
				nFlag = -1;
			}
		}
		bzero(caBuff, sizeof(caBuff));
	}
}

int RecvData::RecvUrl(int nClientSocketFd, const char *caId, int nDataSize)
{
	char caUrl[100];
	bzero(caUrl, sizeof(caUrl));
	recv(nClientSocketFd, caUrl, nDataSize, 0);
	int nFlag = m_Url.VisitURL(caId, nDataSize, caUrl);
	return nFlag;
}



