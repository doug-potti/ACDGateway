#ifndef ACDGW_EXCUTE_MAKECALL_THREAD_H
#define ACDGW_EXCUTE_MAKECALL_THREAD_H

#include <list>
#include <IceUtil/IceUtil.h>
#include "AGTask.h"

typedef std::list<AGTask *> AGMCTASKLIST;

class AGExcuteMakeCallThd:public IceUtil::Thread
{
public:
	AGExcuteMakeCallThd():isDone(false)
	{
		waitIdleDevHandle = CreateEvent(NULL,FALSE,FALSE,NULL);
	}
	~AGExcuteMakeCallThd(){}
	void AddAGMakeCallTaskToATQ(AGTask *pTask);
	void RemoveMakeCallTask(std::string taskId, EnTaskType taskType);
	void SetIdleDevHandle()
	{
		SetEvent(waitIdleDevHandle);
	}
private:
	AGTask *FindAgTask()
	{
		AGTask *pTask = NULL;
		AGMCTASKLIST::iterator iter = agmcTaskList.begin();
		if (iter != agmcTaskList.end())
		{
			pTask = (*iter);
			agmcTaskList.pop_front();
		}
		return pTask;
	}
	void run();
	AGMCTASKLIST                             agmcTaskList;
	IceUtil::Monitor<IceUtil::RecMutex>      atqMutex;
	bool                                     isDone;
	HANDLE                                   waitIdleDevHandle;
};

typedef IceUtil::Handle<AGExcuteMakeCallThd>       AGExcuteMakeCallThdPtr;
extern  AGExcuteMakeCallThdPtr                     gAGExcuteMakeCall;



#endif