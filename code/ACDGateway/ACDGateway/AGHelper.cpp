
#include "AGUtil.h"

AcdgwCfg_t                                AGHelper::sAcdgwCfg;
DEVLIST                                   AGHelper::sDevList;
MONDEVMAP                                 AGHelper::monDevMap;
ICERECMUTEX                               AGHelper::mdmMutex;

std::string AGHelper::ConvertTaspiCmdToString(EnTsapiCmdType cmdType)
{
	switch(cmdType)
	{
	case MakeCall:
		return "MakeCall";
	case MonitorDevice:
		return "MonitorDevice";
	case MonitorViaDevice:
		return "MonitorViaDevice";
	default:
		return "unkown taspi cmd type";
	}
}

unsigned long AGHelper::GenerateInvokeId(std::string identId, int requestType)
{
	unsigned long invokeid = strtoul(identId.c_str(),NULL,10);
	invokeid  = invokeid << 8;
	invokeid |= requestType;
	return invokeid;
}

int AGHelper::AnalyzeInvokeId(unsigned long invokeId, std::string &identId)
{
	int tempType = 0;
	tempType &= 0xFF;
	tempType  = invokeId & 0xFF;
	unsigned long deviceid = invokeId >> 8;
	identId = ConvertULToString(deviceid);
	return tempType;
}

std::string AGHelper::ConvertULToString(unsigned long ulInput)
{
	std::stringstream ss("");
	ss << ulInput;
	std::string strResult;
	ss >> strResult;
	ss.str("");
	return strResult;
}

void AGHelper::GetAcdgwCfgInfo()
{
	char								buf[MAX_PATH], bufTemp[MAX_PATH];
	char								* pPath;

	GetModuleFileName(NULL, buf, MAX_PATH);
	pPath = strrchr(buf, '\\');
	if ( pPath )
	{
		*pPath = '\0';
	}
	strcat(buf, "\\acdgwcfg");
	
	GetPrivateProfileString("TSAPI", "TServerName", "default", bufTemp, 64, buf);
	sAcdgwCfg.tsSvrId = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("TSAPI", "LoginID", "administrator", bufTemp, 64, buf);
	sAcdgwCfg.tsLoginId = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("TSAPI", "Password", "administrator", bufTemp, 64, buf);
	sAcdgwCfg.tsPasswnd = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("DB", "DBType", "MSSQL", bufTemp, 64, buf);
	sAcdgwCfg.dbType = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("DB", "DBIp", "127.0.0.1", bufTemp, 64, buf);
	sAcdgwCfg.dbIp = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("DB", "DBName", "default", bufTemp, 64, buf);
	sAcdgwCfg.dbName = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("DB", "DBUser", "default", bufTemp, 64, buf);
	sAcdgwCfg.dbUser = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("DB", "DBPwd", "default", bufTemp, 64, buf);
	sAcdgwCfg.dbPasswnd = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("HTTP", "HttpServerIp", "127.0.0.1", bufTemp, 64, buf);
	sAcdgwCfg.httpSvrIp = bufTemp;
	memset(bufTemp, 0, 64);
	GetPrivateProfileString("HTTP", "HttpServerPort", "8081", bufTemp, 64, buf);
	sAcdgwCfg.httpSvrPort = bufTemp;
	memset(bufTemp, 0, 64);
}

void AGHelper::AddMonDevToMDM(long refId, Dev_t *monDev)
{
	ICERECMUTEX::Lock lock(mdmMutex);
	monDevMap.insert(std::pair<long, Dev_t*>(refId, monDev));
}

Dev_t* AGHelper::FindDevByRefId(long refId)
{
	ICERECMUTEX::Lock lock(mdmMutex);
	MONDEVMAP::iterator findIter;
	findIter = monDevMap.find(refId);
	if (findIter != monDevMap.end())
	{
		return (*findIter).second;
	}
	return NULL;
}

void AGHelper::RemoveMonDevByRefId(long refId)
{
	ICERECMUTEX::Lock lock(mdmMutex);
	MONDEVMAP::iterator findIter;
	findIter = monDevMap.find(refId);
	if (findIter != monDevMap.end())
	{
		monDevMap.erase(findIter);
	}
}

