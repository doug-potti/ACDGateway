#ifndef FINE_STORAGEMANAGER_20050731__H
#define FINE_STORAGEMANAGER_20050731__H




#include <thread_cs.hpp>
#include <thread_guard.hpp>





template <typename T, int freeObjLimit>
class StorageManager
{
	struct StorageManagerLink
	{
		T *									head;
		T *									tail;
	};


public:
	ThreadCriticalSection				m_linkCs;
	StorageManagerLink					m_link;
	T *									m_pFreeObjLst;
	ThreadCriticalSection				m_objLstCs;


public:
	StorageManager();
	~StorageManager();
	bool Pop(T **p_Buf);
	bool Push(T * p_Buf);
	T * GetFreeObj(void);
	void FreeObj(T * p_obj);
	void DeleteFreeObj();
	int GetObjsNumberofLink();
	int GetObjNums();


private:
	// = Disallow copying and assignment.
	StorageManager(const StorageManager &);
	void operator= (const StorageManager &);
	int									m_newObjNums;
	int									m_freeObjNums;
};




template <typename T, int freeObjLimit>
StorageManager<T, freeObjLimit>::StorageManager()
{
    m_link.head = NULL;
	m_link.tail = NULL;
	m_pFreeObjLst = NULL;
	m_newObjNums = 0;
}




template <typename T, int freeObjLimit>
StorageManager<T, freeObjLimit>::~StorageManager()
{
	T							* delData;


	// Constructor of <mon> automatically
	// acquires the <mutex>	
	ThreadGuard<ThreadCriticalSection>	mon(m_linkCs);


	/* check all left data which is not retrieved */
	while ( m_link.head )
	{
		delData = m_link.head;
		m_link.head = m_link.head->next;
		if ( m_link.head == NULL )
			m_link.tail = NULL;

		if ( delData )
		{
			delete delData;
			delData = NULL;
		}
	}

	mon.Dismiss();

	DeleteFreeObj();
}




template <typename T, int freeObjLimit>
bool StorageManager<T, freeObjLimit>::Pop(T **p_readData)
{
	bool								bRet = true;


	*p_readData = NULL;

	{
		// Constructor of <mon> automatically
		// acquires the <mutex>	
		ThreadGuard<ThreadCriticalSection> mon(m_linkCs);

		if ( m_link.head == NULL )
		{
			bRet = false;
		}
		else
		{
			*p_readData = m_link.head;
			m_link.head = m_link.head->next;
			(*p_readData)->next = NULL;
			if ( m_link.head == NULL )
				m_link.tail = NULL;
		}

		mon.Dismiss();
	}
	
	return bRet;
}




template <typename T, int freeObjLimit>
bool StorageManager<T, freeObjLimit>::Push(T *Buf)
{
	bool								bRet = true;


	{
		// Constructor of <mon> automatically
		// acquires the <mutex>	
		ThreadGuard<ThreadCriticalSection> mon(m_linkCs);

		if ( m_link.head == NULL )
		{
			m_link.head = Buf;
			m_link.tail = Buf;
		}
		else
		{
			m_link.tail->next = Buf;
			m_link.tail = Buf;
		}

		// Destructor of Guard automatically
		// releases the lock, regardless of
		// how we exit this block!			
	}

	return bRet;	
}




template <typename T, int freeObjLimit>
T * StorageManager<T, freeObjLimit>::GetFreeObj(void)
{
	T							* newobj = NULL;


	ThreadGuard<ThreadCriticalSection>	mon(m_objLstCs);


	if ( m_pFreeObjLst == NULL )
	{
		newobj = new T;
		m_newObjNums++;
	}
	else
	{
		newobj          = m_pFreeObjLst;
		m_pFreeObjLst = newobj->next;
		m_freeObjNums --;
	}


	mon.Dismiss();

	if ( newobj )
		newobj->next = NULL;

	return newobj;
}




template <typename T, int freeObjLimit>
void StorageManager<T, freeObjLimit>::FreeObj(T * p_obj)
{
	ThreadGuard<ThreadCriticalSection>	mon(m_objLstCs);


	if ( m_freeObjNums > freeObjLimit )
	{
		delete p_obj;
		m_newObjNums --;
	}
	else
	{
		p_obj->next = m_pFreeObjLst;
		m_pFreeObjLst = p_obj;
		m_freeObjNums ++;
	}
}




template <typename T, int freeObjLimit>
void StorageManager<T, freeObjLimit>::DeleteFreeObj()
{
	T							* obj, * ptr;


	ThreadGuard<ThreadCriticalSection>	mon(m_objLstCs);

	for ( obj = m_pFreeObjLst; obj != NULL; obj = ptr )
	{
		ptr = obj->next;
		delete obj;
	}

	m_pFreeObjLst = NULL;

	mon.Dismiss();
}




template <typename T, int freeObjLimit>
int StorageManager<T, freeObjLimit>::GetObjsNumberofLink()
{
	int									num = 0;
	T *									obj;


	{
		// Constructor of <mon> automatically
		// acquires the <mutex>	
		ThreadGuard<ThreadCriticalSection> mon(m_linkCs);

		obj = m_link.head;
		while ( obj )
		{
			num ++;
			obj = obj->next;
		}

		// Destructor of Guard automatically
		// releases the lock, regardless of
		// how we exit this block!			
	}

	return num;
}




template <typename T, int freeObjLimit>
int StorageManager<T, freeObjLimit>::GetObjNums()
{
	return m_newObjNums;
}




#endif