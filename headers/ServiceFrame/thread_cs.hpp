#ifndef THREAD_CRITICAL_SECTION
#define THREAD_CRITICAL_SECTION




#pragma once
#include "windows.h"




#pragma warning(disable:4786)




class ThreadCriticalSection
{
public:
	ThreadCriticalSection(void) { InitializeCriticalSection (&m_cs); }
	~ThreadCriticalSection(void) { DeleteCriticalSection(&m_cs); }
	int acquire(void) { EnterCriticalSection(&m_cs); return 0; }
	int release(void) { LeaveCriticalSection(&m_cs); return 0; }


private:
	CRITICAL_SECTION					m_cs;

	
	// = Disallow copying and assignment.
	ThreadCriticalSection(const ThreadCriticalSection &);
	void operator= (const ThreadCriticalSection &);
};

#endif