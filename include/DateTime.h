#ifndef DATE_H
#define DATE_H

#include <string>
#include <windows.h>

using namespace std;


class DateTime
{

private:
	int month;
	int year;
	int day;
	int hour;
	int minute;

	int daysCount();
	int hoursCount();
	int minutesCount();

	static bool isVisokos(int year);
	
	void correctDateTime();

public:
	DateTime(int d = 0, int m = 0, int year = 0, int hour = 0, int minute = 0);
	DateTime(const string &);

	int getMonth() const;
	int getDay() const;
	int getYear() const;
	int getHour() const;
	int getMinute() const;

	void setMonth(int);
	void setDay(int);
	void setYear(int);
	void setHour(int);
	void setMinute(int);

	string toDateString() const;

	void addHours(int);

	static DateTime getTodayDateTime();
	static int daysInMonth(int);

	int daysBetween(DateTime &b);
	int hoursBetween(DateTime &b);
	int minutesBetween(DateTime &b);

	bool operator>(const DateTime &b);
};



#endif