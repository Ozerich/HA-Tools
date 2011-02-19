#ifndef HA_H
#define HA_H

#include "OziHttpClient.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "DateTime.h"
#include <windows.h>
#include <time.h>

using namespace std;

const int AUTHORIZED_OK = 1;
const int INCORRECT_PASSWORD = 2;

enum StatPage
{
	WorldPage = 1,
	CountryPage = 2,
	LeaguePage = 3
};

class Player
{
	friend ostream& operator <<(ostream &, const Player &);
public:

	enum PlayerTransferState      //Трансферное положение игрока
	{
		NoSale = 1,       //Не продаётся
		Free = 2,          //Уволен
		Saled = 3,         //Трансфер
		Unrestricted = 4  //Незащищенный
	};


	int id;
	string name;
	int age;
	string country;
	int contract;
	int team;
	int injuryLength;
	int salary;
	pair<bool, int>quality;
	pair<bool, int>potential;
	int abilityIndex;
	int loyalty;
	int satisfaction;
	int weeksInTeam;
	int stars;
	pair<int, int>goalie;
	pair<int, int>speed;
	pair<int, int>defense;
	pair<int, int>strength;
	pair<int, int>offense;
	pair<int, int>self_control;
	pair<int, int>shooting;
	pair<int, int>form;
	pair<int, int>passing;
	pair<int, int>expirience;

	int energy;
	int lastTraining;

	PlayerTransferState playerTransferState;

	Player();

	//ощищает данные игрока
	void reset();

};

class Team
{
	friend ostream& operator<<(ostream &, const Team &);
public:

	int id;
	string name;
	int manager;
	DateTime found;
	int playersCount;
	pair<int, int>league;
	string stadiumName;
	int stadiumCapacity;
	int trainingArea;
	int regenerationArea;
	int fansArea;
	int scoutingArea;
	int marketingArea;


	Team();


	void reset();
	
};

class TransferFilter;
class Manager
{
	friend ostream& operator<<(ostream &, const Manager &);
public:
	string name;
	int team;
	string country;
	int id;
	DateTime registrationDateTime;
	int points;
	bool online;
	vector<DateTime>logins;

	void reset();
	
	Manager();
};


const int SATISFACTIONS_COUNT = 9;
const string SATISFACTIONS[SATISFACTIONS_COUNT] = {"rebellious", "doomy", "dis-satisfied", "uneasy", "neutral", "pleased",
	"satisfied", "happy", "devoted"};



class League
{
public:
	string country;
	int league;
	int division;
};

class TrainingPlayer
{
public:
	Player player;
	int trainingValue;


};

class Parser
{
public:
	
	//Читает html-код страницы с игроком и возвращает данные игрока
	static void parsePlayer(const string &html, Player &player);
	
	//Возвращает значение поля которое относится к атрибуту attr для игрока
	static string getPlayerAttribure(const string &html, const string &attr);

	//Извлекает из строки str число, которое начинается с позиции beg и заканчивается перед endChar
	static int getNumber(const string &str, int beg = 0, const char endChar = ' ');

	//Разделяет двойной атрибут(картинка + значение) на две части
	static void parseDoubleAttribute(const string &str, string &left, string &right);

	//возвращает числовое значение настроения
	static int getSatisfaction(const string &str);

	//Возвращает количество звёзд у игрока
	static int getStarsCount(const string &str);

	//Возвращает готовую пару для скилла
	static pair<int,int> getPlayerSkill(const string &str);

	//Читает html-код страницы с командой и возвращает данные команды
	static void parseTeam(const string &html, Team &team);

	//Читает html-код страницы с постройками и возвращает данные построек
	static void parseTeamBuildings(const string &html, Team &team);

	//Возвращает значение поля которое относится к атрибуту attr для команды
	static string getTeamAttribute(const string &html, const string &attr);
	
	//Возвращает уровень постройки для attr
	static int getBuildingAttribute(const string &html, const string &attr);

	//Возвращает лигу в виде пары
	static pair<int,int> getLeague(const string &text);

	//Читает html-код страницы с менеджером и возвращает данные менеджера
	static void parseManager(const string &html, Manager &manager);
	
	//Возвращает значение поля которое относится к атрибуту attr для менеджера
	static string getManagerAttribute(const string &html, const string &attr);

	//Возвращает массив из 5 дат последних авторизаций менеджера
	static vector<DateTime> getManagerLogins(const string &html);

	//записывает в массив ids номера игроков которые на трансфере в html и возвращает их количество
	static int parseTransfers(const string &html, vector<int>&idsm, TransferFilter &tf);

	static DateTime parseDateTime(const string &html);

	static vector<pair<int,int> > parseTrainings(const string &html);
};

class TransferFilter
{
public:
	static const int FREE = 1;
	static const int MARKET = 2;
	static const int RESTRICTED = 4;
	int type; 

	int hoursToDedline;
	DateTime startSearchDateTime;

	pair<int, int>price;
	pair<int, int>age;
	pair<int, int>quality;
	pair<int, int>potential;
	pair<int, int>expirience;
	pair<int, int>goalie;
	pair<int, int>defence;
	pair<int, int>offence;
	pair<int, int>shooting;
	pair<int, int>speed;
	pair<int, int>strength;
	pair<int, int>self_control;
	pair<int, int>passing;
	pair<int, int>su;

	TransferFilter();
};

class HA
{
private:
	
	//класс для работы с WinInet
	HttpClient http;

	//загрузить страницу
	
	
	//логин и пароль для повторной авторизации
	string m_login;
	string m_password;

	//Возвращает url страницы трансферов с фильтром ft и смещением(offset в url)
	string getTransferUrl(TransferFilter &ft, int offset);

	bool usePause;


public:
bool getPage(const string &adress, string &buffer);
	//Результат авторизации
	enum AuthorizedState
	{
		AuthorizedOk = 1,       //Авторизация прошла успешно
		UserNoFound = 2,        //Пользователь не найден
		IncorrectPassword = 3,  //Неправильный пароль
		NoLogin = 4,            //Не введен логин
		NoPassword = 5,			//Не введен пароль
		AuthorizedError = 6     //Ошибка авторизации
	};

	//конструктор по умолчанию
	HA(const string &login, const string &password);

	//деструктор
	~HA();

	//Авторизация
	AuthorizedState authorize(const string &login, const string &password);

	//Загрузка игрока, возврашает false если игрока не существует
	bool getPlayer(int id, Player &player);

	//Загрузка команды, возвращает false если команды не существует
	bool getTeam(int id, Team &team);

	//Загрузка менеджера, возвращает false если менеджера не существует
	bool getManager(int id, Manager &manager);

	//возврашает массив id игроков которые находятся на трансфере и подходят к фильтеру ft
	vector<int>getTransferPlayers(TransferFilter &tf);
	
	vector<TrainingPlayer>getTrainingStats(StatPage sp);

	//возвращает текущее HA время
	DateTime getCurrentDateTime();

	void pauseOff();

};

#endif