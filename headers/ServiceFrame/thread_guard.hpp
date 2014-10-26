#ifndef THREAD_GUARD
#define THREAD_GUARD




template <class LOCK>
class ThreadGuard
{
private:
	LOCK								&m_lock;


public:
	bool								m_dismissed;
	
	ThreadGuard(LOCK &lock):m_lock(lock),m_dismissed(false)
	{
		m_lock.acquire();
	}

	ThreadGuard(const ThreadGuard& other):m_dismissed(other.m_dismissed)
	{
		other.m_dismissed = true;
	}

	~ThreadGuard(void) throw()
	{
		if ( m_dismissed == false )
			m_lock.release();
	}

	void Dismiss(void) throw()
	{
		m_lock.release();
		m_dismissed = true;
	}


private:
	ThreadGuard & operator = (const ThreadGuard &);
};




#endif