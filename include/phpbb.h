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
		AuthorizedOk = 1,       //Авторизация прошла успешно
		UserNoFound = 2,        //Пользователь не найден
		IncorrectPassword = 3,  //Неправильный пароль
		NoLogin = 4,            //Не введен логин
		NoPassword = 5,			//Не введен пароль
		AuthorizedError = 6     //Ошибка авторизации
	};

	PhpBB(const wstring &server);
	~PhpBB();

	AuthorizedState authorize(const wstring &login, const wstring &password);
	void write_message(const int topic_id, const wstring &text);
};


#endif