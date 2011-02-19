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
	//конструктор с параметром - адрес сервера дл€ соеденени€
	HttpClient(LPCSTR serverName = NULL);

	//деструктор
	~HttpClient();

	//инициализаци€ WinInet
	bool openInternet();
	
	//закрытие дескриптора WinInet;
	void closeInternet();

	//создание соеденени€
	bool openConnection(LPCSTR serverName = NULL);

	//закрытие соеденени€
	void closeConnection();

	//отправка GET запроса
	bool get(LPCSTR actions, LPCSTR referrer = NULL);

	//отправка POST запроса
	bool post(LPCSTR actions, LPCSTR data, LPCSTR referrer = NULL);

	//закрыть текущий запрос
	void closeRequest();

	//получает ответ от сервера на запрос hRequest
	char* getData(char *buffer, DWORD szBuffer, DWORD *bytesRead = NULL);
	char* getData(DWORD *bytesRead = NULL);

	//возврашает размер ответа от сервера на запрос hRequest
	DWORD getDataSize();

	//возвращает последнюю ошибку
	DWORD getError()const;

	//установить заголовки
	void setHeaders(LPCSTR n_headersString);   

	//установить User-Agent
	void setAgentName(LPCSTR n_agentName);

private:

	//адрес сервера дл€ соеденени€
	LPSTR m_serverName;  

	//строка с заголовками
	LPSTR m_headersString;     

	//User-Agent
	LPSTR m_agentName;

	//дексриптор WinInet
	HINTERNET m_hInternet;

	//дескриптор соеденени€
	HINTERNET m_hConnection;

	//дескриптор текущего запроса
	HINTERNET m_hRequest;

	//последн€€ ошибка
	DWORD m_lastError;

	//вспомогательный буффер
	char* m_buffer;

	//размер вспомогательного буффера
	DWORD m_bufferSize;

  

	//копирование строки src в dest
	void copyStr(LPSTR &dest, LPCSTR src);

	//провер€ет условие на ошибку, если ошибка то сохранить в m_lastError
	bool checkError(bool f);

	//отправка запроса
	bool sendRequest(LPCSTR method, LPCSTR actions, LPCSTR data, LPCSTR referrer);

	//установить новый размер вспомогательного буффера
	void setBufferSize(DWORD bufferSize);


};

#endif