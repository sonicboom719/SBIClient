///////////////////////////////////////////////////////////////////////////////
//	CDateクラス: 汎用日付管理クラス
///////////////////////////////////////////////////////////////////////////////

#ifndef CDATE_H_
#define CDATE_H_

///////////////////////////////////////////////////////////////////////////////
//	header include

#include <stdio.h>
#include <time.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
//	macro definitions

#define TABLE_NAME_BYTES		32

#define LDATE_MAX				99991231L
#define LDATE_MIN				19000101L

///////////////////////////////////////////////////////////////////////////////
//	type definitions

typedef long	LDATE;

///////////////////////////////////////////////////////////////////////////////
//	class

class CDate
{
public:
	//	public members
	int		m_year;
	int		m_month;
	int		m_day;

public:
	enum DayOfWeek {
		SUNDAY = 0,
		MONDAY,
		TUESDAY,
		WEDNESDAY,
		THURSDAY,
		FRIDAY,
		SATURDAY
	};

public:
	CDate();
	CDate( LDATE lDate );
	CDate( const CDate& source );
	CDate( const char* szDate );
	~CDate();

	bool	FromValue( LDATE lDate );
	bool	FromString( const char* szDate );
	void	FromToday();
	void	FromTimeValue( time_t timer );

	LDATE	ToValue() const {return m_year * 10000 + m_month * 100 + m_day;}
	void	SetToday() { FromToday();}

	bool	IsSaturdaySunday() const;
	bool	IsUruudoshi() const;
	bool	IsSyukujitsu() const;
	bool	IsFurikaeKyujitsu() const;
	bool	IsHoliday() const;
	bool	IsMarketOpenDate() const;
	bool	IsFirstMarketDateOfMonth() const;
	bool	IsFiveDayWorkWeek() const;
	bool	IsBeforeMajorSQDay() const;
	int		GetBeforeMajorSQDay() const;
	int		GetEndDayThisMonth() const;
	int		GetEndMarketOpenDayThisMonth() const;
	int		GetDayNumberOfYear() const;

	int		GetDayWeek() const;
	CDate	GetNextWeekday() const;
	int		DiffMonthFrom(const CDate& other) const;
	int		DiffMarketOpenDayFrom( CDate other ) const;
	void	AddMonth(int value);
	void	AddMarketOpenDay(int value);
	void	NextMarketOpenDate();
	void	PrevMarketOpenDate();
	void	NextWeekMarketOpenDate();
	void	PrevWeekMarketOpenDate();
	void	NextMonthMarketOpenDate();
	void	PrevMonthMarketOpenDate();

	//	string conversion
	std::string ToString(int format=0) const;

	CDate operator + (int value) const;
	CDate operator - (int value) const;
	int	operator - (const CDate& other) const;
	CDate& operator ++ ();
	CDate& operator -- ();

	CDate& operator = (const CDate& other);
	CDate& operator = (LDATE lDate);
	CDate& operator = (const char* szDate);
	CDate& operator += (int value);
	CDate& operator -= (int value);
	
	operator LDATE() const {return ToValue();};

	bool operator >= (const CDate& other) const {return (ToValue() >= other.ToValue());}
	bool operator >= (LDATE lDate) const {return (ToValue() >= lDate);}
	bool operator > (const CDate& other) const {return (ToValue() > other.ToValue());}
	bool operator > (LDATE lDate) const {return (ToValue() > lDate);}
	bool operator <= (const CDate& other) const {return (ToValue() <= other.ToValue());}
	bool operator <= (LDATE lDate) const {return (ToValue() <= lDate);}
	bool operator < (const CDate& other) const {return (ToValue() < other.ToValue());}
	bool operator < (LDATE lDate) const {return (ToValue() < lDate);}
	bool operator == (const CDate& other) const {return (ToValue() == other.ToValue());}
	bool operator == (LDATE lDate) const {return (ToValue() == lDate);}
	bool operator != (const CDate& other) const {return (ToValue() != other.ToValue());}
	bool operator != (LDATE lDate) const {return (ToValue() != lDate);}

	//	static functions
	static int	GetEndDayOfMonth( int month, int year );
	static bool	IsUruudoshi( int year );
	static LDATE Today();

private:
	//	private helpers
	void Init();

	//	static helpers
	static int vernal_equinox( int year );
	static int autumnal_equinox( int year );
	static int ParseMonth( const char* szMonth );
	
	//	private data members
	char m_str[32];
};

#endif