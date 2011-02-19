#include "../include/HA.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>

using namespace std;

const string login = "Ozerich";
const int ID = 240876;
const string password = "o7o7o7o7";

HA ha("", "");
int mode;
ofstream fout("output.txt");
DateTime today;

void Login(bool autologin = false)
{
	if(autologin)
	{
		ha.authorize(login, password);
		return;
	}
	string login, password;
	cout << "Login : ";
	cin >> login;
	cout << "Password : ";
	cin >> password;
	cout << "Authorize.....";
	
	HA::AuthorizedState authorizedState;
	authorizedState = ha.authorize(login, password);
	switch (authorizedState)
	{
	case HA::AuthorizedError:
		cout << "Error";
		break;
	case HA::AuthorizedOk:
		cout << "OK";
		break;
	case HA::IncorrectPassword:
		cout << "Incorrect password";
		break;
	case HA::NoLogin:
		cout << "No login";
		break;
	case HA::NoPassword:
		cout << "No password";
		break;
	case HA::UserNoFound:
		cout << "User no found";
		break;
	}
	cout << endl;
}

bool checkDateTime(DateTime DateTime)
{
	if(DateTime.daysBetween(today) <= 4)
		return false;
	if(mode == 2)
		cout << DateTime.toDateString() << " : ";
	else 
		fout << DateTime.toDateString() << " : ";
	return true;
}

bool checkManager(int id)
{
	Manager manager;
	ha.getManager(id, manager);
	if(manager.logins.empty())
		manager.logins.push_back(DateTime());
	return checkDateTime(manager.logins[0]);
}

bool checkTeam(int id)
{
	Team team;
	team.manager = 0;
	ha.getTeam(id, team);
	return checkManager(team.manager);
}

bool checkPlayer(int id)
{
	Player player;
	ha.getPlayer(id, player);
	return checkTeam(player.team);;
}

void readParametr(const string &name, pair<int, int> &value)
{
	cout << name << " : ";
	string s;
	int i,j,a,b;
	char ch, buf[10];
	getline(cin, s);
	value.first = -1000;
	value.second = 1000;
	i = 0;
	while(i < s.size() && s[i] != ',')
	{
		buf[i] = s[i];
		i++;
	}
	buf[i] = 0;
	if(i > 0)
		value.first = atoi(buf);
	i++, j = 0;
	while(i < s.size())
		buf[j++] = s[i++];
	buf[j] = 0;
	if(j > 0)
		value.second = atoi(buf);
	cout << endl;
}

void readFilter(TransferFilter &tf)
{
	readParametr("Price", tf.price);
	readParametr("Quality", tf.quality);
	readParametr("Potential", tf.potential);
	readParametr("Goalie", tf.goalie);
	readParametr("Defence", tf.defence);
	readParametr("Offence", tf.offence);
	readParametr("Shooting", tf.shooting);
	readParametr("Passing", tf.passing);
	readParametr("Speed", tf.speed);
	readParametr("Strength", tf.strength);
	readParametr("Self-Control", tf.self_control);
	readParametr("Expirience", tf.expirience);
	readParametr("SU", tf.su);
	
	cout << "Hours to dedline : ";
	cin >> tf.hoursToDedline;
}

int main(int argc, char **argv)
{
	today = DateTime::getTodayDateTime();
	Login(true);
	TransferFilter tf;
	tf.type |= TransferFilter::RESTRICTED;
	tf.startSearchDateTime = ha.getCurrentDateTime();
	tf.defence.first = 120;
	tf.hoursToDedline = 12;
	readFilter(tf);
	cout << "Output mode (1 - file, 2 - console)";
	cin >> mode;
	mode = 1;
	vector<int> ms = ha.getTransferPlayers(tf);
	cout << ms.size() << " players are founded" << endl;
	for(int i = 0; i < ms.size(); i++)
	{
		cout << endl << i + 1 << ". Checking " << ms[i] << ": ";
		if(checkPlayer(ms[i]))
		{
			cout << "GOOD";
			if(mode == 1)
				fout << i + 1 << " " << ms[i] << endl;
			else 
				cout << i + 1 << " " << ms[i] << endl;
		}  
		else 
			cout << "BAD";

		Sleep(2000 + (rand() % 3000));
	}
	cout << "\nCompleted!";
	cin.get();cin.get();
	return 0;
}