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

	enum PlayerTransferState      //����������� ��������� ������
	{
		NoSale = 1,       //�� ��������
		Free = 2,          //������
		Saled = 3,         //��������
		Unrestricted = 4  //������������
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

	//������� ������ ������
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
	
	//������ html-��� �������� � ������� � ���������� ������ ������
	static void parsePlayer(const string &html, Player &player);
	
	//���������� �������� ���� ������� ��������� � �������� attr ��� ������
	static string getPlayerAttribure(const string &html, const string &attr);

	//��������� �� ������ str �����, ������� ���������� � ������� beg � ������������� ����� endChar
	static int getNumber(const string &str, int beg = 0, const char endChar = ' ');

	//��������� ������� �������(�������� + ��������) �� ��� �����
	static void parseDoubleAttribute(const string &str, string &left, string &right);

	//���������� �������� �������� ����������
	static int getSatisfaction(const string &str);

	//���������� ���������� ���� � ������
	static int getStarsCount(const string &str);

	//���������� ������� ���� ��� ������
	static pair<int,int> getPlayerSkill(const string &str);

	//������ html-��� �������� � �������� � ���������� ������ �������
	static void parseTeam(const string &html, Team &team);

	//������ html-��� �������� � ����������� � ���������� ������ ��������
	static void parseTeamBuildings(const string &html, Team &team);

	//���������� �������� ���� ������� ��������� � �������� attr ��� �������
	static string getTeamAttribute(const string &html, const string &attr);
	
	//���������� ������� ��������� ��� attr
	static int getBuildingAttribute(const string &html, const string &attr);

	//���������� ���� � ���� ����
	static pair<int,int> getLeague(const string &text);

	//������ html-��� �������� � ���������� � ���������� ������ ���������
	static void parseManager(const string &html, Manager &manager);
	
	//���������� �������� ���� ������� ��������� � �������� attr ��� ���������
	static string getManagerAttribute(const string &html, const string &attr);

	//���������� ������ �� 5 ��� ��������� ����������� ���������
	static vector<DateTime> getManagerLogins(const string &html);

	//���������� � ������ ids ������ ������� ������� �� ��������� � html � ���������� �� ����������
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
	
	//����� ��� ������ � WinInet
	HttpClient http;

	//��������� ��������
	
	
	//����� � ������ ��� ��������� �����������
	string m_login;
	string m_password;

	//���������� url �������� ���������� � �������� ft � ���������(offset � url)
	string getTransferUrl(TransferFilter &ft, int offset);

	bool usePause;


public:
bool getPage(const string &adress, string &buffer);
	//��������� �����������
	enum AuthorizedState
	{
		AuthorizedOk = 1,       //����������� ������ �������
		UserNoFound = 2,        //������������ �� ������
		IncorrectPassword = 3,  //������������ ������
		NoLogin = 4,            //�� ������ �����
		NoPassword = 5,			//�� ������ ������
		AuthorizedError = 6     //������ �����������
	};

	//����������� �� ���������
	HA(const string &login, const string &password);

	//����������
	~HA();

	//�����������
	AuthorizedState authorize(const string &login, const string &password);

	//�������� ������, ���������� false ���� ������ �� ����������
	bool getPlayer(int id, Player &player);

	//�������� �������, ���������� false ���� ������� �� ����������
	bool getTeam(int id, Team &team);

	//�������� ���������, ���������� false ���� ��������� �� ����������
	bool getManager(int id, Manager &manager);

	//���������� ������ id ������� ������� ��������� �� ��������� � �������� � �������� ft
	vector<int>getTransferPlayers(TransferFilter &tf);
	
	vector<TrainingPlayer>getTrainingStats(StatPage sp);

	//���������� ������� HA �����
	DateTime getCurrentDateTime();

	void pauseOff();

};

#endif