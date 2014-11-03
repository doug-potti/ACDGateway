
#include "AGUtil.h"
#include "AGService.h"

AcdgwCfg_t                                AGHelper::sAcdgwCfg;
DEVLIST                                   AGHelper::sDevList;
MONDEVMAP                                 AGHelper::monDevMap;
ICERECMUTEX                               AGHelper::mdmMutex;
DIALMAP                                   AGHelper::dialMap;
TASKDEVLIST                               AGHelper::taskDevList;
ICERECMUTEX                               AGHelper::tdlMutex;
TASKLIST                                  AGHelper::taskList;
ICERECMUTEX                               AGHelper::tlMutex;
LOGONMAP                                  AGHelper::logonMap;
ICERECMUTEX                               AGHelper::lmMutex;

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

void AGHelper::AddDialMapToDM(std::string business, std::string dialNo)
{
	dialMap.insert(std::pair<std::string, std::string>(business, dialNo));
}

std::string AGHelper::FindDialNoByBusiness(std::string business)
{
	DIALMAP::iterator findIter = dialMap.find(business);
	if (findIter != dialMap.end())
	{
		return (*findIter).second;
	}

	return "";
}

void AGHelper::AddTaskDevToTDL(TaskDev_t *taskDev)
{
	ICERECMUTEX::Lock lock(tdlMutex);
	taskDevList.push_back(taskDev);
}

TaskDev_t *AGHelper::FindIdleTaskDev()
{
	ICERECMUTEX::Lock lock(tdlMutex);
	TASKDEVLIST::iterator iter = taskDevList.begin();
	for (; iter != taskDevList.end(); ++iter)
	{
		if ((*iter)->isIdle)
		{
			(*iter)->isIdle = false;

			return *iter;
		}
	}
	return NULL;
}

void AGHelper::SetIdleTaskDev(std::string devId)
{
	ICERECMUTEX::Lock lock(tdlMutex);
	TASKDEVLIST::iterator iter = taskDevList.begin();
	for (; iter != taskDevList.end(); ++iter)
	{
		if ((*iter)->taskDevId == devId && !((*iter)->isIdle))
		{
			(*iter)->isIdle = true;
			if (gAGService != NULL)
			{
				gAGService->SetIdleDevHandle();
			}
		}
	}
}

void AGHelper::AddTaskToTL(AGTask *agTask)
{
	ICERECMUTEX::Lock lock(tlMutex);
	taskList.push_back(agTask);
}

bool AGHelper::IsExistTask(std::string taskId, EnTaskType taskType)
{
	ICERECMUTEX::Lock lock(tlMutex);
	TASKLIST::iterator iter = taskList.begin();
	for (; iter != taskList.end(); ++iter)
	{
		if (((*iter)->GetTaskId() == taskId) &&
			((*iter)->GetTaskType() == taskType))
		{
			return true;
		}
	}

	return false;
}

AGTask* AGHelper::FindTaskByTaskDevRefId(long monRefId)
{
	ICERECMUTEX::Lock lock(tlMutex);
	TASKLIST::iterator iter = taskList.begin();
	for (; iter != taskList.end(); ++iter)
	{
		if ((*iter)->GetTaskRefId() == monRefId)
		{
			return *iter;
		}
	}
	return NULL;
}

AGTask *AGHelper::FindTaskByTaskId(std::string taskId)
{
	ICERECMUTEX::Lock lock(tlMutex);
	TASKLIST::iterator iter = taskList.begin();
	for (; iter != taskList.end(); ++iter)
	{
		if ((*iter)->GetTaskId() == taskId)
		{
			return *iter;
		}
	}
	return NULL;
}

AGTask *AGHelper::FindTaskByDevId(std::string devId)
{
	ICERECMUTEX::Lock lock(tlMutex);
	TASKLIST::iterator iter = taskList.begin();
	for (; iter != taskList.end(); ++iter)
	{
		if ((*iter)->GetTaskDevId() == devId)
		{
			return *iter;
		}
	}
	return NULL;
}


void AGHelper::RemoveTaskByRefId(long refId)
{
	ICERECMUTEX::Lock lock(tlMutex);
	TASKLIST::iterator iter = taskList.begin();
	for (; iter != taskList.end(); ++iter)
	{
		if ((*iter)->GetTaskRefId() == refId)
		{
			AGTask *tempTask = (*iter);
			taskList.erase(iter);
			delete tempTask;
			tempTask = NULL;
			return;
		}
	}
}

void AGHelper::RemoveTaskByTaskId(std::string taskId)
{
	ICERECMUTEX::Lock lock(tlMutex);
	TASKLIST::iterator iter = taskList.begin();
	for (; iter != taskList.end(); ++iter)
	{
		if ((*iter)->GetTaskId() == taskId)
		{
			AGTask *tempTask = (*iter);
			taskList.erase(iter);
			delete tempTask;
			tempTask = NULL;
			return;
		}
	}
}

void AGHelper::RemoveTaskByDevId(std::string devId)
{
	ICERECMUTEX::Lock lock(tlMutex);
	TASKLIST::iterator iter = taskList.begin();
	for (; iter != taskList.end(); ++iter)
	{
		if ((*iter)->GetTaskDevId() == devId)
		{
			AGTask *tempTask = (*iter);
			taskList.erase(iter);
			delete tempTask;
			tempTask = NULL;
			return;
		}
	}
}

void AGHelper::AddTerAgtToLM(std::string logonTerId, std::string agentId)
{
	ICERECMUTEX::Lock lock(lmMutex);
	logonMap.insert(std::pair<std::string, std::string>(logonTerId, agentId));
}

std::string AGHelper::FindAgentIdByTerId(std::string terId)
{
	ICERECMUTEX::Lock lock(lmMutex);
	LOGONMAP::iterator findIter;
	findIter = logonMap.find(terId);
	if (findIter != logonMap.end())
	{
		return (*findIter).second;
	}
	return "NOFIND";
}

void AGHelper::RemoveTerAgtFromLM(std::string terId)
{
	ICERECMUTEX::Lock lock(lmMutex);
	LOGONMAP::iterator findIter;
	findIter = logonMap.find(terId);
	if (findIter != logonMap.end())
	{
		logonMap.erase(findIter);
	}
}
