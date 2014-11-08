#ifndef ACDGW_SERVICE_H
#define ACDGW_SERVICE_H

#include <queue>
#include "svcproc.hpp"
#include "AGTask.h"
#include <IceUtil/IceUtil.h>

typedef std::queue<AGTask *> AGTASKLIST;

class AGService;
extern AGService *gAGService;
class AGService:public ServiceProcess_c
{
public:

	AGService(WORD majorVersion,
			  WORD minorVersion,
			  const char * manuf,
			  const char * prod_desp,
			  const char * prod_name,
			  const char * svc_desp,
			  const char * svc_name)
		: ServiceProcess_c(majorVersion, minorVersion, manuf, prod_desp, prod_name, svc_desp, svc_name)
	{
		isDone = false;
		gAGService = this;
	}
	~AGService(){}

	void Main();
	void Maintenance(){}
	BOOL OnStart();
	void OnStop();
	void OnBeforeStart();
	void SetAutoRunAsOSStartup(bool p_start){}
	HICON LoadAppIcon();
	HICON LoadStartIcon();
	HICON LoadPauseIcon();
	HICON LoadStopIcon();
	void OnRunType(ServiceCommands_t p_type){}
	void CreateConfigWin(HINSTANCE p_hInstance, HWND p_hWnd){}

	void AddTaskToTL(AGTask *agTask);
	std::string GetDialNo(std::string medId, std::string medType, std::string cystLvl);
private:
	void StratMonitorDevice(); 
	
private:
	AGTASKLIST                           taskList;
	IceUtil::Monitor<IceUtil::RecMutex>  tlMutex;
	bool                                 isDone;
};

const unsigned int ACDGW_MAJOR_VERSION						= 1;
const unsigned int ACDGW_MINOR_VERSION						= 0;
const char ACDGW_MANUFACTURER[]							    = "Hcm Co., Ltd.";
const char ACDGW_PRODUCT_DESCRIPTION[]					    = "ACDGateway Service";
const char ACDGW_PRODUCT_NAME[]							    = "ACDGateway";
const char ACDGW_SERVICE_NAME[]							    = "ACDGateway";
const char ACDGW_SERVICE_DESCRIPTION[]					    = "ACDGateway Service 1.0.00";

#endif