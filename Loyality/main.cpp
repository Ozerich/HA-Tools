#include "../include/HA.h"
#include "../include/DateTime.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>

using namespace std;


using namespace std;

const string login = "Ozerich";
const string password = "o7o7o7o7";

HA ha("", "");
ofstream fout;
ifstream fin;
DateTime today;

void writeNewData()
{
	fout.open("base.txt");
	TransferFilter tf;
	tf.type |= TransferFilter::RESTRICTED;
	tf.startSearchDateTime = ha.getCurrentDateTime();
	tf.hoursToDedline = 24;
	tf.potential.first = 10;
	tf.su.first = 200;
	tf.expirience.first = 100;
	vector<int> ms = ha.getTransferPlayers(tf);
	for(int i = 0; i < ms.size(); i++)
		fout << ms[i] << endl;
	fout.close();
}

void checkData()
{
	fin.open("base.txt");
	fout.open("result.txt", ios_base::app);

	int id;
	Player player;
	while(fin >> id)
	{
		cout << "Checking " << id << "....";
		ha.getPlayer(id, player);
		if(player.playerTransferState == Player::NoSale)
		{
			fout << id << endl;
			cout << "Good";
		} 
		else
			cout << "Bad";
		cout << endl;
	}
	fin.close();
	fout.close();
}

int main()
{
	ha.authorize(login, password);
	checkData();
	writeNewData();
	return 0;
} 