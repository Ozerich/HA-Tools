#include "DateTime.h"

DateTime::DateTime(int day, int month, int year, int hour, int minute)
{
	setDay(day);
	setMonth(month);
	setYear(year);
	setHour(hour);
	setMinute(minute);

}

DateTime::DateTime(const string &str)
{
	string num;
	if(str.size() == string("dd.mm.yyyy").size())
	{
		num.assign(str.begin(), str.begin() + 2);
		setDay(atoi(num.c_str()));
		num.assign(str.begin() + 3, str.begin() + 5);
		setMonth(atoi(num.c_str()));
		num.assign(str.begin() + 6, str.begin() + 10);
		setYear(atoi(num.c_str()));
		setHour(0);
		setMinute(0);
	}
	else if(str.size() == string("hh:mm, dd.mm").size())
	{
		num.assign(str.begin(), str.begin() + 2);
		setHour(atoi(num.c_str()));
		num.assign(str.begin() + 3, str.begin() + 5);
		setMinute(atoi(num.c_str()));
		num.assign(str.begin() + 7, str.begin() + 9);
		setDay(atoi(num.c_str()));
		num.assign(str.begin() + 10, str.begin() + 12);
		setMonth(atoi(num.c_str()));
		setYear(getTodayDateTime().getYear());
	}
}

void DateTime::setDay(int _day)
{
	day = _day;
}

void DateTime::setMonth(int _month)
{
	month = _month;
}

void DateTime::setYear(int _year)
{
	year = _year;
}

void DateTime::setHour(int _hour)
{
	hour = _hour;
}

void DateTime::setMinute(int _minute)
{
	minute = _minute;
}

int DateTime::getDay()const
{
	return day;
}

int DateTime::getMonth() const
{
	return month;
}

int DateTime::getYear() const
{
	return year;
}

int DateTime::getHour()const
{
	return hour;
}

int DateTime::getMinute()const
{
	return minute;
}

string DateTime::toDateString() const
{
	string res;
	char buf[10];
	
	res.append(itoa(getDay(), buf, 10));
	res.push_back('.');
	res.append(itoa(getMonth(), buf, 10));
	res.push_back('.');
	res.append(itoa(getYear(), buf, 10));
	
	return res;
}

DateTime DateTime::getTodayDateTime()
{
	LPSYSTEMTIME time = new  _SYSTEMTIME();
	GetSystemTime(time);
	return DateTime(time->wDay, time->wMonth, time->wYear, time->wHour, time->wMinute);
}

int DateTime::daysBetween(DateTime &b)
{
	int daysA = daysCount();
	int daysB = b.daysCount();;
	return abs(daysB - daysA);
}

int DateTime::minutesBetween(DateTime &b)
{
	int minutesA = minutesCount();
	int minutesB = b.minutesCount();
	return abs(minutesA - minutesB);
}

int DateTime::hoursBetween(DateTime &b)
{
	int hoursA = hoursCount();
	int hoursB = b.hoursCount();
	return abs(hoursA - hoursB);
}

int DateTime::minutesCount()
{
	return hoursCount() * 60 + getMinute();
}

int DateTime::hoursCount()
{
	return daysCount() * 24 + getHour();
}

int DateTime::daysCount()
{
	int days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int res = 0;
	for(int i = 0; i < getYear() - 1; i++)
		res += isVisokos(i) ? 366 : 365;
	if(isVisokos(getYear()))
		days_in_month[1] = 29;
	for(int i = 0; i < getMonth() - 1; i++)
		res += days_in_month[i];
	return res + getDay() - 1;
}

bool DateTime::isVisokos(int y)
{
	if(y % 4 == 0 && y % 100 != 0)
		return true;
	else
		return false;
}

void DateTime::addHours(int count)
{
	setHour(getHour() + count);
	correctDateTime();
}

void DateTime::correctDateTime()
{
	setDay(getDay() + getHour() / 24);
	setHour(getHour() % 24);
	if(getDay() > daysInMonth(getMonth()))
	{
		setDay(1);
		setMonth(getMonth() + 1);
		if(getMonth() > 12)
		{
			setMonth(1);
			setYear(getYear() + 1);
		}
	}
}

int DateTime::daysInMonth(int month)
{
	int days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	return days_in_month[month - 1];
}

bool DateTime::operator>(const DateTime &b)
{
	if(getYear() > b.getYear())
		return true;
	else if(getYear() == b.getYear())
	{
		if(getMonth() > b.getMonth())
			return true;
		else if(getMonth() == b.getMonth())
		{
			if(getDay() > b.getDay())
				return true;
			else if(getDay() == b.getDay())
			{
				if(getHour() > b.getHour())
					return true;
				else if(getHour() == b.getHour())
				{
					if(getMinute() > b.getMinute())
						return true;
				}
			}
		}
	}
	return false;
}