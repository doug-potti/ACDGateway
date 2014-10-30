#ifndef ACDGW_UTIL_H
#define ACDGW_UTIL_H

#include <sstream>
#include <vector>
#include <map>
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

enum EnDevType
{
	Device,
	Split,
	Vdn,
};

const int PBXEVENT_SIZE = (sizeof(CSTAEvent_t) + 1);

typedef IceUtil::Monitor<IceUtil::RecMutex> ICERECMUTEX;

struct Dev_t
{
	std::string devId;
	EnDevType   devType;
	long        monRefId;

	Dev_t()
	{
		monRefId = -1;
	}
};

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

struct AcdgwCfg_t
{
	std::string                         tsSvrId;
	std::string                         tsLoginId;
	std::string                         tsPasswnd;
	std::string                         dbType;
	std::string                         dbIp;
	std::string                         dbName;
	std::string                         dbUser;
	std::string                         dbPasswnd;
	std::string                         httpSvrIp;
	std::string                         httpSvrPort;

	std::string   ToString()
	{
		std::string strCfg;
		strCfg.append("TSlinkName:");
		strCfg.append(tsSvrId);
		strCfg.append("\r\n\t\t");
		strCfg.append("TSLoginId:");
		strCfg.append(tsLoginId);
		strCfg.append("\r\n\t\t");
		strCfg.append("TSPasswnd:");
		strCfg.append(tsPasswnd);
		strCfg.append("\r\n\t\t");
		strCfg.append("DBType:");
		strCfg.append(dbType);
		strCfg.append("\r\n\t\t");
		strCfg.append("DBIp:");
		strCfg.append(dbIp);
		strCfg.append("\r\n\t\t");
		strCfg.append("DBName:");
		strCfg.append(dbName);
		strCfg.append("\r\n\t\t");
		strCfg.append("DBUser:");
		strCfg.append(dbUser);
		strCfg.append("\r\n\t\t");
		strCfg.append("HttpSvtIp:");
		strCfg.append(httpSvrIp);
		strCfg.append("\r\n\t\t");
		strCfg.append("HttpSvrPort:");
		strCfg.append(httpSvrPort);
		return strCfg;
	}
};


typedef std::vector<Dev_t *>  DEVLIST;
typedef std::map<long, Dev_t*> MONDEVMAP;
class AGHelper
{
public:
	AGHelper(){}
	~AGHelper(){}

	static std::string   ConvertTaspiCmdToString(EnTsapiCmdType cmdType);
	static unsigned long GenerateInvokeId(std::string identId, int requestType);
	static int           AnalyzeInvokeId(unsigned long invokeId, std::string &identId);
	static void          GetAcdgwCfgInfo();

	static void          AddMonDevToMDM(long refId, Dev_t *monDev);
	static Dev_t*        FindDevByRefId(long refId);
	static void          RemoveMonDevByRefId(long refId);

private:
	static std::string   ConvertULToString(unsigned long ulInput);
public:
	static AcdgwCfg_t    sAcdgwCfg;
	static DEVLIST       sDevList;
private:

	static MONDEVMAP     monDevMap;
	static ICERECMUTEX   mdmMutex;
};

#endif