/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerFile.h
*				File Identify: save file from Client and parse xml/json/Url
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#ifndef SERVERFILE_H_
#define SERVERFILE_H_

#include "ServerSend.h"
#include "ServerAes.h"
#include "tinyxml.h"
#include "cJSON.h"
#include "curl.h"

class File
{
public:
	File() 
	{
		
	}
	
	virtual ~File() 
	{
		
	}
	
	virtual void GetPath(void)
	{
		
	}
	
	int Aes_Decrypt_File(const unsigned char *ucp_EnBuff, int nEnSize,\
		unsigned char *ucp_DeBuff)
	{
		return m_Aes.AesDecryptFile(ucp_EnBuff, nEnSize, ucp_DeBuff);
	}

	int SaveFile(int nClientSocketFd, const char *cpPath, int nCmd,\
		int nDataSize, int nEncrypt);
	
private:
	FILE *m_pFp;
	Aes m_Aes;
};


class Xml : public File
{
public:
	Xml()
	{
		
	}
	
	virtual ~Xml()
	{
		
	}
	
	void XmlSendMessageToClient(int nClientSocketFd, int nCmd,\
		int nDataSize, const char caDataBuff[])
	{
		m_SendData.SendMessageToClient(nClientSocketFd, nCmd, nDataSize, caDataBuff);
	}

	char *GetPath(const char *cpId);
	
	int ParseXML(int nClientSocketFd, const char *cpId, int nDataSize,\
		int nEncrypt);
	
private:
	TiXmlDocument m_XmlDocument;
	SendData m_SendData;
};


class Json : public File
{
public:
	Json()
	{
		
	}
	
	virtual ~Json()
	{
		
	}
	
	void JsonSendMessageToClient(int nClientSocketFd, int nCmd,int nDataSize,\
		const char caDataBuff[])
	{
		m_SendData.SendMessageToClient(nClientSocketFd, nCmd, nDataSize, caDataBuff);
	}

	char *GetPath(const char *cpId);
	
	int ParseJSON(int nClientSocketFd, const char *cpId, int nDataSize,\
		int nEncrypt);
	
private:
	char *m_cpJsonStr;
	SendData m_SendData;
};


class OtherFile : public File
{
public:
	OtherFile()
	{
		
	}
	
	virtual ~OtherFile()
	{
		
	}
	
	char *GetPath(const char *cpId, int cmd);
	int SaveFile(int nClientSocketFd, const char *cpId, int nCmd,\
		int nDataSize, int nEncrypt);
	
private:

};


class Url : public File
{
public:
	Url()
	{
		
	}
	
	virtual ~Url()
	{
		
	}
	
	char *GetPath(const char *cpId);
	
	int VisitURL(const char *cpId, int ndDataSize, const char caBuff[]);
	
	int DownloadFileFromUrl(CURL *pEasy_Handle, const char *cpRemotePath,\
		const char *cpLocalPath);
	
	size_t GetContentLengthFunc(void *pTr, size_t Size, size_t Nmemb,\
		void *pStream);
	
private:	
	FILE *m_pFp;
	CURLcode m_Ret;
	CURL *m_pEasy_Handle;
	curl_off_t m_Local_File_Len;
	struct stat m_File_Info;
	static size_t WirteFunc(void *pTr, size_t Size, size_t Nmemb, void *pStream);
};

#endif
