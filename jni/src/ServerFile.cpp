/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerFile.cpp
*				File Identify: save file from Client and parse xml/json/Url
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#include "ServerFile.h"


int File::SaveFile(int nClientSocketFd, const char *cpPath, int nCmd,\
	int nDataSize, int nEncrypt)
{
	if ((NULL == cpPath) || (nCmd < 0) || (nDataSize < 0) || (nEncrypt < 0))
	{
		perror("function SaveFile error");
		return -1;
	}
	
	int nRetSize = 0;
	int nLength = 0;
	char caBuffer[MAX_SIZE] = {0};
	
    FILE *fp = fopen(cpPath, "ab+");   
    if(NULL == fp)   
    {   
        perror("open file error");
        return -1;   
    }   
	
	if (nEncrypt > 1)//decryption
	{
		cout << " cyphertext transmitting " << endl;
		
		int nRealLength = nDataSize;
		bzero(caBuffer, sizeof(caBuffer));
		unsigned char ucaDeBuff[MAX_SIZE];
		memset((char*)ucaDeBuff, 0, MAX_SIZE);
		cout << "fileSize = " << nDataSize << endl;
	
		while((nLength = recv(nClientSocketFd, caBuffer, MAX_SIZE, 0)) > 0)
		{   
			nRetSize = Aes_Decrypt_File((unsigned char *)caBuffer, nLength, ucaDeBuff);
			fwrite(ucaDeBuff, sizeof(char), nRetSize, fp);
			nRealLength -=  nLength;

			memset((char*)ucaDeBuff, 0, MAX_SIZE);
			bzero(caBuffer, MAX_SIZE);
			
			if (0 == nRealLength)
			{
				cout << "Receive File: " << cpPath;
				cout << " From Client Successful!" << endl;
				break;
			}
		} 
	}
	else if(1 == nEncrypt) //not decryption
	{
		cout << "not cyphertext transmitting " << endl;

		nLength = 0;
		int nRecvLength = 0;
		memset(caBuffer, 0, MAX_SIZE); 
		
		while((nLength = recv(nClientSocketFd, caBuffer, MAX_SIZE, 0)) > 0)
		{   
			fwrite(caBuffer, sizeof(char), nLength, fp);
			nRecvLength += nLength;
			memset(caBuffer, 0, MAX_SIZE);
			
			if (nRecvLength == nDataSize)
			{
				cout << "Receive File: " << cpPath;
				cout << " From Client Successful!\n" << endl;
				break;
			}
		}
	}
	fclose(fp);   	
	return 0;
}


char *Xml::GetPath(const char *cpId)
{
	char *cpPath = (char *)malloc(sizeof(char) * 40);
	sprintf(cpPath, "./%s/joey.xml",cpId);
	return cpPath;
}


int Xml::ParseXML(int nClientSocketFd, const char *cpId,\
	int nDataSize, int nEncrypt)
{
	if ((nClientSocketFd < 0) || (NULL == cpId)
		|| (nDataSize < 0) || (nEncrypt < 0))
	{
		perror("function bParseXml error");
		return -1;
	}
	
	char caBuff[MAX_SIZE] = {0};
	bzero(caBuff, sizeof(caBuff));
	char *cpPath = GetPath(cpId);

	if (0 == SaveFile(nClientSocketFd, cpPath, 2, nDataSize, nEncrypt))
	{
		cout << "save file success " << endl;
	}
	
	if (m_XmlDocument.LoadFile(cpPath))
	{
		TiXmlElement *xmlRoot = m_XmlDocument.RootElement();
		if (NULL != xmlRoot)
		{
			TiXmlElement *xmlNode1Name = xmlRoot->FirstChildElement("name");
			const char *node1Name = xmlNode1Name->GetText();
			const char *node1AttId = xmlNode1Name->Attribute("id");
			
			cout << "==================parseing xml data=======================\n";
			cout << node1Name << "'s id is " << node1AttId << endl << endl;
			
			TiXmlElement *xmlNode2Stu = xmlRoot->FirstChildElement("Student");
			TiXmlElement *xmlNode2_Name = xmlNode2Stu->FirstChildElement("name");
			TiXmlElement *xmlNode2_Classes = xmlNode2Stu->FirstChildElement("classes");
			const char *node2Name = xmlNode2_Name->GetText();
			const char *node2Classes = xmlNode2_Classes->GetText();
			
			cout << "The Student " << node2Name << " studing in ";
			cout << node2Classes << " class ." << endl;
			cout << "================================================================\n";
			cout << "==================parse xml data finished=======================\n";
			cout << "================================================================\n\n";
			
			sprintf(caBuff, "%s's id is %s, and The student %s studing in %s class",\
				node1Name, node1AttId, node2Name, node2Classes);
		}
	}
	
	nDataSize = strlen(caBuff);
	XmlSendMessageToClient(nClientSocketFd, 2, nDataSize, caBuff);
	
	if (NULL != cpPath)
	{
		free(cpPath);
		cpPath = NULL;
	}

	return 0;
}


char *Json::GetPath(const char *cpId)
{
	char *cpPath = (char *)malloc(sizeof(char) * 40);
	sprintf(cpPath, "./%s/joey.json",cpId);
	return cpPath;
}



int Json::ParseJSON(int nClientSocketFd, const char *cpId, int nDataSize,\
	int nEncrypt)
{
	if ((nClientSocketFd < 0) || (NULL == cpId)
		|| (nDataSize < 0) || (nEncrypt < 0))
	{
		perror("function bParseJson error");
		return -1;
	}
	
	char caBuff[MAX_SIZE] = {0};
	char *cpPath = GetPath(cpId);
	bzero(caBuff, sizeof(caBuff));

	if (0 == SaveFile(nClientSocketFd, cpPath, 3, nDataSize, nEncrypt))
	{
		cout << "save file success " << endl;
	}
	
	FILE *fp = fopen(cpPath, "r");
	m_cpJsonStr = (char *)malloc(sizeof(char *)*nDataSize);
	fread(m_cpJsonStr, 1, nDataSize, fp);
	fclose(fp);
	
	cJSON *root = cJSON_Parse(m_cpJsonStr);
	if (NULL != root)
	{
		cJSON *temp = cJSON_GetObjectItem(root, "people");
		if (NULL != temp)
		{
			int nCount = cJSON_GetArraySize(temp);
			for (int index = 0; index < nCount; index++)
			{
				cJSON *json = cJSON_GetArrayItem(temp, index);
				if (NULL != json)
				{
					cJSON *firstName = NULL;
					cJSON *lastName = NULL;
					cJSON *email = NULL;
					cJSON *age = NULL;
					cJSON *height = NULL;
					
					char *cpFName = NULL;
					char *cpLName = NULL;
					char *cpEmail = NULL;
					int nAge = 0;
					double dHeight = 0.0;

					firstName = cJSON_GetObjectItem(json, "firstName");
					cout << "==================parseing json data==========\n";
					if (NULL != firstName)
					{
						cout << "my firstName is ";
						cout << firstName->valuestring << endl;
						cpFName = firstName->valuestring;
					}

					lastName = cJSON_GetObjectItem(json, "lastName");
					if (NULL != lastName)
					{
						cout << "my lastName is ";
						cout << lastName->valuestring << endl;
						cpLName = lastName->valuestring;
					}

					email = cJSON_GetObjectItem(json, "email");
					if (NULL != email)
					{
						cout << "and my email is ";
						cout << email->valuestring << endl;
						cpEmail = email->valuestring;
					}

					age = cJSON_GetObjectItem(json, "age");
					if (NULL != age)
					{
						cout << "I am " << age->valueint;
						cout << " years old" << endl;
						nAge = age->valueint;
					}

					height = cJSON_GetObjectItem(json, "height");
					if (NULL != height)
					{
						cout << "and I am " << height->valuedouble;
						cout << " m" << endl;
						dHeight = height->valuedouble;
					}
					cout << "===================================================\n";
					cout << "===========parse json data finished================\n";
					cout << "=================================================\n\n";
					
					sprintf(caBuff,
						"hello, my name is %s.%s, and I'm %d years old and %lf m, and email address is %s",\
						cpFName, cpLName, nAge, dHeight, cpEmail);
					
					nDataSize = strlen(caBuff);
					JsonSendMessageToClient(nClientSocketFd, 3, nDataSize, caBuff);
					bzero(caBuff, sizeof(caBuff));
				}
			}
		}
	}
	
	cJSON_Delete(root);
	root = NULL;
	if (NULL != cpPath)
	{
		free(cpPath);
		cpPath = NULL;
	}

	return 0;
}


char *OtherFile::GetPath(const char *cpId, int nCmd)
{
	char *cpPath = (char *)malloc(sizeof(char) * 40);
	if (4 == nCmd)
	{
		sprintf(cpPath, "./%s/joey.bmp",cpId);
	}
	else if (5 == nCmd)
	{
		sprintf(cpPath, "./%s/joey.txt",cpId);	
	}
	
	return cpPath;
}


int OtherFile::SaveFile(int nClientSocketFd, const char *cpId, int nCmd,\
	int nDataSize, int nEncrypt)
{
	char *cpPath = GetPath(cpId, nCmd);
	int nRet = File::SaveFile(nClientSocketFd, cpPath, nCmd,\
		nDataSize, nEncrypt);

	if (NULL != cpPath)
	{
		free(cpPath);
		cpPath = NULL;
	}
	return nRet;
}


char *Url::GetPath(const char *cpId)
{
	char *cpPath = (char *)malloc(sizeof(char) * 40);
	sprintf(cpPath, "./%s/joey.mp4",cpId);
	return cpPath;
}


int Url::VisitURL(const char *cpId, int ndDataSize, const char caBuff[])
{
	if ((NULL == cpId) || (ndDataSize < 0) || (NULL == caBuff))
	{
		perror("function parseUrl error");
		return -1;
	}
	
	char caUrl[100] = {0};
	bzero(caUrl, sizeof(caUrl));
	strncpy(caUrl, caBuff, sizeof(char)*ndDataSize);
	curl_global_init(CURL_GLOBAL_ALL);
	m_pEasy_Handle = curl_easy_init();
	char *cpPath = GetPath(cpId);
	DownloadFileFromUrl(m_pEasy_Handle, caUrl, cpPath);
	curl_easy_cleanup(m_pEasy_Handle);
	curl_global_cleanup();

	return 0;
}
	

int Url::DownloadFileFromUrl(CURL *pEasy_Handle, const char *cpRemotePath,\
	const char *cpLocalPath)
{
	m_Local_File_Len = -1;
	m_Ret = CURLE_GOT_NOTHING;
	
	if (stat(cpLocalPath, &m_File_Info) == 0)
	{
		m_Local_File_Len = m_File_Info.st_size;
	}

	m_pFp = fopen(cpLocalPath, "ab+");
	if (NULL == m_pFp)
	{
		perror(NULL);
		return -1;
	}
	
	curl_easy_setopt(pEasy_Handle, CURLOPT_URL, cpRemotePath);
	curl_easy_setopt(pEasy_Handle, CURLOPT_WRITEFUNCTION, WirteFunc);
	curl_easy_setopt(pEasy_Handle, CURLOPT_WRITEDATA, m_pFp);
	curl_easy_setopt(pEasy_Handle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(pEasy_Handle, CURLOPT_VERBOSE, 1L);
	m_Ret = curl_easy_perform(pEasy_Handle); 

	fclose(m_pFp);

	if (m_Ret == CURLE_OK)
	{
		return 0;
	}
	else
	{
		fprintf(stderr, "%s\n", curl_easy_strerror(m_Ret));
		return -1;
	}
}

	
size_t Url::GetContentLengthFunc(void *pTr, size_t Size, size_t Nmemb,\
	void *pStream)
{
	int nRet;
	int nLen = 0;

	nRet = sscanf((const char *)pTr, "Content_Length: %d\n", &nLen);
	if (nRet)
	{
		*((long *)pStream) = nLen;
	}

	return Size * Nmemb;
}

	
size_t Url::WirteFunc(void *pTr, size_t Size, size_t Nmemb, void *pStream)
{
	return fwrite(pTr, Size, Nmemb, (FILE *)pStream);
}
