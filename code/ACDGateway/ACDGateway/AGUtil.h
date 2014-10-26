#ifndef ACDGW_UTIL_H
#define ACDGW_UTIL_H

#include <sstream>
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

const int PBXEVENT_SIZE = (sizeof(CSTAEvent_t) + 1);

struct AvayaPbxEvent_t
{
	char                                cstaEvent[PBXEVENT_SIZE];
	int									privateDataLength;
	ATTEvent_t							attEvent;
};

#endif