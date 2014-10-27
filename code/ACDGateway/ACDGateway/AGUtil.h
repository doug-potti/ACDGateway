#ifndef ACDGW_UTIL_H
#define ACDGW_UTIL_H

#include <sstream>
#include <IceUtil/IceUtil.h>
#include "acs.h"
#include "attpriv.h"
#include "csta.h"
#include "AGLogHelper.h"

#define OUTTRACELOG(logmsg) \
	if(gLogHelper != NULL)\
	{\
		gLogHelper->WriteTrace(logmsg);\
	}

#define OUTDEBUGLOG(logmsg) \
	if(gLogHelper != NULL)\
	{\
		gLogHelper->WriteDebug(logmsg);\
	}

#define OUTINFOLOG(logmsg) \
	if(gLogHelper != NULL)\
	{\
		gLogHelper->WriteInfo(logmsg);\
	}

#define OUTWARNLOG(logmsg) \
	if(gLogHelper != NULL)\
	{\
		gLogHelper->WriteWarn(logmsg);\
	}

#define OUTERRORLOG(logmsg) \
	if(gLogHelper != NULL)\
	{\
		gLogHelper->WriteError(logmsg);\
	}

#define OUTFATALLOG(logmsg) \
	if(gLogHelper != NULL)\
	{\
		gLogHelper->WriteFatal(logmsg);\
	}

enum EnTsapiCmdType
{
	MonitorDevice,
	MonitorViaDevice,
	MakeCall,
};

const int PBXEVENT_SIZE = (sizeof(CSTAEvent_t) + 1);

typedef IceUtil::Monitor<IceUtil::RecMutex> ICERECMUTEX;

struct AvayaPbxEvent_t
{
	char                                cstaEvent[PBXEVENT_SIZE];
	int									privateDataLength;
	ATTEvent_t							attEvent;
};

struct TsapiCommand_t
{
	EnTsapiCmdType                      tsapiCmdType;
	std::string                         activeDevId;
	std::string                         agentId;
	long                                activeCallId;
	long                                heldCallId;
	std::string                         heldDevId;
	std::string                         uui;
	std::string                         destNo;
	unsigned long                       invokeId;
};



class AGHelper
{
public:
	AGHelper(){}
	~AGHelper(){}

	static unsigned long GenerateInvokeId(std::string identId, int requestType);
	static int AnalyzeInvokeId(unsigned long invokeId, std::string &identId);

private:
	static std::string ConvertULToString(unsigned long ulInput);
private:
};

#endif