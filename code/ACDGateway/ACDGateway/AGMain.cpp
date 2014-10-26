#include "AGService.h"
#include "sp_resource.h"

void AGService::OnBeforeStart()
{

}

BOOL AGService::OnStart()
{
	BOOL bRet = ServiceProcess_c::OnStart();
	if ( bRet == FALSE )
	{
		return FALSE;
	}

	//Start


	return TRUE;
}




void AGService::OnStop()
{
	ServiceProcess_c::OnStop();
}




void AGService::Main()
{
	//
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