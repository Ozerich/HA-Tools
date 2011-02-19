#ifndef OZI_HTTP_CLIENT_H
#define OZI_HTTP_CLIENT_H

#include "windows.h"
#include "wininet.h"
#pragma comment(lib, "wininet")
#include "tchar.h"

const int DEFAULT_CONTENT_LENGTH = 1000000;

class HttpClient
{
public:
	//����������� � ���������� - ����� ������� ��� ����������
	HttpClient(LPCSTR serverName = NULL);

	//����������
	~HttpClient();

	//������������� WinInet
	bool openInternet();
	
	//�������� ����������� WinInet;
	void closeInternet();

	//�������� ����������
	bool openConnection(LPCSTR serverName = NULL);

	//�������� ����������
	void closeConnection();

	//�������� GET �������
	bool get(LPCSTR actions, LPCSTR referrer = NULL);

	//�������� POST �������
	bool post(LPCSTR actions, LPCSTR data, LPCSTR referrer = NULL);

	//������� ������� ������
	void closeRequest();

	//�������� ����� �� ������� �� ������ hRequest
	char* getData(char *buffer, DWORD szBuffer, DWORD *bytesRead = NULL);
	char* getData(DWORD *bytesRead = NULL);

	//���������� ������ ������ �� ������� �� ������ hRequest
	DWORD getDataSize();

	//���������� ��������� ������
	DWORD getError()const;

	//���������� ���������
	void setHeaders(LPCSTR n_headersString);   

	//���������� User-Agent
	void setAgentName(LPCSTR n_agentName);

private:

	//����� ������� ��� ����������
	LPSTR m_serverName;  

	//������ � �����������
	LPSTR m_headersString;     

	//User-Agent
	LPSTR m_agentName;

	//���������� WinInet
	HINTERNET m_hInternet;

	//���������� ����������
	HINTERNET m_hConnection;

	//���������� �������� �������
	HINTERNET m_hRequest;

	//��������� ������
	DWORD m_lastError;

	//��������������� ������
	char* m_buffer;

	//������ ���������������� �������
	DWORD m_bufferSize;

  

	//����������� ������ src � dest
	void copyStr(LPSTR &dest, LPCSTR src);

	//��������� ������� �� ������, ���� ������ �� ��������� � m_lastError
	bool checkError(bool f);

	//�������� �������
	bool sendRequest(LPCSTR method, LPCSTR actions, LPCSTR data, LPCSTR referrer);

	//���������� ����� ������ ���������������� �������
	void setBufferSize(DWORD bufferSize);


};

#endif