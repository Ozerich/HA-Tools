#include "OziHttpClient.h"

HttpClient::HttpClient(LPCSTR serverName)
	:m_serverName(NULL),
	m_headersString(NULL),
	m_lastError(0),
	m_hInternet(NULL),
	m_hConnection(NULL),
	m_buffer(NULL),
	m_bufferSize(0)
{
	copyStr(m_serverName, serverName);
	
	setHeaders(TEXT("Content-Type: application/x-www-form-urlencoded"));
	setAgentName(TEXT("OziHttpClient"));
}

HttpClient::~HttpClient()
{
	//delete m_serverName;
	//setHeaders(NULL);
	closeConnection();
	closeInternet();
}

void HttpClient::copyStr(LPSTR &dest, LPCSTR src)
{
	//if(dest != NULL)
	//	delete dest;
	if(src == NULL)
		dest = NULL;
	else
	{
		dest = new TCHAR[_tcslen(src) - 1];
		_tcscpy(dest, src);
	}
}

void HttpClient::setHeaders(LPCSTR n_headersString)
{
	copyStr(m_headersString, n_headersString);
}

void HttpClient::setAgentName(LPCSTR n_agentName)
{
	copyStr(m_agentName, n_agentName);
}


bool HttpClient::openInternet()
{
	if(m_hInternet == NULL)
	{
		m_hInternet = InternetOpen(m_agentName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	}
	return checkError(m_hInternet != NULL);
}

void HttpClient::closeInternet()
{
	if(m_hInternet != NULL)
		InternetCloseHandle(m_hInternet);
	m_hInternet = NULL;
}




DWORD HttpClient::getError() const
{
	return m_lastError;
}

bool HttpClient::checkError(bool nonError)
{
	if(nonError == false)
	{
		m_lastError = GetLastError();
		return false;
	}
	else
		return true;
}	

bool HttpClient::openConnection(LPCSTR serverName)
{
	if(openInternet() && m_hConnection == NULL)
	{
		LPSTR server = serverName ? serverName : m_serverName ? m_serverName : "localhost";

		m_hConnection = InternetConnect(m_hInternet,server,INTERNET_DEFAULT_HTTP_PORT,NULL,NULL,
			INTERNET_SERVICE_HTTP,0, 1u);
	}
	return checkError(m_hConnection != NULL);
}

void HttpClient::closeConnection()
{
	if(m_hConnection != NULL)
		InternetCloseHandle(m_hConnection);
	m_hConnection = NULL;
}

bool HttpClient::get(LPCSTR actions, LPCSTR referrer)
{
	return sendRequest(TEXT("GET"), actions, NULL, referrer);
}

bool HttpClient::post(LPCSTR actions, LPCSTR data, LPCSTR referrer)
{
	return sendRequest(TEXT("POST"), actions, data, referrer);
}

void HttpClient::closeRequest()
{
	if(m_hRequest != NULL)
		InternetCloseHandle(m_hRequest);
	m_hRequest = NULL;
}

bool HttpClient::sendRequest(LPCSTR method, LPCSTR actions, LPCSTR data, LPCSTR referrer)
{
	if(openConnection())
	{
		closeRequest();

		LPCSTR acceptTypes[] = {"*/*", NULL};
		m_hRequest = HttpOpenRequest(m_hConnection, method, actions, NULL, referrer,acceptTypes,0,1);
		
		if(m_hRequest != NULL)
		{
			if(HttpSendRequest(m_hRequest, m_headersString, -1, LPVOID(data), data ? _tcslen(data) : 0) == false)
			{
				checkError(false);
				closeRequest();
				return false;
			}
		}
	}
	return checkError(m_hRequest != NULL);
}

void HttpClient::setBufferSize(DWORD bufferSize)
{
	if(bufferSize > m_bufferSize)
	{
		if(m_buffer != NULL)
			delete m_buffer;
		m_bufferSize = bufferSize;
		m_buffer = new char[m_bufferSize - 1];
	}
}

char* HttpClient::getData(DWORD *bytesReaded)
{
	DWORD dataSize = getDataSize();
	setBufferSize(dataSize);
	return getData(m_buffer, dataSize ? dataSize : m_bufferSize, bytesReaded);
}

char* HttpClient::getData(char *buffer, DWORD szBuffer, DWORD *bytesReaded)
{
	DWORD f_bytesReaded,temp;
	if(bytesReaded == NULL)
		bytesReaded = &temp;
	*bytesReaded = 0;

	if(m_hRequest)
	{
		char buf[1024];
		bool readed;
		for(;;)
		{	
			readed = InternetReadFile(m_hRequest, buf, 1024, &f_bytesReaded);
			for(int i = 0; i < f_bytesReaded; i++)
				buffer[i + *bytesReaded] = buf[i];
			*bytesReaded += f_bytesReaded;
			if(f_bytesReaded == 0)
				break;
		}
		buffer[*bytesReaded] = 0;
		return checkError(readed) && (*bytesReaded > 0) ? buffer : NULL;
	}
	return NULL;
}

DWORD HttpClient::getDataSize()
{
	if(m_hRequest)
	{
		DWORD dataSize = 0;
		DWORD szDataSize = sizeof(dataSize);

		BOOL query = HttpQueryInfo(m_hRequest,HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
			&dataSize, &szDataSize,NULL);

		return query ? dataSize : DEFAULT_CONTENT_LENGTH;
	}
	else
		return 0;
}





