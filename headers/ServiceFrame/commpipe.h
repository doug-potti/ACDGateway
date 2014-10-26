#ifndef ABU_COMMONPIPE_20040523__H
#define ABU_COMMONPIPE_20040523__H




#include <thread_cs.h>
#include <thread_guard.h>




struct commlist
{
	void * head;
	void * tail;
};




template <typename T>
class CommPipe_c
{
private:
	Thread_Mutex						_mutex;
	commlist							_list;

public:
	CommPipe_c()
	{
		_list.head = NULL;
		_list.tail = NULL;
	}


	~CommPipe_c()
	{
		T *									delData;


		/* check all left data which is not retrieved */
		while ( _list.head )
		{
			// Constructor of <mon> automatically
			// acquires the <mutex>	
			Guard<Thread_Mutex> mon(_mutex);
			
			delData = (T *)_list.head;
			_list.head = (void *)((T *)_list.head)->next;
			if ( _list.head == NULL )
				_list.tail = NULL;
				
			mon.Dismiss();
			
			if ( delData )
			{
				delete delData;
				delData = NULL;
			}

			// Destructor of Guard automatically
			// releases the lock, regardless of
			// how we exit this block!		
		}
	}


	bool WINAPI Pop(T **Buf)
	{
		bool bRet = true;

		*Buf = NULL;

		try
		{
			// Constructor of <mon> automatically
			// acquires the <mutex>	
			Guard<Thread_Mutex> mon(_mutex);

			if ( _list.head == NULL )
			{
				bRet = false;
			}
			else
			{
				*Buf = (T *)_list.head;
				_list.head = _list.head->next;
				if ( _list.head == NULL )
					_list.tail = NULL;
			}

			// Destructor of Guard automatically
			// releases the lock, regardless of
			// how we exit this block!
		}
		catch(...)
		{
			bRet = false;
		}
		
		return bRet;
	}


	bool WINAPI Push(T *Buf)
	{
		bool bRet = true;

		try
		{
			// Constructor of <mon> automatically
			// acquires the <mutex>	
			Guard<Thread_Mutex> mon(_mutex);

			if ( _list.head == NULL )
			{
				_list.head = (void *)Buf;
				_list.tail = (void *)Buf;
			}
			else
			{
				_list.tail->next = (void *)Buf;
				_list.tail = (void *)Buf;
			}

			// Destructor of Guard automatically
			// releases the lock, regardless of
			// how we exit this block!			
		}
		catch(...)
		{
			bRet = false;
		}

		return bRet;
	}


	virtual void ReleasePipeCell(T * p_data) = 0;


	virtual bool NewPipeCell(T ** p_newData) = 0;


	virtual bool DeletePipeCell(T * p_newData) = 0;
};




#endif