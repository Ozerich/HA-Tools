#ifndef PHPBB_H
#define PHPBB_H

#include "OziHttpClient.h";
#include <string>
#include <vector>

using namespace std;

class PhpBB
{
private:
	HttpClient http;
	wstring m_server;
	wstring m_login;
	wstring m_sid;
	wstring m_password;
public:
	enum AuthorizedState
	{
		AuthorizedOk = 1,       //����������� ������ �������
		UserNoFound = 2,        //������������ �� ������
		IncorrectPassword = 3,  //������������ ������
		NoLogin = 4,            //�� ������ �����
		NoPassword = 5,			//�� ������ ������
		AuthorizedError = 6     //������ �����������
	};

	PhpBB(const wstring &server);
	~PhpBB();

	AuthorizedState authorize(const wstring &login, const wstring &password);
	void write_message(const int topic_id, const wstring &text);
};


#endif