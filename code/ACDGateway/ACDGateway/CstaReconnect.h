#ifndef ACDGW_CSTA_RECONNECT_H
#define ACDGW_CSTA_RECONNECT_H

#include "AGUtil.h"

class CstaReconnectThd:public IceUtil::Thread
{
public:
	CstaReconnectThd():isDone(false), isReconnecting(false)
	{
		reconnectHandle = CreateEvent(NULL,FALSE,FALSE, NULL);
	}
	~CstaReconnectThd(){}
	void StartCstaReconnect();
	void Stop();
private:
	void run();
	bool ReconnectTs();
private:
	bool   isDone;
	bool   isReconnecting;
	HANDLE reconnectHandle;
};

typedef IceUtil::Handle<CstaReconnectThd> CstaReconnectThreadPtr;
extern CstaReconnectThreadPtr gCstaReconnectThd;



#endif