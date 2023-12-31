///////////////////////////////////////////////////////////////////////////////
//	CDateクラス
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
//	CDate 構築と破棄

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
//	CDate ヘルパー関数

void CDate::Init()
{
	m_year = 0;
	m_month = 0;
	m_day = 0;
}

///////////////////////////////////////////////////////////////////////////////
//	CDate 静的ヘルパー関数

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
//	CDate 操作

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
		//	yymmdd 形式の場合
		unsigned int dateValue = atoi(szBuf);
		iNum[0] = dateValue / 10000;
		iNum[1] = (dateValue - iNum[0] * 10000) / 100;
		iNum[2] = dateValue - iNum[0] * 10000 - iNum[1] * 100;
	}
	else if (strspn(szBuf, "0123456789/.-") >= 5)
	{
		//	y/m/d or y.m.d y-m-d 形式の場合 
		pToken = strtok(szBuf, "./-");
		while (pToken != NULL){
			iNum[num++] = atoi(pToken);
			pToken = strtok(NULL, "./-");
		}
		if (num < 3)
			return false;	//	numeric should be 3 set
	}
	else if (_mbsspn((unsigned char*)szBuf, (unsigned char*)"0123456789年月日") >= 6)
	{
		//	y年m月d日 形式の場合 
		pToken = (char*)_mbstok((unsigned char*)szBuf, (unsigned char*)"年月日");
		while (pToken != NULL){
			iNum[num++] = atoi(pToken);
			pToken = (char*)_mbstok(NULL, (unsigned char*)"年月日");
		}
		if (num < 3)
			return false;	//	numeric should be 3 set
	}
	else if (strspn( szBuf, "JanFebMarAprMayJunJulAugSepOctNovDec0123456789 ,") >= 10)
	{
		//	Month d, y 形式の場合 
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
	//	2つのCDateオブジェクト間に何ヶ月分の差があるかを返す
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
		if (	m_day == 1		//	元旦
			||	m_day == 15)	//	成人の日
			return true;
		else return false;
	}
	if (m_month == 2){
		if (m_day == 11)	//	建国記念日
			return true;
		else return false;
	}	
	if (m_month == 3){
		if (m_day == 20)	//	春分の日
			return true;
		else return false;
	}
	if (m_month == 4){
		if (m_day == 29)	//	みどりの日
			 return true;
		else return false;
	}
	if (m_month == 5){
		if (	m_day == 3		//	憲法記念日
			||	m_day == 4		//	国民の休日
			||	m_day == 5)		//	こどもの日
			return true;
		else return false;
	}
	if (m_month == 7){
		if (m_day == 20)	//	うみの日
			return true;
		else return false;
	}
	if (m_month == 9){
		if (	m_day == 15	//	敬老の日
			||	m_day == 23)	//	秋分の日
			return true;
		else return false;
	}
	if (m_month == 10){
		if (m_day == 10)	//	体育の日
			return true;
		else return false;
	}
	if (m_month == 11){
		if (	m_day == 3		//	文化の日
			||	m_day == 23)	//	勤労感謝の日
			return true;
		else return false;
	}
	if (m_month == 12 && m_day == 23) return true;	//	天皇誕生日

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
	static char* jdayweek[] = { "日","月","火","水","木","金","土" };
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
					strcpy( buf, "今日" );
				else if (*this == date - 1)
					strcpy( buf, "昨日" );
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

    // その月の何番目の○曜日か？
    const int number = (int)((day - 1) / 7) + 1;

    switch (m_month) {
        case 1:
            // 元旦
            if (day == 1) return true;
            // 成人の日(第2月曜日)
            return (year <= 1999)
                ? (day == 15)
                : (dow == MONDAY && number == 2);
        case 2:
            // 建国記念の日
            if (day == 11) return true;
            // 昭和天皇の大喪の礼
            return year == 1989 && day == 24;
        case 3:
            // 春分の日
            return day == vernal_equinox(year);
        case 4:
            // 皇太子明仁親王の結婚の儀
            if (year == 1959 && day == 10) return true;
            // 昭和の日
            return day == 29;
        case 5:
            // 憲法記念日
            if (day == 3) return true;
            // [1986〜]国民の休日, [2007〜]みどりの日
            if (day == 4 && year >= 1986) return true;
            // こどもの日
            return day == 5;
        case 6:
            // 皇太子徳仁親王の結婚の儀
            return year == 1993 && day == 9;
        case 7:
            // 海の日(第3月曜日)
            if (year <= 1995) return false;
            if (year <= 2002) return day == 20;
            return dow == MONDAY && number == 3;
        case 9:
			{
            // 敬老の日
            if (year <= 2002) {
                if (day == 15) return true;
            } else {
                if (dow == MONDAY && number == 3)
                    return true;
            }
            // 秋分の日
            int aut_eq = autumnal_equinox(year);
            if (day == aut_eq) return true;
            // 国民の休日(敬老の日と秋分の日に挟まれた場合)
            if (year >= 2003 && dow == TUESDAY) {
                int prev_number = (int)(((day - 1) - 1) / 7) + 1;
                if (prev_number == 3 && (day + 1) == aut_eq)
                    return true;
            }
            return false;
			}
        case 10:
            // 体育の日
            return (year <= 1999)
                ? (day == 10)
                : (dow == MONDAY && number == 2);
        case 11:
            // 即位礼正殿の儀
            if (year == 1990 && day == 12) return true;
            // 文化の日, 勤労感謝の日
            return day == 3 || day == 23;
        case 12:
            // 天皇誕生日
            return year >= 1989 && day == 23;
        default:
            return false;
    }
}

bool CDate::IsBeforeMajorSQDay() const
{
	//	3の倍数の月でなければFALSE
	if (m_month % 3) return false;

	//	その月のSQ日(第2金曜日)を求める
	CDate lastTradeDate(*this);
	lastTradeDate.m_day = 1;
	while (lastTradeDate.GetDayWeek() != FRIDAY) ++lastTradeDate;
	lastTradeDate = lastTradeDate + 7;
	//	その前営業日(最終取引日)を求める
	do { --lastTradeDate; } while (!lastTradeDate.IsMarketOpenDate());

	return (*this == lastTradeDate);
}

int CDate::GetBeforeMajorSQDay() const
{
	//	3の倍数の月でなければ無効値を返す
	if (m_month % 3) return 0;

	//	その月のSQ日(第2金曜日)を求める
	CDate lastTradeDate(*this);
	lastTradeDate.m_day = 1;
	while (lastTradeDate.GetDayWeek() != FRIDAY) ++lastTradeDate;
	lastTradeDate = lastTradeDate + 7;
	//	その前営業日(最終取引日)を求める
	do { --lastTradeDate; } while (!lastTradeDate.IsMarketOpenDate());

	return lastTradeDate.m_day;
}

bool CDate::IsMarketOpenDate() const
{
	const int dow = GetDayWeek();

	// 日曜日
    if (dow == SUNDAY) return false;

    // 株式会社東京証券取引所 業務規定による休業日
    if (m_month == 1 && m_day <= 3) return false;
    if (m_month == 12) {
        if (m_year >= 1989 && m_day == 31) return false;
        if (m_year <= 1988 && m_day >= 29) return false;
    }
    // 土曜日
    if (dow == SATURDAY) {
        if (IsFiveDayWorkWeek()) return false;
    }
    // 国民の祝日
    if (IsHoliday()) return false;

    // 振替休日
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
	//	【警告】暫定的な実装です。正しくないかも。
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
	//	明らかに開場日でないならFALSEを返す(多少の高速化になる)
	if (m_day >= 8) return false;

	//	当月の最初の開場日を求める
	CDate first(*this);
	first.m_day = 1;
	while (!first.IsMarketOpenDate()) ++first;

	// 最初の開場日と同じか？
	return (*this == first);
}

bool CDate::IsFiveDayWorkWeek() const
{
    // その月の何番目の○曜日か？
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
