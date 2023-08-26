///////////////////////////////////////////////////////////////////////////////
//	CDateƒNƒ‰ƒX
///////////////////////////////////////////////////////////////////////////////

#pragma warning (disable: 4996)

///////////////////////////////////////////////////////////////////////////////
//	local header include

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mbstring.h>
#include "CDate.h"

///////////////////////////////////////////////////////////////////////////////
//	CDate \’z‚Æ”jŠü

CDate::CDate()
{
	Init();
}

CDate::CDate( LDATE lDate )
{
	Init();
	FromValue(lDate);
}

CDate::CDate( const CDate& source )
{
	Init();
	*this = source;
}

CDate::CDate( const char* szDate )
{
	Init();
	FromString(szDate);
}

CDate::~CDate()
{
}

///////////////////////////////////////////////////////////////////////////////
//	CDate ƒwƒ‹ƒp[ŠÖ”

void CDate::Init()
{
	m_year = 0;
	m_month = 0;
	m_day = 0;
}

///////////////////////////////////////////////////////////////////////////////
//	CDate Ã“Iƒwƒ‹ƒp[ŠÖ”

int CDate::vernal_equinox( int year )
{
    double equinox = 0.242194 * (year - 1980);
    return (year < 1980)
        ? ((int)(20.8357 + equinox - (int)((year - 1983) / 4)))
        : ((int)(20.8431 + equinox - (int)((year - 1980) / 4)));
}

int CDate::autumnal_equinox( int year )
{
    double equinox = 0.242194 * (year - 1980);
    return (year < 1980)
        ? ((int)(23.2588 + equinox - (int)((year - 1983) / 4)))
        : ((int)(23.2488 + equinox - (int)((year - 1980) / 4)));
}

int CDate::ParseMonth( const char* szMonth )
{
	static const char* table[] =
		{ "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec",NULL };

	for (int i = 0; table[i] != NULL; i++) {
		if (stricmp( szMonth, table[i] ) == 0)
			return i + 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//	CDate ‘€ì

bool CDate::FromValue( LDATE lDate )
{
	int	year, month, day;

	if (lDate == 0) {
		m_year = 0;
		m_month = 0;
		m_day = 0;
		return false;
	}

	year = (int) lDate / 10000;
	month = (int) (lDate - year * 10000) / 100;
	day = lDate % 100;

	if (month < 1 || month > 12) return false;
	if (day < 1 || day > 31) return false;

	m_year = year;
	m_month = month;
	m_day = day;

	return true;
}

bool CDate::FromString( const char* szDate )
{
	int		i, iNum[16], index = 0, num = 0;
	char	szBuf[256], *pToken;

	//	trim left space
	for (i=0; szDate[i] == ' ' && i < 256 - 2; i++) ;
	if (i >= 256)
		return false;	//	only space
	
	//	local copy
	strncpy(szBuf, &szDate[i], 256 - 1);
	szBuf[255] = '\0';
	
	//	char set filter
	if (strspn(szBuf, "0123456789") == 6 || strspn(szBuf, "0123456789") == 8)
	{
		//	yymmdd Œ`®‚Ìê‡
		unsigned int dateValue = atoi(szBuf);
		iNum[0] = dateValue / 10000;
		iNum[1] = (dateValue - iNum[0] * 10000) / 100;
		iNum[2] = dateValue - iNum[0] * 10000 - iNum[1] * 100;
	}
	else if (strspn(szBuf, "0123456789/.-") >= 5)
	{
		//	y/m/d or y.m.d y-m-d Œ`®‚Ìê‡ 
		pToken = strtok(szBuf, "./-");
		while (pToken != NULL){
			iNum[num++] = atoi(pToken);
			pToken = strtok(NULL, "./-");
		}
		if (num < 3)
			return false;	//	numeric should be 3 set
	}
	else if (_mbsspn((unsigned char*)szBuf, (unsigned char*)"0123456789”NŒ“ú") >= 6)
	{
		//	y”NmŒd“ú Œ`®‚Ìê‡ 
		pToken = (char*)_mbstok((unsigned char*)szBuf, (unsigned char*)"”NŒ“ú");
		while (pToken != NULL){
			iNum[num++] = atoi(pToken);
			pToken = (char*)_mbstok(NULL, (unsigned char*)"”NŒ“ú");
		}
		if (num < 3)
			return false;	//	numeric should be 3 set
	}
	else if (strspn( szBuf, "JanFebMarAprMayJunJulAugSepOctNovDec0123456789 ,") >= 10)
	{
		//	Month d, y Œ`®‚Ìê‡ 
		pToken = strtok( szBuf, " ,");
		if (pToken == NULL) return false;
		iNum[1] = ParseMonth(pToken);
		pToken = strtok( NULL, " ,");
		if (pToken == NULL) return false;
		iNum[2] = atoi(pToken);
		pToken = strtok( NULL, " ,");
		if (pToken == NULL) return false;
		iNum[0] = atoi(pToken);
	}
	else return false;

	//	value range filter
	if (iNum[1] < 1 || iNum[1] > 12)
		return false;
	if (iNum[2] < 1 || iNum[2] > 31)
		return false;
	if (iNum[0] < 100){
		//	trim year number
		if (iNum[0] > 70) iNum[0] += 1900;
		else iNum[0] += 2000;
	}
	if (iNum[0] < 0)
		return false;

	//	set date
	m_year = iNum[0];
	m_month = iNum[1];
	m_day = iNum[2];

	return true;
}

void CDate::FromToday()
{
   time_t timer;
   struct tm *t_st;

   time(&timer);
   t_st = localtime(&timer);
   m_year = t_st->tm_year + 1900;
   m_month = t_st->tm_mon + 1;
   m_day = t_st->tm_mday;
}

LDATE CDate::Today()
{
	CDate date;
	date.FromToday();
	return date;
}

void CDate::FromTimeValue( time_t timer )
{
   struct tm *t_st;

   t_st = localtime(&timer);
   m_year = t_st->tm_year + 1900;
   m_month = t_st->tm_mon + 1;
   m_day = t_st->tm_mday;

}

CDate CDate::operator + (int value) const
{
	CDate date(*this);
	for (int i=0; i<value; i++)
		++date;
	return date;
}

CDate CDate::operator - (int value) const
{
	CDate date(*this);
	for (int i=0; i<value; i++)
		--date;
	return date;
}

int CDate::operator - (const CDate& other) const
{
	int iSign, year1, year2, daynum1, daynum2;
	if (m_year < other.m_year){
		year1 = m_year;
		daynum1 = GetDayNumberOfYear();
		year2 = other.m_year; 
		daynum2 = other.GetDayNumberOfYear();
		iSign = -1;
	}else{
		year1 = other.m_year;
		daynum1 = other.GetDayNumberOfYear();
		year2 = m_year; 
		daynum2 = GetDayNumberOfYear();
		iSign = 1;
	}
	for (int i=year1; i<year2; i++)
		daynum2 += IsUruudoshi(i) ? 366 : 365;
	return (daynum2 - daynum1) * iSign;
}

CDate& CDate::operator ++ ()
{
	if (m_day + 1 > GetEndDayThisMonth()){
		m_day = 1;
		m_month++;
		if (m_month > 12){
			m_month = 1;
			m_year++;
		}
	}
	else m_day++;
	return *this;
}

CDate& CDate::operator -- ()
{
	if (m_day == 1){
		m_month--;
		if (m_month < 1){
			m_year--;
			m_month = 12;
		}
		m_day = GetEndDayThisMonth();
	}
	else m_day--;
	return *this;
}

CDate& CDate::operator = ( const CDate& other )
{
	m_year = other.m_year;
	m_month = other.m_month;
	m_day = other.m_day;
	return *this;
}

CDate& CDate::operator = ( LDATE lDate )
{
	FromValue(lDate);
	return *this;
}

CDate& CDate::operator = ( const char* szDate )
{
	FromString(szDate);
	return *this;
}

CDate& CDate::operator += (int value)
{
	*this = *this + value;
	return *this;
}

CDate& CDate::operator -= (int value)
{
	*this = *this - value;
	return *this;
}

int CDate::GetEndDayThisMonth() const
{
	return GetEndDayOfMonth( m_month, m_year );
}

int CDate::GetEndDayOfMonth( int month, int year )
{
	static int lastDay[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (month == 2)
		return IsUruudoshi(year) ? 29 : 28; 
	else return lastDay[month - 1];
}

int CDate::GetEndMarketOpenDayThisMonth() const
{
	CDate date(*this);
	date.m_day = date.GetEndDayThisMonth();
	while (!date.IsMarketOpenDate()) --date;
	return date.m_day;
}

CDate CDate::GetNextWeekday() const
{
	CDate date(*this);
	do {
		++date;
	}while (date.IsSaturdaySunday() || date.IsSyukujitsu() || date.IsFurikaeKyujitsu());
	return date;
}

int CDate::DiffMonthFrom(const CDate& other) const
{
	//	2‚Â‚ÌCDateƒIƒuƒWƒFƒNƒgŠÔ‚É‰½ƒ–Œ•ª‚Ì·‚ª‚ ‚é‚©‚ğ•Ô‚·
	int sign, year1, year2, month1, month2;
	if (m_year < other.m_year) {
		year1 = m_year;
		month1 = m_month;
		year2 = other.m_year; 
		month2 = other.m_month;
		sign = -1;
	} else if (m_year > other.m_year) {
		year1 = other.m_year;
		month1 = other.m_month;
		year2 = m_year; 
		month2 = m_month;
		sign = 1;
	} else {
		return m_month - other.m_month;
	}
	return ((year2 - year1 - 1)*12 + month2 + 12 - month1) * sign;
}

int CDate::DiffMarketOpenDayFrom( CDate other ) const
{
	int count = 0;

	if (*this > other) {
		do {
			++other;
			if (other.IsMarketOpenDate()) count++;
		} while (other != *this);
	}
	else if (*this < other) {
		do {
			--other;
			if (other.IsMarketOpenDate()) count--;
		} while (other != *this);
	}

	return count;
}

void CDate::AddMonth(int value)
{
	if (value > 0) {
		m_year += value / 12;
		m_month += value % 12;
		if (m_month > 12) {
			m_year++;
			m_month -= 12;
		}
	}
	else if (value < 0) {
		m_year -= -value / 12;
		m_month -= -value % 12;
		if (m_month < 1) {
			m_year--;
			m_month += 12;
		}
	}
}

void CDate::AddMarketOpenDay(int value)
{
	int count = 0;

	if (value > 0) {
		while (1) {
			++(*this);
			if (IsMarketOpenDate()) {
				count++;
				if (count == value) return;
			}
		}
	}
	else if (value < 0) {
		while (1) {
			--(*this);
			if (IsMarketOpenDate()) {
				count--;
				if (count == value) return;
			}
		}
	}
}

bool CDate::IsUruudoshi() const
{
	return IsUruudoshi(m_year);
}

bool CDate::IsUruudoshi( int year )
{
	return ((year % 4 == 0) && (year % 100) || (year % 400 == 0));
}

bool CDate::IsSyukujitsu() const
{
	if (m_month == 1){
		if (	m_day == 1		//	Œ³’U
			||	m_day == 15)	//	¬l‚Ì“ú
			return true;
		else return false;
	}
	if (m_month == 2){
		if (m_day == 11)	//	Œš‘‹L”O“ú
			return true;
		else return false;
	}	
	if (m_month == 3){
		if (m_day == 20)	//	t•ª‚Ì“ú
			return true;
		else return false;
	}
	if (m_month == 4){
		if (m_day == 29)	//	‚İ‚Ç‚è‚Ì“ú
			 return true;
		else return false;
	}
	if (m_month == 5){
		if (	m_day == 3		//	Œ›–@‹L”O“ú
			||	m_day == 4		//	‘–¯‚Ì‹x“ú
			||	m_day == 5)		//	‚±‚Ç‚à‚Ì“ú
			return true;
		else return false;
	}
	if (m_month == 7){
		if (m_day == 20)	//	‚¤‚İ‚Ì“ú
			return true;
		else return false;
	}
	if (m_month == 9){
		if (	m_day == 15	//	Œh˜V‚Ì“ú
			||	m_day == 23)	//	H•ª‚Ì“ú
			return true;
		else return false;
	}
	if (m_month == 10){
		if (m_day == 10)	//	‘Ìˆç‚Ì“ú
			return true;
		else return false;
	}
	if (m_month == 11){
		if (	m_day == 3		//	•¶‰»‚Ì“ú
			||	m_day == 23)	//	‹Î˜JŠ´Ó‚Ì“ú
			return true;
		else return false;
	}
	if (m_month == 12 && m_day == 23) return true;	//	“Vc’a¶“ú

	return false;
}

bool CDate::IsSaturdaySunday() const
{
	int dayweek = GetDayWeek();
	return (dayweek == 0 || dayweek == 6);
}

bool CDate::IsFurikaeKyujitsu() const
{
	if (IsSaturdaySunday() || IsSyukujitsu()) return false;
	CDate date(*this);
	--date;
	if (date.IsSaturdaySunday())
	{
		if (date.IsSyukujitsu())
			return true;
		--date;
		if (date.IsSaturdaySunday() && date.IsSyukujitsu())
			return true;
	}
	return false;
}
	
std::string CDate::ToString( int format ) const
{
	//	warning!: not aware multi threads access.
	static char* dayweek[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
	static char* jdayweek[] = { "“ú","Œ","‰Î","…","–Ø","‹à","“y" };
	char buf[32];

	switch (format) {
	case 0:
		if (m_year && m_month && m_day)
			sprintf( buf, "%u/%u/%u", m_year, m_month, m_day );
		else
			strcpy( buf, "----/--/--" );
		break;
	case 1:
		if (m_year && m_month && m_day)
			sprintf( buf, "%02u/%u", m_year % 100, m_month );
		else
			strcpy( buf, "--/--" );
		break;
	case 2:
		if (m_year && m_month && m_day)
			sprintf( buf, "%u/%u/%u(%s)", m_year, m_month, m_day, jdayweek[GetDayWeek()] );
		else
			strcpy( buf, "----/--/--(--)" );
		break;
	case 3:
		{
			if (m_year && m_month && m_day) {
				CDate date;
				date.FromToday();
				if (*this == date)
					strcpy( buf, "¡“ú" );
				else if (*this == date - 1)
					strcpy( buf, "ğ“ú" );
				else
					sprintf( buf, "%u/%u/%u(%s)", m_year, m_month, m_day, jdayweek[GetDayWeek()] );
			}
			else strcpy( buf, "----/--/--(--)" );
		}
		break;
	case 4:
		if (m_year && m_month && m_day)
			sprintf( buf, "%02u/%u/%u", m_year % 100, m_month, m_day );
		else
			strcpy( buf, "--/--/--" );
		break;
	case 5:
		sprintf( buf, "%u", ToValue() );
		break;
	case 6:
		if (m_year && m_month && m_day)
			sprintf( buf, "%u", m_year );
		else
			strcpy( buf, "----" );
		break;
	case 7:
		if (m_year && m_month && m_day)
			if (m_month == 1)
				sprintf( buf, "%u", m_year );
			else
				sprintf( buf, "%02u/%u", m_year % 100, m_month );
		else
			strcpy( buf, "--/--" );
		break;
	}
	return std::string( buf );
}


int CDate::GetDayWeek() const
{
	int year, month, day;
	if (m_month < 1 || m_month > 12) return -1;
	if (m_day < 1 || m_day > 31) return -1;
	year = m_year;
	month = m_month;
	day = m_day;
	if (month < 3) { year--; month += 12;}	
	return (year + year / 4 - year / 100 + year / 400 + (13 * month + 8) / 5 + day) % 7;
}

int CDate::GetDayNumberOfYear() const
{
	//	DayNumber is index (0-364/365). DayNumber of 1,Jan is zero.
	int month = m_month;
	int iDayNumber = m_day - 1;
	while (month-- > 1)
		iDayNumber += GetEndDayOfMonth(month, m_year);
	return iDayNumber;
}

bool CDate::IsHoliday() const
{
    const int year = m_year;
    const int day = m_day;
    const int dow = GetDayWeek();

    // ‚»‚ÌŒ‚Ì‰½”Ô–Ú‚Ì›—j“ú‚©H
    const int number = (int)((day - 1) / 7) + 1;

    switch (m_month) {
        case 1:
            // Œ³’U
            if (day == 1) return true;
            // ¬l‚Ì“ú(‘æ2Œ—j“ú)
            return (year <= 1999)
                ? (day == 15)
                : (dow == MONDAY && number == 2);
        case 2:
            // Œš‘‹L”O‚Ì“ú
            if (day == 11) return true;
            // º˜a“Vc‚Ì‘å‘r‚Ì—ç
            return year == 1989 && day == 24;
        case 3:
            // t•ª‚Ì“ú
            return day == vernal_equinox(year);
        case 4:
            // c‘¾q–¾me‰¤‚ÌŒ‹¥‚Ì‹V
            if (year == 1959 && day == 10) return true;
            // º˜a‚Ì“ú
            return day == 29;
        case 5:
            // Œ›–@‹L”O“ú
            if (day == 3) return true;
            // [1986`]‘–¯‚Ì‹x“ú, [2007`]‚İ‚Ç‚è‚Ì“ú
            if (day == 4 && year >= 1986) return true;
            // ‚±‚Ç‚à‚Ì“ú
            return day == 5;
        case 6:
            // c‘¾q“¿me‰¤‚ÌŒ‹¥‚Ì‹V
            return year == 1993 && day == 9;
        case 7:
            // ŠC‚Ì“ú(‘æ3Œ—j“ú)
            if (year <= 1995) return false;
            if (year <= 2002) return day == 20;
            return dow == MONDAY && number == 3;
        case 9:
			{
            // Œh˜V‚Ì“ú
            if (year <= 2002) {
                if (day == 15) return true;
            } else {
                if (dow == MONDAY && number == 3)
                    return true;
            }
            // H•ª‚Ì“ú
            int aut_eq = autumnal_equinox(year);
            if (day == aut_eq) return true;
            // ‘–¯‚Ì‹x“ú(Œh˜V‚Ì“ú‚ÆH•ª‚Ì“ú‚É‹²‚Ü‚ê‚½ê‡)
            if (year >= 2003 && dow == TUESDAY) {
                int prev_number = (int)(((day - 1) - 1) / 7) + 1;
                if (prev_number == 3 && (day + 1) == aut_eq)
                    return true;
            }
            return false;
			}
        case 10:
            // ‘Ìˆç‚Ì“ú
            return (year <= 1999)
                ? (day == 10)
                : (dow == MONDAY && number == 2);
        case 11:
            // ‘¦ˆÊ—ç³“a‚Ì‹V
            if (year == 1990 && day == 12) return true;
            // •¶‰»‚Ì“ú, ‹Î˜JŠ´Ó‚Ì“ú
            return day == 3 || day == 23;
        case 12:
            // “Vc’a¶“ú
            return year >= 1989 && day == 23;
        default:
            return false;
    }
}

bool CDate::IsBeforeMajorSQDay() const
{
	//	3‚Ì”{”‚ÌŒ‚Å‚È‚¯‚ê‚ÎFALSE
	if (m_month % 3) return false;

	//	‚»‚ÌŒ‚ÌSQ“ú(‘æ2‹à—j“ú)‚ğ‹‚ß‚é
	CDate lastTradeDate(*this);
	lastTradeDate.m_day = 1;
	while (lastTradeDate.GetDayWeek() != FRIDAY) ++lastTradeDate;
	lastTradeDate = lastTradeDate + 7;
	//	‚»‚Ì‘O‰c‹Æ“ú(ÅIæˆø“ú)‚ğ‹‚ß‚é
	do { --lastTradeDate; } while (!lastTradeDate.IsMarketOpenDate());

	return (*this == lastTradeDate);
}

int CDate::GetBeforeMajorSQDay() const
{
	//	3‚Ì”{”‚ÌŒ‚Å‚È‚¯‚ê‚Î–³Œø’l‚ğ•Ô‚·
	if (m_month % 3) return 0;

	//	‚»‚ÌŒ‚ÌSQ“ú(‘æ2‹à—j“ú)‚ğ‹‚ß‚é
	CDate lastTradeDate(*this);
	lastTradeDate.m_day = 1;
	while (lastTradeDate.GetDayWeek() != FRIDAY) ++lastTradeDate;
	lastTradeDate = lastTradeDate + 7;
	//	‚»‚Ì‘O‰c‹Æ“ú(ÅIæˆø“ú)‚ğ‹‚ß‚é
	do { --lastTradeDate; } while (!lastTradeDate.IsMarketOpenDate());

	return lastTradeDate.m_day;
}

bool CDate::IsMarketOpenDate() const
{
	const int dow = GetDayWeek();

	// “ú—j“ú
    if (dow == SUNDAY) return false;

    // Š”®‰ïĞ“Œ‹ØŒ”æˆøŠ ‹Æ–±‹K’è‚É‚æ‚é‹x‹Æ“ú
    if (m_month == 1 && m_day <= 3) return false;
    if (m_month == 12) {
        if (m_year >= 1989 && m_day == 31) return false;
        if (m_year <= 1988 && m_day >= 29) return false;
    }
    // “y—j“ú
    if (dow == SATURDAY) {
        if (IsFiveDayWorkWeek()) return false;
    }
    // ‘–¯‚Ìj“ú
    if (IsHoliday()) return false;

    // U‘Ö‹x“ú
	CDate beforeDay1( *this - 1 );
	if (m_year >= 1973 && dow == MONDAY) {
        if (beforeDay1.IsHoliday()) return false;
    }
	CDate beforeDay2( *this - 2 );
    if (m_year >= 2007 && dow == TUESDAY) {
        if (beforeDay1.IsHoliday() &&
            beforeDay2.IsHoliday()) return false;
    }
	CDate beforeDay3( *this - 3 );
    if (m_year >= 2007 && dow == WEDNESDAY) {
        if (beforeDay1.IsHoliday() &&
            beforeDay2.IsHoliday() &&
            beforeDay3.IsHoliday()) return false;
    }
    return true;
}

void CDate::NextMarketOpenDate()
{
	do { ++(*this); } while (!IsMarketOpenDate());
}

void CDate::PrevMarketOpenDate()
{
	do { --(*this); } while (!IsMarketOpenDate());
}

void CDate::NextWeekMarketOpenDate()
{
	while (GetDayWeek() != SUNDAY) ++(*this);
	NextMarketOpenDate();
}

void CDate::PrevWeekMarketOpenDate()
{
	//	yŒxzb’è“I‚ÈÀ‘•‚Å‚·B³‚µ‚­‚È‚¢‚©‚àB
	*this -= 14;
	NextWeekMarketOpenDate();
}

void CDate::NextMonthMarketOpenDate()
{
	AddMonth(1);
	m_day = 1;
	if (!IsMarketOpenDate()) NextMarketOpenDate();
}

void CDate::PrevMonthMarketOpenDate()
{
	AddMonth(-1);
	m_day = 1;
	if (!IsMarketOpenDate()) NextMarketOpenDate();
}

bool CDate::IsFirstMarketDateOfMonth() const
{
	//	–¾‚ç‚©‚ÉŠJê“ú‚Å‚È‚¢‚È‚çFALSE‚ğ•Ô‚·(‘½­‚Ì‚‘¬‰»‚É‚È‚é)
	if (m_day >= 8) return false;

	//	“–Œ‚ÌÅ‰‚ÌŠJê“ú‚ğ‹‚ß‚é
	CDate first(*this);
	first.m_day = 1;
	while (!first.IsMarketOpenDate()) ++first;

	// Å‰‚ÌŠJê“ú‚Æ“¯‚¶‚©H
	return (*this == first);
}

bool CDate::IsFiveDayWorkWeek() const
{
    // ‚»‚ÌŒ‚Ì‰½”Ô–Ú‚Ì›—j“ú‚©H
    const int number = (int)((m_day - 1) / 7) + 1;

    if (m_year >= 1990) return true;
    if (m_year == 1989) return m_month >= 2 || number != 4;
    if (m_year >= 1987) return number == 2 || number == 3;
    if (m_year == 1986)
        return m_month >= 8
            ? (number == 2 || number == 3) : (number == 2);
    if (m_year >= 1984) return number == 2;
    if (m_year == 1983)
        return m_month >= 8
            ? (number == 2) : (number == 3);
    return number == 3;
}
