#include "phpbb.h";

PhpBB::PhpBB(const wstring &server):m_server(server)
{
	http.openInternet();
	http.openConnection(m_server.c_str());
	int a = http.getError();
	int b = a;
}

PhpBB::~PhpBB()
{
	http.closeConnection();
	http.closeInternet();
}

PhpBB::AuthorizedState PhpBB::authorize(const wstring &login, const wstring &password)
{
	if(login.empty())
		return NoLogin;
	if(password.empty())
		return NoPassword;

	m_login = login;
	m_password = password;

	wstring action = L"username=" + login + L"&password=" + password + L"&redirect=&login=Вход";
	if(http.post(L"forum/login.php", action.c_str()))
	{
		wstring str =  http.getData();
		if(str.find(L"Вы ввели неверное/неактивное имя пользователя или неверный пароль") != -1)
			return IncorrectPassword;
		str.assign(str.begin() + str.find(L"sid=")+4, str.end());
		m_sid = str.assign(str.begin(), str.begin() + str.find('"'));
	}
}

void PhpBB::write_message(const int topic_id, const wstring &text)
{
	wchar_t topic[10];
	_itow(topic_id, topic, 10);
	wstring action = L"message="+text+L"&attach_sig=on&mode=reply&sid="+m_sid+L"&disable_bbcode=&disable_smilies=&t="+topic+L"&post=Отправить";
	if(http.post(TEXT("forum/posting.php"), action.c_str()))
	{
		wstring str =  http.getData();
		int a = 0;
	}
}                      
