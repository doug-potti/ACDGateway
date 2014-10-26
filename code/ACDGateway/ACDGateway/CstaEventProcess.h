#ifndef ACDGW_CSTA_EVENT_PROCESS_H
#define ACDGW_CSTA_EVENT_PROCESS_H

#include <queue>
#include "AGUtil.h"
#include <IceUtil/IceUtil.h>

typedef std::queue<AvayaPbxEvent_t *> PbxEventQueue;

class CstaEventProcess:public IceUtil::Thread
{
public:
	CstaEventProcess():isDone(false)
	{

	}
	~CstaEventProcess(){}
	void AddPbxEventToQueue(AvayaPbxEvent_t *pbxEvent);
private:
	void run();
	void ProcessAvayaPbxEvent(AvayaPbxEvent_t *pbxEventData);
	void ProcessACSUnsolicited(AvayaPbxEvent_t * pbxEventData);
	void ProcessACSConfirmation(AvayaPbxEvent_t * pbxEventData);
	void ProcessCSTAConfirmation(AvayaPbxEvent_t * pbxEventData);
	void ProcessCSTAUnsolicited(AvayaPbxEvent_t * pbxEventData);
	void ProcessCSTAEventReport(AvayaPbxEvent_t * pbxEventData);
	void WriteCSTAUnsolicited(AvayaPbxEvent_t *pbxEventData);

private:
	bool                                     isDone;
	PbxEventQueue                            pbxEventQueue;
	static char                              arrErrMsg[]; 
	IceUtil::Monitor<IceUtil::RecMutex>      mutexQueue;
#define CheckEventData(eventdata) \
	if( eventdata == NULL)\
	{\
		return;\
	};

};

typedef IceUtil::Handle<CstaEventProcess> CstaEventProcessPtr;
extern CstaEventProcessPtr gCstaEventProcessThd;

#endif