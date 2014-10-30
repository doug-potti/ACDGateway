#include "AGService.h"
#include "sp_resource.h"
#include "DBHelper.h"
#include "CstaCommand.h"
#include "CstaEventProcess.h"
#include "CstaInterface.h"
#include "MonitorThread.h"
#include "CstaReconnect.h"
#include "AGHttpServer.h"


void AGService::OnBeforeStart()
{
	AGHelper::GetAcdgwCfgInfo();
}

BOOL AGService::OnStart()
{
	BOOL bRet = ServiceProcess_c::OnStart();
	if ( bRet == FALSE )
	{
		return FALSE;
	}

	if (gLogHelper== NULL)
	{
		gLogHelper = new AGLogHelper("log4cplus.properties");
	}

	if (gDBHelper == NULL)
	{
		gDBHelper = new DBHelper();
	}

	if (gCstaInterfaceThd == NULL)
	{
		gCstaInterfaceThd = CstaInterface::GetCstaInstance();
	}
	
	if (gCstaCmdThread == NULL)
	{
		gCstaCmdThread = new CstaCommand();
	}

	if (gCstaEventProcessThd == NULL)
	{
		gCstaEventProcessThd = new CstaEventProcess();
	}

	if (gMonitorThread == NULL)
	{
		gMonitorThread = new MonitorThread();
	}

	if (gAGHttpServer == NULL)
	{
		gAGHttpServer = new AGHttpServer(AGHelper::sAcdgwCfg.httpSvrIp, 
										 atoi(AGHelper::sAcdgwCfg.httpSvrPort.c_str()));
	}
	return TRUE;
}




void AGService::OnStop()
{
	ServiceProcess_c::OnStop();
}




void AGService::Main()
{
	OUTINFOLOG(AGHelper::sAcdgwCfg.ToString());

	if (gDBHelper->GetDevFromDB())
	{
		if (AGHelper::sDevList.size() == 0)
		{
			OUTINFOLOG("ACDGateway get device from db count is 0. so return.");
			return;
		}
	}
	else
	{
		OUTERRORLOG("ACDGateway get device from db failed.");
		return;
	}

	if ( !gCstaInterfaceThd->OpenAcsStream(AGHelper::sAcdgwCfg.tsSvrId,
										   AGHelper::sAcdgwCfg.tsLoginId,
										   AGHelper::sAcdgwCfg.tsPasswnd))
	{
		OUTERRORLOG("ACDGateway Csta Interface open acs stream failed.");
		return;
	}

	gCstaInterfaceThd->start();
	OUTINFOLOG("ACDGateway start csta event recive thread.");
	gCstaEventProcessThd->start();
	OUTINFOLOG("ACDGateway start csta event process thread.");
	gCstaCmdThread->start();
	OUTINFOLOG("ACDGateway start csta command recive&process thread.");
	gMonitorThread->start();
	OUTINFOLOG("ACDGateway start monitor device thread.");
	gAGHttpServer->start();
	OUTINFOLOG("ACDGateway start http server thread.");
	StratMonitorDevice();



}

void AGService::StratMonitorDevice()
{
	std::vector<Dev_t*>::iterator iter = AGHelper::sDevList.begin();
	for (; iter != AGHelper::sDevList.end(); ++iter)
	{
		if (gMonitorThread != NULL)
		{
			gMonitorThread->AddMonDevToMQ(*iter);
		}
	}
}


void AGService::Maintenance()
{

}




void AGService::SetAutoRunAsOSStartup(bool p_start)
{

}




HICON AGService::LoadAppIcon()
{
	return LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
}




HICON AGService::LoadStartIcon()
{
	return LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_START));
}




HICON AGService::LoadPauseIcon()
{
	return LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_PAUSE));
}




HICON AGService::LoadStopIcon()
{
	return LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_STOP));
}




void AGService::OnRunType(ServiceCommands_t p_type)
{
	if ( p_type == SvcCmdTray )
	{
	}
}




void AGService::CreateConfigWin(HINSTANCE p_hInstance, HWND p_hWnd)
{
// 	DialogBox(hInstance,
// 			 MAKEINTRESOURCE(IDD_CFGWIN),
// 			 p_hWnd,
// 			 StaticCfgWndProc);
}

INT_PTR CALLBACK AGService::StaticCfgWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool								bAuto = false;
	char								szWinText[MAX_PATH];


	switch ( msg )
	{
	case WM_INITDIALOG:
		MoveWindow(hWnd, 300, 200, 425, 350, TRUE);
		return TRUE;

	case WM_CLOSE:
		break;

	case WM_DRAWITEM: 
		return TRUE;

	case WM_COMMAND:
		switch ( wParam )
		{
		case IDOK:
			EndDialog(hWnd, IDOK);
			return TRUE;

		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			return TRUE;
		}
		switch( HIWORD(wParam) )
		{
		case BN_CLICKED:
			UINT								flag;
			return TRUE;

		default:
			return FALSE;
		}
		break;
	}
	return FALSE;
}

FDECLARE_MAINFRAME(AGService,
				   ACDGW_MAJOR_VERSION,
				   ACDGW_MINOR_VERSION,
				   ACDGW_MANUFACTURER,
				   ACDGW_PRODUCT_DESCRIPTION,
				   ACDGW_PRODUCT_NAME,
				   ACDGW_SERVICE_NAME,
				   ACDGW_SERVICE_DESCRIPTION)