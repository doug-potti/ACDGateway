#ifndef ACDGW_MONITOR_THREAD_H
#define ACDGW_MONITOR_THREAD_H

#include <queue>
#include "AGUtil.h"

typedef std::queue<Dev_t *>             MONQUEUE;


class MonitorThread:public IceUtil::Thread
{
public:
	MonitorThread():isDone(false)
	{}
	~MonitorThread(){}
	void Stop();
	void AddMonDevToMQ(Dev_t *pDev);
private:
	void run();
private:
	bool                                    isDone;
	MONQUEUE                                monQueue;
	ICERECMUTEX                             mqRecMutex;
};

typedef IceUtil::Handle<MonitorThread>      MonitorThdPtr;
extern  MonitorThdPtr                       gMonitorThread;

#endif