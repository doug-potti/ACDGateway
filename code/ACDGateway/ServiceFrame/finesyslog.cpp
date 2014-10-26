#include <finesyslog.hpp>




std::string								IFINE_SYSLOG::LevelDes[13] =
{		"NoLog",
		// Log a fatal error
		"Fatal",
		// Log a non-fatal error
		"Error",
		// Log a warning
		"Warning",
		// Log general information
		"Info",
		// Log net information
		"Net",
		// Log debugging information
		"Debug",
		// Log more debugging information
		"Debug2",
		// Log even more debugging information
		"Debug3",
		// Log a lot of debugging information
		"Debug4",
		// Log a real lot of debugging information
		"Debug5",
		// Log a bucket load of debugging information
		"Debug6",

		"All"
};




void IFINE_SYSLOG::ChangeLogName(std::string p_logName)
{
	ThreadGuard<ThreadCriticalSection>	logGuard(m_logMutex);
	m_logName = p_logName;
}




void IFINE_SYSLOG::SetLevel(Level p_level)
{
	m_logLevel = p_level;
}




void IFINE_SYSLOG::Lock(void)
{
	m_logGuard = new ThreadGuard<ThreadCriticalSection>(m_logMutex);
}


void IFINE_SYSLOG::Unlock(void)
{
	delete m_logGuard;
}




/************************************************************************
 *																		*
 *							CLASS OF SysLogF							*
 *																		*
 ************************************************************************/


class SysLogF : public IFINE_SYSLOG
{
public:
	SysLogF()
	{
		m_logF = NULL;
		m_strBuf = std::cout.rdbuf();
	}


	~SysLogF()
	{
		if ( m_logF )
			delete m_logF;
		if ( m_strBuf )
			std::cout.rdbuf(m_strBuf);
	}


	std::ostream & GetOStream(void)
	{
		Lock();
		if ( m_logF )
		{
			delete m_logF;
			m_logF = NULL;
		}

		m_logF = new std::ofstream(m_logName.c_str(), std::ios::app);

		std::cout.rdbuf(m_logF->rdbuf());

		return std::cout;
	}


	void PopInfo(Level p_curLevel)
	{
		if ( m_logF )
		{
			delete m_logF;
			m_logF = NULL;
		}
		Unlock();
	}


private:
	std::ofstream						* m_logF;
	std::streambuf						* m_strBuf;
};




IFINE_SYSLOG * CreateLogF()
{
	return new SysLogF;
}




/************************************************************************
 *																		*
 *							CLASS OF SysLogC							*
 *																		*
 ************************************************************************/


class SysLogC : public IFINE_SYSLOG
{
public:
	SysLogC()
	{
	}


	~SysLogC()
	{
	}


	std::ostream & GetOStream(void)
	{
		Lock();
		return std::cout;
	}


	void PopInfo(Level p_curLevel)
	{
		Unlock();
	}
};




IFINE_SYSLOG * CreateLogC()
{
	return new SysLogC;
}




SysLogB::SysLogB()
{
}


SysLogB::~SysLogB()
{
}




std::ostream & SysLogB::GetOStream(void)
{
	Lock();
	if ( m_logName.length() != 0 )
	{
		m_logF.open(m_logName.c_str(), std::ios::app);
	}

	return m_o;
}


void SysLogB::PopInfo(Level p_curLevel)
{
	m_s = m_o.str();
	if ( m_logName.length() != 0 )
	{
		if ( m_logF.is_open() )
		{
			m_logF << m_s << std::endl << std::flush;
			m_logF.clear();
			m_logF.close();
		}
	}
	if ( NoLog != m_logLevel )
	{
		OutputWin(m_s);
	}
	m_o.seekp(0);
	m_o.clear();
	Unlock();
}