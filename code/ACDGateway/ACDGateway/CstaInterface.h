#ifndef ACDGW_CSTA_INTERFACE_H
#define ACDGW_CSTA_INTERFACE_H

#include <IceUtil/IceUtil.h>
#include "AGUtil.h"

class CstaInterface:public IceUtil::Thread
{
private:
	CstaInterface();
	~CstaInterface();
	void run();
	bool GetEventBlock();
	bool GetEventPoll();
	bool SysStatusStart();
	bool SysStatusStop();
public:
	static CstaInterface *GetCstaInstance();
	void stoprun();	
	bool OpenAcsStream(std::string acsServerName, std::string acsUser, std::string acsPassword);
	bool CloseAcsStream();
	bool MonitorDevice(std::string devId);
	bool MonitorViaDevice(std::string viaDevId);
	bool StopMonitorDevice(long monitorRefId, std::string devId);
	bool QueryDeviceInfo(std::string  deviceId);
	bool CallMake(std::string devId, std::string destNo, std::string userData,unsigned long invokeId);

private:
	static CstaInterface*						cstaInterfaceSelf;
	static char									arrErrorMsg[];
	ACSHandle_t									acsHandle;
	ATTPrivateData_t							privateData;
	ATTEvent_t									attEvent;
	RetCode_t									retCode;
	CSTAEvent_t									cstaEvent;
	unsigned short								usEventBufSize;
	unsigned short								usNumEvents;
	bool										isDone;
	bool										isRun;
	IceUtil::Monitor<IceUtil::RecMutex>			runMutexEvent;
	static char									arrPbxEvent[PBXEVENT_SIZE];
	HANDLE										pauseHandler;
	bool										isPause;

#define CheckAcsHandle \
	if( acsHandle == NULL)\
	{\
		return false;\
	};
};

typedef IceUtil::Handle<CstaInterface> CstaInterfacePtr;
extern CstaInterfacePtr gCstaInterfaceThd;

#endif