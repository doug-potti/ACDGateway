#ifndef FSERVER_LOG_20041011__H
#define FSERVER_LOG_20041011__H




#include <finesyslog.hpp>

class SysLogSP : public SysLogB
{
public:
	SysLogSP(HWND p_hwnd)
	{
		m_hWnd = p_hwnd;
	}


	~SysLogSP()
	{
	}


	void OutputWin(std::string & o);


private:
	HWND								m_hWnd;
	char								m_buf[4096];
};




IFINE_SYSLOG * CreateLogSP(HWND p_hwnd);




#define OUTTOWINDOWLOG_CREATE(name, hwnd)	\
	name = CreateLogSP(hwnd)




OUTTOLOG_EXTDECLARE(g_spLog);




#define SPLOG(level)																	\
	if ( level <= g_spLog->m_logLevel )	{												\
	g_spLog->GetOStream() << outputtime << IFINE_SYSLOG::LevelDes[level + 1] << "\t"


#define SPLOGEND(level)				\
	g_spLog->PopInfo(level);			}




#endif