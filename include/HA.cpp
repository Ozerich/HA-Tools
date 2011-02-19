#include "HA.h"

HA::HA(const string &login, const string &password) : m_login(login), m_password(password), usePause(true)
{
	http.openInternet();
	http.openConnection("www.hockeyarena.net");
	srand(time(NULL));
}

HA::~HA()
{
	http.closeInternet();
	http.closeConnection();
}



HA::AuthorizedState HA::authorize(const string &login, const string &password)
{
	if(login.empty())
		return NoLogin;
	else if(password.empty())
		return NoPassword;
	else
	{
		bool debug = 1;
		string action = "nick=" + login + "&password=" + password;
		if(http.post("en/index.php?p=security_log.php", action.c_str()))
		{
			string str =  http.getData();
			if(str.find("does no exists") != -1)
				return UserNoFound;
			else if(str.find("Wrong password") != -1)
				return IncorrectPassword;
			else
			{
				m_login = login;
				m_password = password;
				return AuthorizedOk;	
			}
		}
		return AuthorizedError;
	}
}

void HA::pauseOff()
{
	usePause = false;
}

bool HA::getPage(const string &adress, string &buffer)
{
	if(usePause)
		Sleep(rand() % 3000 + 2000);
	http.get(adress.c_str());
	buffer = http.getData();
	if(buffer.find("You don't have access to this page") != -1)
	{
		authorize(m_login, m_password);
		http.get(adress.c_str());
		buffer = http.getData();
	}


	return true;
}

DateTime HA::getCurrentDateTime()
{
	DateTime res;
	string html;
	getPage("en/index.php?smenu=mpublic", html);
	res = Parser::parseDateTime(html);
	return res;
}

bool HA::getManager(int id, Manager &manager)
{
	string html;
	manager.id = id;
	if(id == 0)
		return false;
	char sId[10];
	itoa(id, sId, 10);
	char txt[100] = "en/index.php?p=manager_manager_info.php&id=";
	_tcscat(txt, sId);
	getPage(txt, html);

	Parser::parseManager(html, manager);
	return !manager.name.empty();
}

bool HA::getPlayer(int id, Player &player)
{
	string html;
	char sId[10];
	itoa(id, sId, 10);
	char txt[100] = "en/index.php?p=public_player_info.inc&id=";
	_tcscat(txt, sId);
	getPage(txt, html);
	if(html.find("Player does not exist or has retired") != -1)
		return false;
	Parser::parsePlayer(html, player);
	return true;
}

bool HA::getTeam(int id, Team &team)
{
	string html;
	char sId[10];
	itoa(id, sId, 10);
	char txt[100] = "en/index.php?p=public_team_info_basic.php&team_id=";
	char txt2[100] = "en/index.php?p=public_team_info_stadium.php&team_id=";
	_tcscat(txt, sId);
	_tcscat(txt2, sId);
	getPage(txt, html);
	if(html.find("team does not exist!") != -1)
		return false;
	team.id = id;
	Parser::parseTeam(html, team);
	getPage(txt2, html);
	Parser::parseTeamBuildings(html, team);
	return true;
}

string intToStr(int a)
{
	char buf[10];
	itoa(a, buf, 10);
	return buf;
}

//добавляет к url &<name>_min= и &<name>_max= если значение больше нуля
void addToTransferUrl(string &url, pair<int, int> value, string name)
{
	if(value.first > -1000)
	{
		url.append("&");
		url.append(name);
		url.append("_min");
		url.append("=");
		url.append(intToStr(value.first));

	}
	if(value.second < 1000)
	{
		url.append("&");
		url.append(name);
			url.append("_max");
		url.append("=");
		url.append(intToStr(value.second));
	}
}

string HA::getTransferUrl(TransferFilter &tf, int offset)
{
	string str = "en/index.php?p=manager_player_market_sql.php&type=";
	
	switch(tf.type)
	{
	case 1:
		str.push_back('0');
		break;
	case 2:
		str.push_back('1');
		break;
	case 4:
		str.push_back('2');
		break;
	default:
		str.push_back('3');
		break;
	}
	
	str.append("&deadline=4");

	str.append("&offset=");
	str.append(string(intToStr(offset)));

	addToTransferUrl(str, tf.age, "age");
	addToTransferUrl(str, tf.goalie, "skill_goalie");
	addToTransferUrl(str, tf.defence, "skill_defense");
	addToTransferUrl(str, tf.offence, "skill_attack");
	addToTransferUrl(str, tf.shooting, "skill_shot");
	addToTransferUrl(str, tf.speed, "speed");
	addToTransferUrl(str, tf.strength, "strength");
	addToTransferUrl(str, tf.passing, "passing");
	addToTransferUrl(str, tf.self_control, "selfcontrol");
	addToTransferUrl(str, tf.price, "price");
	addToTransferUrl(str, tf.expirience, "experience");
	addToTransferUrl(str, tf.quality, "quality");
	addToTransferUrl(str, tf.potential, "potential");

	return str;
}


vector<TrainingPlayer> HA::getTrainingStats(StatPage sp)
{
	vector<pair<int, int> > data;
	vector<TrainingPlayer> result;
	TrainingPlayer player;
	string url, text;

	switch(sp)
	{
	case LeaguePage:
		url = "en/index.php?p=manager_top_salary.php";
		break;
	case CountryPage:
		url = "en/index.php?p=manager_top_state_salary.php";
		break;
	case WorldPage:
		url = "en/index.php?p=manager_top_world_salary.php";
		break;
	}

	getPage(url, text);

	data = Parser::parseTrainings(text);
	for(int i = 0; i < data.size(); i++)
	{
		getPlayer(data[i].first, player.player);
		player.trainingValue = data[i].second;
		result.push_back(player);
	}

	return result;
}

vector<int> HA::getTransferPlayers(TransferFilter &tf)
{
	vector<int>	res,ms;

	string html;
	int offset = 0, count = 25;
	while(count == 25)
	{
		string url = getTransferUrl(tf, offset);
		getPage(url, html);
		ms.clear();
		count = Parser::parseTransfers(html, ms, tf);
		for(int i = 0; i < ms.size(); i++)
			res.push_back(ms[i]);
		offset += 25;
	}
	return res;
}



ostream& operator<< (ostream &out, const Player &player)
{
	out << player.name << "(" << player.id << ")" << endl;
	out << "Team id : " << player.team << endl;
	out << player.age << " years from " << player.country << endl;
	if(player.injuryLength == 0)
		out << "Healthy";
	else
		out << "Injury for " << player.injuryLength << " days";
	out << endl << "Salary : " << player.salary << endl;
	out << "Ability Index : " << player.abilityIndex << endl;
	out << "Satisfaction : " << player.satisfaction << endl;
	out << "Loyalty : " << player.loyalty << endl;
	out << "RIP : " << player.quality.second << (player.quality.first ? "" : "(?)")
		<< "/" << player.potential.second << (player.potential.first ? "" : "(?)") << endl;
	out << "Stars : " << player.stars << endl;
	out << "Goalie : " << player.goalie.first << "(" << player.goalie.second << "%)" << endl;
	out << "Defense : " << player.defense.first << "(" << player.defense.second << "%)" << endl;
	out << "Offense : " << player.offense.first << "(" << player.offense.second << "%)" << endl;
	out << "Shooting : " << player.shooting.first << "(" << player.shooting.second << "%)" << endl;
	out << "Passing : " << player.passing.first << "(" << player.passing.second << "%)" << endl;
	out << "Speed : " << player.speed.first << "(" << player.speed.second << "%)" << endl;
	out << "Strength : " << player.strength.first << "(" << player.strength.second << "%)" << endl;
	out << "Self Control : " << player.self_control.first << "(" << player.self_control.second << "%)" << endl;
	out << "Form : " << player.form.first << "(" << player.form.second << ")" << endl;
	out << "Expirience : " << player.expirience.first << "(" << player.expirience.second << "%)" << endl;
	out << "Training : " << player.lastTraining << "%" << endl << endl;
	switch(player.playerTransferState)
	{
	case Player::Free:
		cout << "Free player";
		break;
	case Player::Saled:
		cout << "Player for sale";
		break;
	case Player::Unrestricted:
		cout << "Unrestricted free agent";
		break;
	case Player::NoSale:
		cout << "No sale";
		break;
	}
	return out;
}


Player::Player()
{

}

void Player::reset()
{
	name = country = "";
	id = age = contract = team = injuryLength = salary = abilityIndex = loyalty = satisfaction = weeksInTeam 
		= energy = lastTraining = 0;
	quality = make_pair(0, 0);
	potential = make_pair(0, 0);
	speed = make_pair(0, 0);
	defense = make_pair(0, 0);
	strength = make_pair(0, 0);
	offense = make_pair(0, 0);
	self_control = make_pair(0, 0);
	shooting = make_pair(0, 0);
	form = make_pair(0, 0);
	passing = make_pair(0, 0);
	expirience = make_pair(0, 0);
	goalie = make_pair(0, 0);
}

Manager::Manager()
{
}

void Manager::reset()
{
	name = country = "";
	points = id = team = 0;
	registrationDateTime = DateTime();
}

Team::Team()
{

}

void Team::reset()
{

	manager = playersCount = stadiumCapacity = trainingArea = regenerationArea = fansArea = scoutingArea 
		= marketingArea = 0;
	stadiumName = name = "";
	found = DateTime(0,0,0);
}

ostream& operator<<(ostream &out, const Team &team)
{
	out << team.name << " (" << team.id << "), manager id = " << team.manager << endl;
	out << "DateTime of found : " << team.found.toDateString() << endl;
	out << "Players count : " << team.playersCount << endl << endl;
	
	out << "Stadium : " << team.stadiumName << " (" << team.stadiumCapacity << ")" << endl;
	out << "Training area : " << team.trainingArea << endl;
	out << "Regeneration area : " << team.regenerationArea << endl;
	out << "Fan Club : " << team.fansArea << endl;
	out << "Scouting : " << team.scoutingArea << endl;
	out << "Marketing : " << team.marketingArea << endl;
	
	return out;
}

ostream& operator<<(ostream &out, const Manager &manager)
{
	out << "id " << manager.id << endl;
	out << manager.name;
	if(manager.online)
		cout << " (Online)";
	cout << endl;
	out << "Team id : " << manager.team << endl;
	out << "Country : " << manager.country << endl;
	out << "Points : " << manager.points << endl;
	out << "Registration : " << manager.registrationDateTime.toDateString() << endl << endl;
	out << "Last logins :" << endl;
	for(int i = 0; i < manager.logins.size(); i++)
		out << manager.logins[i].toDateString() << endl;
	return out;
}



TransferFilter::TransferFilter()
{
	type = 0;
	price = age = quality = potential = expirience = goalie = defence = offence = shooting = passing 
		= self_control = speed = strength = su = make_pair(-10000, 10000);
}

int Parser::getSatisfaction(const string &str)
{
	for(int i = 0; i < SATISFACTIONS_COUNT; i++)
		if(SATISFACTIONS[i].compare(str) == 0)
			return i - SATISFACTIONS_COUNT / 2;
	return -5;
}


string Parser::getPlayerAttribure(const string &html, const string &attr)
{
	string res;
	string s_attr = attr;
	s_attr.append("</td>");
	int p = html.find(s_attr);
	if(p == -1)
		return "";
	p += string(s_attr).size();
	int i = p + 31;
	while(!(html[i] == '<' && html[i + 1] == '/' && html[i + 2] == 't' && html[i + 3] == 'd'))
		res.push_back(html[i++]);
	return res;
}

int Parser::getNumber(const string &str, int beg,  const char endChar)
{
	int i = beg, j = 0;
	char num[100];
	while(i < str.size() && str[i] != endChar)
		num[j++] = str[i++];
	num[j] = 0;
	return atoi(num);
}

void Parser::parseDoubleAttribute(const string &str, string &left, string &right)
{
	int i = 0;
	left.assign(str.begin(), str.begin() + str.find('>') + 1);
	right.assign(str.begin() + str.find('>') + 1, str.end());
}

int Parser::getStarsCount(const string &str)
{
	int res = 0;
	string s = str;
	while(s.find("star") != -1)
	{
		s.erase(s.begin() + s.find("star"));
		res++;
	}
	return res;
}

pair<int,int> Parser::getPlayerSkill(const string &str)
{
	return make_pair(getNumber(str), getNumber(str, str.find('(') + 1,'%'));
}

void Parser::parsePlayer(const string &html, Player &player)
{
	player.reset();

	string s,left,right;
	char num[10],ch;
	int p = html.find("Information about player ") + string("Information about player ").size();
	int i = p, j;
	while(html[i] != ',')
		player.name.push_back(html[i++]);
	
	i+=5, j = 0;

	while(html[i] != ' ')
		num[j++] = html[i++];
	num[j] = 0;
	player.id = atoi(num);

	s = Parser::getPlayerAttribure(html, "Age");
	player.age = atoi(s.c_str());

	s = Parser::getPlayerAttribure(html, "Country");
	Parser::parseDoubleAttribute(s, left, right);
	player.country.assign(right.begin() + 6, right.end());

	s = Parser::getPlayerAttribure(html, "Contract");
	player.contract = getNumber(s);

	s = Parser::getPlayerAttribure(html, "Team");
	Parser::parseDoubleAttribute(s, left, right);
	player.team = getNumber(s, s.find("id=") + 3,'<');

	s = Parser::getPlayerAttribure(html, "Injury");
	player.injuryLength = (s[0] == 'H') ? 0 : getNumber(s, s.find("(") + 1);


	s = Parser::getPlayerAttribure(html, "Quality");
	Parser::parseDoubleAttribute(s, left, right);
	right.erase(right.begin(), right.begin() + 6);
	player.quality = make_pair(s[s.find("pics/") + 4] == 't', getNumber(right));

	s = Parser::getPlayerAttribure(html, "Salary");
	while(s.find(' ') != -1)
		s.erase(s.begin() + s.find(' '));
	player.salary = getNumber(s);

	s = Parser::getPlayerAttribure(html, "Potential");
	Parser::parseDoubleAttribute(s, left, right);
	right.erase(right.begin(), right.begin() + 6);
	player.potential = make_pair(s[s.find("pics/") + 4] == 't', getNumber(right));
	
	s = Parser::getPlayerAttribure(html, "Ability index");
	player.abilityIndex = getNumber(s);

	s = Parser::getPlayerAttribure(html, "Loyalty");
	player.loyalty = getNumber(s);

	s = Parser::getPlayerAttribure(html, "Satisfaction");
	player.satisfaction = Parser::getSatisfaction(s);

	s = Parser::getPlayerAttribure(html, "Weeks in team");
	player.weeksInTeam = getNumber(s);

	s = Parser::getPlayerAttribure(html, "Star");
	player.stars = Parser::getStarsCount(s);

	s = Parser::getPlayerAttribure(html, "Goalie");
	player.goalie = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Speed");
	player.speed = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Defense");
	player.defense = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Strength");
	player.strength = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Offense");
	player.offense = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Self-control");
	player.self_control = Parser::getPlayerSkill(s);
	
	s = Parser::getPlayerAttribure(html, "Shooting");
	player.shooting = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Form");
	player.form = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Passing");
	player.passing = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Experience");
	player.expirience = Parser::getPlayerSkill(s);

	s = Parser::getPlayerAttribure(html, "Energy");
	player.energy = getNumber(s);

	s = Parser::getPlayerAttribure(html, "Training");
	player.lastTraining = getNumber(s, 0, '%');

	if(html.find("Unrestricted free agent") != -1)
		player.playerTransferState = Player::Unrestricted;
	else if(html.find("Player for sale") != -1)
		player.playerTransferState = Player::Saled;
	else if(html.find("Free player") != -1)
		player.playerTransferState = Player::Free;
	else
		player.playerTransferState = Player::NoSale;

	if(p = player.name.find("\"C\"") != -1)
		player.name.assign(player.name.begin(), player.name.begin() + player.name.find('"'));
	if(p = player.name.find("\"A\"") != -1)
		player.name.assign(player.name.begin(), player.name.begin() + player.name.find('"'));


}

string Parser::getTeamAttribute(const string &html, const string &attr)
{
	string m_attr = attr, res;
	m_attr.append("</b></td>");
	int p = html.find(m_attr) + m_attr.size() + 30;
	while(html[p] != '>')
		p++;
	int i = p + 1;
	while(!(html[i] == '<' && html[i + 1] == '/' && html[i + 2] == 't' && html[i + 3] == 'd'))
		res.push_back(html[i++]);
	return res;
}

int Parser::getBuildingAttribute(const string &html, const string &attr)
{
	string m_attr = attr, res;
	m_attr.append("</b>");
	int p = html.find(m_attr) + m_attr.size();
	int i = p;
	while(!(html[i] == 'r' && html[i + 1] == 'i' && html[i + 2] == 'g' && html[i + 3] == 'h'))
		i++;
	while(html[i] != '>')
		i++;
	i++;
	return getNumber(html, i);
}

string Parser::getManagerAttribute(const string &html, const string &attr)
{
	string m_attr = attr, res;
	m_attr.append("</b>");
	int p = html.find(m_attr) + m_attr.size();
	int i = p + 15;
	
	while(html[i] != '>')
		i++;
	i++;
	while(!(html[i] == '&' && html[i + 1] == 'n' && html[i + 2] == 'b' && html[i + 3] == 's'))
		res.push_back(html[i++]);
	return res;
}

pair<int,int> Parser::getLeague(const string &str)
{
	string lefts, rights;
	int left, right;
	
	lefts.assign(str.begin(), str.begin() + str.find("."));
	if(lefts == "I")left = 1;
	if(lefts == "II")left = 2;
	if(lefts == "III")left = 3;
	if(lefts == "IV")left = 4;
	if(lefts == "V")left = 5;
	if(lefts == "VI")left = 6;
	if(lefts == "VII")left = 7;
	if(lefts == "VIII")left = 8;
	if(lefts == "IX")left = 9;
	if(lefts == "X")left = 10;

	rights.assign(str.begin() + str.find(".") + 1, str.end());
	right = atoi(rights.c_str());
	
	return make_pair(left,right);
}

void Parser::parseTeam(const string &html, Team &team)
{
	team.reset();

	int p = html.find("<td colspan=\"3\" height=\"18\" class=\"ysptblhdr\" align=\"center\">&nbsp;") + 
		string("<td colspan=\"3\" height=\"18\" class=\"ysptblhdr\" align=\"center\">&nbsp;").size();
	while(html[p] != 10)
		team.name.push_back(html[p++]);
 
	string s = Parser::getTeamAttribute(html, "Manager");
	team.manager = getNumber(s, s.find("id=") + 3,'&');

	s = Parser::getTeamAttribute(html, "League");
	p = s.find("id=");
	while(s[p++] != '>');
	s.assign(s.begin() + p, s.begin() + s.find("</a>"));
	team.league = getLeague(s);

	s = Parser::getTeamAttribute(html, "Found");
	team.found = DateTime(s.assign(s.begin(), s.end() - 6));

	s = Parser::getTeamAttribute(html, "Number of players");
	s.erase(s.end() - 6, s.end());
	team.playersCount = atoi(s.c_str());

	s = Parser::getTeamAttribute(html, "Stadium name");
	s.erase(s.end() - 6, s.end());
	team.stadiumName = s;

	s = Parser::getTeamAttribute(html, "Stadium capacity");
	s.erase(s.end() - 6, s.end());
	team.stadiumCapacity = getNumber(s, 0, '&');
}

void Parser::parseTeamBuildings(const string &html, Team &team)
{
	team.trainingArea = getBuildingAttribute(html, "Training area");
	team.regenerationArea = getBuildingAttribute(html, "Regeneration area");
	team.fansArea = getBuildingAttribute(html, "Cheerleaders");
	team.scoutingArea = getBuildingAttribute(html, "Scouting department");
	team.marketingArea = getBuildingAttribute(html, "Marketing department");
}


vector<DateTime> Parser::getManagerLogins(const string &html)
{
	string txt,s;
	int p,j;
	txt.assign(html.begin() + html.find("&nbsp;&nbsp;&nbsp;<b>Date</b>"), html.end());
	vector<DateTime>res;
	for(int i = 0; i < 5; i++)
	{
		s = "<tr class=sr1>";
		if(i % 2 == 1)
			s[s.size() - 2] = '2';
		p = txt.find(s);
		if(p == -1)
			break;
		p += string(s).size();
		j = p + 92;
		while(txt[j] != ' ')
			j++;
		s.assign(txt.begin() + j + 1, txt.begin() + j + 11);
		res.push_back(DateTime(s));
		txt.erase(txt.begin() + p - 14, txt.begin() + p);
	}
	return res;
}

void Parser::parseManager(const string &html, Manager &manager)
{

	manager.reset();

	int p = html.find("align=\"center\">Manager ") + string("align=\"center\">Manager ").size();
	while(html[p] != 10 && html[p] != '<')
		manager.name.push_back(html[p++]);
	if(manager.name.empty())
		return;
	string s = getManagerAttribute(html, "Team");
	manager.team = getNumber(s, s.find("id=") + 3, '\"');

	manager.country = getManagerAttribute(html, "Country");

	s = getManagerAttribute(html, "Registered from");
	string DateTimes = s.assign(s.begin() + 7, s.end());
	manager.registrationDateTime = DateTime(DateTimes);

	manager.points = getNumber(getManagerAttribute(html, "Points"), 0, '&');

	s = getManagerAttribute(html, "Online");
	manager.online = (s[0] == 'y');

	manager.logins = getManagerLogins(html);
}

int Parser::parseTransfers(const string &html, vector<int> &ids, TransferFilter &tf)
{
	DateTime lastDeadLine = tf.startSearchDateTime;
	lastDeadLine.addHours(tf.hoursToDedline);
	string _html,s,ss;
	int su, i, readedCount = 0;
	DateTime dt;
	ids.clear();
	int p, pp;
	_html.assign(html.begin() + html.find("<tr class=sr1>"), html.end());
	_html.assign(_html.begin(), _html.begin() + _html.find("</tr>"));
	p = 0;
	while(p != -1)
	{
		s.assign(_html.begin() + p, _html.begin() + p + 200);
		_html.erase(_html.begin() + p, _html.begin() + p + 15);
		
		for(i = 0; i < 10; i++)
		{
			pp = _html.find("<td class=\"nir\">");
			_html.erase(_html.begin() + pp, _html.begin() + pp + string("<td class=\"nir\">").size());
		}
		su = getNumber(_html, pp + 6, '<');
		for(i = 0; i < 5; i++)
		{
			pp = _html.find("<td class=\"nir\">");
			if(pp != -1)
				_html.erase(_html.begin() + pp, _html.begin() + pp + string("<td class=\"nir\">").size());
		}

		p =(readedCount % 2 == 0) ? _html.find("<tr class=sr2>") : _html.find("<tr class=sr1>");
		if(p != - 1)
		{
			ss.assign(_html.begin() + p - 20, _html.begin() + p - 20 + 12);
			dt = DateTime(ss);			
		}
		else
		{
			ss.assign(_html.end() - 21, _html.end() - 9);
			dt = DateTime(ss);
		}
		if(dt > lastDeadLine)
			break;

		pp = s.find("id=") + 3;
		ss.clear();
		while(s[pp] != '\'')
			ss.push_back(s[pp++]);
		readedCount++;
		if(su >= tf.su.first && su <= tf.su.second)
			ids.push_back(getNumber(ss));
	}

	return readedCount;
}



DateTime Parser::parseDateTime(const string &html)
{
	int j, p = html.find("var serverClock = new Date(") + string("var serverClock = new Date(").size();
	string s;
	char num[10];

	DateTime dt;

	s.assign(html.begin() + p, html.begin() + p + 15);
	
	j = p = 0;
	while(s[p + j] != ',')
	{
		num[j] = s[p + j];
		j++;
	}
	num[j] = 0;
	dt.setYear(getNumber(num));
	
	p += j + 1;
	j = 0;
	while(s[p + j] != ',')
	{
		num[j] = s[p + j];
		j++;
	}
	num[j] = 0;
	dt.setMonth(getNumber(num) + 1);
	
	p += j + 1;
	j = 0;
	while(s[p + j] != ',')
	{
		num[j] = s[p + j];
		j++;
	}
	num[j] = 0;
	dt.setDay(getNumber(num));

	p += j + 1;
	j = 0;
	while(s[p + j] != ',')
	{
		num[j] = s[p + j];
		j++;
	}
	num[j] = 0;
	dt.setHour(getNumber(num));
		
	p += j + 1;
	j = 0;
	while(p + j < s.size())
	{
		num[j] = s[p + j];
		j++;
	}
	num[j] = 0;
	dt.setMinute(getNumber(num));

	return dt;
}


vector<pair<int, int> > Parser::parseTrainings(const string &html)
{
	vector<pair<int,int> > result;

	int p_id, p_value = 0;
	vector<string>trs,tds;
	string temp;

	int p = html.find("top 10 players based on training");
	string str;
	str.assign(html.begin() + p, html.end());

	p = str.find("<table class=\"stats\" width=\"100%\" id=\"table-\" border='0'>");
	str.assign(str.begin() + p, str.end());

	p = str.find("</table>");
	str.assign(str.begin(), str.begin() + p);

	while(str.size() > 0)
	{
		temp.assign(str.begin() + str.find("<tr"), str.begin() + str.find("</tr>"));
		trs.push_back(temp);
		str.assign(str.begin() + str.find("</tr>") + 6, str.end());
	}

	for(int i = 1; i < trs.size(); i++)
	{
		str = trs[i];
		tds.clear();

		while(str.size() > 15)
		{
			temp.assign(str.begin() + str.find("<td"), str.begin() + str.find("</td>"));
			tds.push_back(temp);
			str.assign(str.begin() + str.find("</td>") + 6, str.end());
		}

		tds[0].assign(tds[0].begin() + tds[0].find("id=") + 3, tds[0].end());
		temp.assign(tds[0].begin(), tds[0].begin() + tds[0].find('"'));
		p_id = atoi(temp.c_str());
		
		
		temp.assign(tds[4].begin() + tds[4].find(">") + 1, tds[4].end() - 1);
		p_value = atoi(temp.c_str());

		result.push_back(make_pair(p_id, p_value));

	}

	
	return result;
}

