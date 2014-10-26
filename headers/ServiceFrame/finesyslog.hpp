#ifndef FINE_SYSLOG_20041010__H
#define FINE_SYSLOG_20041010__H




#include <iosfwd>
#include <istream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <thread_guard.hpp>
#include <thread_cs.hpp>




class IFINE_SYSLOG
{
public:
	enum Level
	{
		// Log from standard error stream
		NoLog = -1,
		// Log a fatal error
		Fatal,
		// Log a non-fatal error
		Error,
		// Log a warning
		Warning,
		// Log general information
		Info,
		// Log net information
		Net,
		// Log debugging information
		Debug,
		// Log more debugging information
		Debug2,
		// Log even more debugging information
		Debug3,
		// Log a lot of debugging information
		Debug4,
		// Log a real lot of debugging information
		Debug5,
		// Log a bucket load of debugging information
		Debug6,

		All,

		NumLogLevels
	};


	static std::string					LevelDes[13];


public:
	IFINE_SYSLOG() {}
	virtual ~IFINE_SYSLOG() = 0 {}


	virtual std::ostream & GetOStream(void) = 0 {}
	virtual void PopInfo(Level) = 0 {}
	void SetLevel(Level);
	void ChangeLogName(std::string p_logName);
	void Lock(void);
	void Unlock(void);


	std::string							m_logName;
	Level								m_logLevel;
	ThreadCriticalSection				m_logMutex;


private:
	ThreadGuard<ThreadCriticalSection>	* m_logGuard;
};




template <class charT, class traits>
inline
std::basic_ostream<charT, traits>&
outputtime (std::basic_ostream<charT, traits>& strm)
{
	SYSTEMTIME							now;
	char								timebuf[32];


	GetLocalTime(&now);
	sprintf(timebuf,
			"%d/%2d/%2d %2d:%2d:%2d.%3d",
			now.wYear,
			now.wMonth,
			now.wDay,
			now.wHour,
			now.wMinute,
			now.wSecond,
			now.wMilliseconds);
	strm << timebuf << std::setw(20) << std::setfill(' ');
	return strm;
}




IFINE_SYSLOG * CreateLogF();


#define OUTTOLOG_DECLARE(name)	\
	IFINE_SYSLOG * name = NULL


#define OUTTOLOG_EXTDECLARE(name)	\
	extern IFINE_SYSLOG * name


#define OUTTOFILELOG_CREATE(name)	\
	name = CreateLogF()


#define OUTTOLOG_DESTROY(name)	\
	if ( name ){				\
		delete name;			\
		name = NULL;}




IFINE_SYSLOG * CreateLogC();


#define OUTTOCONSOLELOG_CREATE(name)	\
	name = CreateLogC()




/************************************************************************
 *																		*
 *							CLASS OF SysLogB							*
 *																		*
 ************************************************************************/


class SysLogB : public IFINE_SYSLOG
{
public:
	SysLogB();


	~SysLogB();


	std::ostream & GetOStream(void);


	void PopInfo(Level p_curLevel);


	virtual void OutputWin(std::string &) = 0 {}


private:
	std::ofstream						m_logF;
	std::ostringstream					m_o;
	std::string							m_s;
	SysLogB(const SysLogB &);
	const SysLogB & operator = (const SysLogB &);
};




#endif