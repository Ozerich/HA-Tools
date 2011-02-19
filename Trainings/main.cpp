#include "../include/HA.h"
#include "../include/DateTime.h"
#include "../include/phpbb.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>
#include <cctype>

using namespace std;

const string login = "OlegIO";
const string password = "o7o7o7o7";

HA ha("", "");
ofstream fout;
ifstream fin;




string toBBString(TrainingPlayer &tp)
{
	string result;

	char age[10];
	itoa(tp.player.age, age, 10);

	char id[10];
	itoa(tp.player.id, id, 10);

	char quality[10];
	itoa(tp.player.quality.second, quality, 10);

	char potential[10];
	itoa(tp.player.potential.second, potential, 10);

	char tr_value[10];
	itoa(tp.trainingValue, tr_value, 10);

	Team team;
	ha.getTeam(tp.player.team, team);

	char tr_area[10];
	itoa(team.trainingArea, tr_area, 10);

	char reg_area[10];
	itoa(team.regenerationArea, reg_area, 10);

	char league[10];
	itoa(team.league.first, league, 10);

	char division[10];
	itoa(team.league.second, division, 10);

	result = "[url=http://www.hockeyarena.net/ru/index.php?p=public_player_info.inc&id=";
	result += id;
	result += "]";

	result += tp.player.name;
	result += "[/url]";
	result = result + " " + age + " лет, ";

	result += quality;
	result += "/";
	result += potential;
	result += ", ";


	result += team.name;
	result += "(";
	result += league;
	result += ".";
	result += division;
	result += "): ";

	result += tr_area;
	result += "/";
	result += reg_area;


	result += " = ";
	result += tr_value;
	result += "%";

	return result;
}


int main()
{
	ha.authorize(login, password);
	ha.pauseOff();

	string text = "", str;

	text = "[b]Мир[/b]\r\n";
	vector<TrainingPlayer> result = ha.getTrainingStats(WorldPage);
	for(int i = 0; i < result.size(); i++)
	{
		str = toBBString(result[i]);
		text = text + str + "\n";
	}

	text += "\r\n[b]Беларусь[/b]\r\n";
	result = ha.getTrainingStats(CountryPage);
	for(int i = 0; i < result.size(); i++)
	{
		str = toBBString(result[i]);
		text = text + str + "\n";
	}

	text += "\n\n[b]Created by Ozerich's software (using OLEGio`s sponsor pack)[/b]";

	const char *p1 = text.c_str();
	char p2[40000];
	


	ofstream fout("result.txt");
	fout << p1;
	
	//string login,password;
	//ifstream fin("login.txt");
	//fin>>login>>password;



	//forum.authorize(login, password);
	//forum.write_message(1247,s);

	return 0;
}