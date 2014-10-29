#ifndef ACDGW_SERVICE_H
#define ACDGW_SERVICE_H

#include "svcproc.hpp"

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

	}
	~AGService(){}

	void Main();
	void Maintenance();
	BOOL OnStart();
	void OnStop();

	void OnBeforeStart();
	void SetAutoRunAsOSStartup(bool p_start);
	HICON LoadAppIcon();
	HICON LoadStartIcon();
	HICON LoadPauseIcon();
	HICON LoadStopIcon();
	void OnRunType(ServiceCommands_t p_type);
	void CreateConfigWin(HINSTANCE p_hInstance, HWND p_hWnd);
	static INT_PTR CALLBACK StaticCfgWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticTSvrLstWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void StratMonitorDevice();  
};

const unsigned int ACDGW_MAJOR_VERSION						= 1;
const unsigned int ACDGW_MINOR_VERSION						= 0;
const char ACDGW_MANUFACTURER[]							    = "Hcm Co., Ltd.";
const char ACDGW_PRODUCT_DESCRIPTION[]					    = "ACDGateway Service";
const char ACDGW_PRODUCT_NAME[]							    = "ACDGateway";
const char ACDGW_SERVICE_NAME[]							    = "ACDGateway";
const char ACDGW_SERVICE_DESCRIPTION[]					    = "ACDGateway Service 1.0.00";

#endif