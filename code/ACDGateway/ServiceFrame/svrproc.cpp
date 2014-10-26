/****************************************************************
 *																*
 * svcproc.cpp													*
 *																*
 * Copyright (c) 1993-2003 Finetel Telecommunication			*
 * Equipment. Ltd.												*
 *																*
 ****************************************************************/




#include <svcproc.hpp>




using namespace std;




#define UWM_SYSTRAY												(WM_USER + 1)
#define SYSTRAY_ICON_ID											1


#define DATE_WIDTH												72
#define THREAD_WIDTH											80
#define LEVEL_WIDTH												32
#define PROTO_WIDTH												40
#define ACTION_WIDTH											48




static const char *const ServiceCommandNames[NumSvcCmds] =
{
	"Tray",
	"NoTray",
	"Version",
	"Install",
	"Remove",
	"Start",
	"Stop",
	"Pause",
	"Resume",
	"Deinstall",
	"NoWin"
};


HINSTANCE								ServiceProcess_c::hInstance;


static ServiceProcess_c *				PProcessInstance;
char **									ServiceProcess_c::p_argv;
int										ServiceProcess_c::p_argc;




class ServiceProcess_c;




/****************************************************************
 *																*
 *					CLASS OF ServiceManager_c					*
 *																*
 ****************************************************************/
class ServiceManager_c
{
public:
	ServiceManager_c(){ error = 0; }

	virtual BOOL Create(ServiceProcess_c * svc) = 0;
	virtual BOOL Delete(ServiceProcess_c * svc) = 0;
	virtual BOOL Start(ServiceProcess_c * svc) = 0;
	virtual BOOL Stop(ServiceProcess_c * svc) = 0;
	virtual BOOL Pause(ServiceProcess_c * svc) = 0;
	virtual BOOL Resume(ServiceProcess_c * svc) = 0;

	DWORD GetError() const { return error; }


protected:
	DWORD								error;
};




class NT_ServiceManager_c : public ServiceManager_c
{
public:
    NT_ServiceManager_c()  { schSCManager = schService = NULL; }
    ~NT_ServiceManager_c();

    BOOL Create(ServiceProcess_c *svc);
    BOOL Delete(ServiceProcess_c *svc);
    BOOL Start(ServiceProcess_c *svc);
    BOOL Stop(ServiceProcess_c *svc) { return Control(svc, SERVICE_CONTROL_STOP); }
    BOOL Pause(ServiceProcess_c *svc) { return Control(svc, SERVICE_CONTROL_PAUSE); }
    BOOL Resume(ServiceProcess_c *svc) { return Control(svc, SERVICE_CONTROL_CONTINUE); }
	BOOL Query(ServiceProcess_c *svr, DWORD &p_state);
    DWORD GetError() const { return error; }


private:
    BOOL OpenManager();
    BOOL Open(ServiceProcess_c *svc);
    BOOL Control(ServiceProcess_c *svc, DWORD command);

    SC_HANDLE							schSCManager;
	SC_HANDLE							schService;
};




NT_ServiceManager_c::~NT_ServiceManager_c()
{
	if ( schService != NULL )
		CloseServiceHandle(schService);

	if ( schSCManager != NULL )
		CloseServiceHandle(schSCManager);
}




BOOL NT_ServiceManager_c::OpenManager()
{
	if ( schSCManager != NULL )
		return TRUE;
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( schSCManager != NULL )
		return TRUE;

	error = ::GetLastError();
	SPLOG(IFINE_SYSLOG::Info) << "Could not open Service Manager." << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	return FALSE;
}




BOOL NT_ServiceManager_c::Open(ServiceProcess_c * svc)
{
	if ( !OpenManager() )
		return FALSE;

	if ( schService != NULL )
		return TRUE;
	schService = OpenService(schSCManager, svc->GetSvcName(), SERVICE_ALL_ACCESS);
	if ( schService != NULL )
		return TRUE;

	error = ::GetLastError();
	SPLOG(IFINE_SYSLOG::Info) << "Service is not installed." << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	return FALSE;
}




BOOL NT_ServiceManager_c::Create(ServiceProcess_c * svc)
{
	if ( !OpenManager() )
		return FALSE;

	schService = OpenService(schSCManager, svc->GetSvcName(), SERVICE_ALL_ACCESS);
	if ( schService != NULL )
	{
		SPLOG(IFINE_SYSLOG::Info) << "Service is already installed." << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		return FALSE;
	}

	string								binaryPath = "\"";

	binaryPath += svc->GetFile();
	binaryPath += "\"";
	SPLOG(IFINE_SYSLOG::Info) << binaryPath.c_str() << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	schService = CreateService(schSCManager,                   // SCManager database
								svc->GetSvcName(),                 // name of service
								svc->GetSvcDesp(),                 // name to display
								SERVICE_ALL_ACCESS,             // desired access
								SERVICE_WIN32_OWN_PROCESS,      // service type
								SERVICE_AUTO_START,             // start type
								SERVICE_ERROR_NORMAL,           // error control type
								binaryPath.c_str(),                 // service's binary
								NULL,                           // no load ordering group
								NULL,                           // no tag identifier
								svc->GetServiceDependencies(),  // no dependencies
								NULL,                           // LocalSystem account
								NULL);                          // no password
	if ( schService == NULL )
	{
		error = ::GetLastError();
		return FALSE;
	}


	HKEY								key;
	std::string							b;


	b = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";
	b += svc->GetSvcName();
	if ( (error = RegCreateKey(HKEY_LOCAL_MACHINE, b.c_str(), &key)) != ERROR_SUCCESS )
		return FALSE;


	unsigned char * fn = (unsigned char *)(const char *)svc->GetFile();
	int fnlen = strlen(svc->GetFile());
	if ( (error = RegSetValueEx(key, "EventMessageFile", 0, REG_EXPAND_SZ, fn, fnlen)) == ERROR_SUCCESS &&
			(error = RegSetValueEx(key, "CategoryMessageFile", 0, REG_EXPAND_SZ, fn, fnlen)) == ERROR_SUCCESS )
	{
		DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
		if ( (error = RegSetValueEx(key,
									"TypesSupported",
									0,
									REG_DWORD,
									(LPBYTE)&dwData,
									sizeof(DWORD))) == ERROR_SUCCESS )
		{
			dwData = IFINE_SYSLOG::NumLogLevels;
			error = RegSetValueEx(key,
									"CategoryCount",
									0,
									REG_DWORD,
									(LPBYTE)&dwData,
									sizeof(DWORD));
		}
	}

	RegCloseKey(key);

	return error == ERROR_SUCCESS;
}




BOOL NT_ServiceManager_c::Delete(ServiceProcess_c * svc)
{
	if ( !Open(svc) )
		return FALSE;

	std::string name = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";
	name += svc->GetSvcName();
	error = ::RegDeleteKey(HKEY_LOCAL_MACHINE, (char *)(const char *)name.c_str());

	if ( !::DeleteService(schService) )
		error = ::GetLastError();

	return error == ERROR_SUCCESS;
}




BOOL NT_ServiceManager_c::Start(ServiceProcess_c * svc)
{
	if ( !Open(svc) )
		return FALSE;

	BOOL ok = ::StartService(schService, 0, NULL);
	error = ::GetLastError();
	return ok;
}




BOOL NT_ServiceManager_c::Control(ServiceProcess_c * svc, DWORD command)
{
	if ( !Open(svc) )
		return FALSE;

	SERVICE_STATUS status;
	BOOL ok = ::ControlService(schService, command, &status);
	error = ::GetLastError();
	return ok;
}




BOOL NT_ServiceManager_c::Query(ServiceProcess_c *svc, DWORD &p_state)
{
	if ( !Open(svc) )
		return FALSE;


    SERVICE_STATUS                      svrStatus;


    if ( QueryServiceStatus(schService, &svrStatus) == 0 )
        return FALSE;
    else
    {
        p_state = svrStatus.dwCurrentState;
		return TRUE;
    }
}




/****************************************************************
 *																*
 *					CLASS OF NOTIFIICONDATA_C					*
 *																*
 ****************************************************************/
class NotifyIconData_c : public NOTIFYICONDATA
{
public:
	NotifyIconData_c(HWND hWnd, UINT flags, const char *tip = NULL);
	void Add()    { Shell_NotifyIcon(NIM_ADD,    this); }
	void Delete() { Shell_NotifyIcon(NIM_DELETE, this); }
	void Modify() { Shell_NotifyIcon(NIM_MODIFY, this); }
};




NotifyIconData_c::NotifyIconData_c(HWND p_hwindow, UINT p_nflags, const char *p_sztip)
{
	cbSize = sizeof(NOTIFYICONDATA);
	hWnd   = p_hwindow;
	uID    = SYSTRAY_ICON_ID;
	uFlags = p_nflags;

	if ( p_sztip != NULL )
	{
		strncpy(szTip, p_sztip, sizeof(szTip) - 1);
		szTip[sizeof(szTip) - 1] = '\0';
		uFlags |= NIF_TIP;
	}
}




enum TrayIconRegistryCommand
{
	AddTrayIcon,
	DelTrayIcon,
	CheckTrayIcon
};




static BOOL TrayIconRegistry(ServiceProcess_c *p_svc, TrayIconRegistryCommand p_cmd)
{
	HKEY								key;


	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
						0,
						KEY_ALL_ACCESS,
						&key) != ERROR_SUCCESS )
		return FALSE;


	DWORD								err = 1;
	DWORD								type;
	DWORD								len;
	std::string								str;


	switch ( p_cmd )
	{
		case CheckTrayIcon :
			err = RegQueryValueEx(key, p_svc->GetName(), 0, &type, NULL, &len);
			break;

		case AddTrayIcon :
			str = "\"";
			str += p_svc->GetFile();
			str += "\" Tray";
			err = RegSetValueEx(key,
								p_svc->GetName(),
								0,
								REG_SZ,
								(LPBYTE)(const char *)str.c_str(),
								str.length() + 1);
			break;

		case DelTrayIcon :
			err = RegDeleteValue(key, (char *)(const char *)p_svc->GetName());
	}

	RegCloseKey(key);
	return err == ERROR_SUCCESS;
}




static BOOL IsServiceRunning(ServiceProcess_c * svc)
{
	HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, svc->GetName());
	if ( hEvent == NULL )
		return ::GetLastError() == ERROR_ACCESS_DENIED;

	CloseHandle(hEvent);
	return TRUE;
}




void MakeVersionInfo(DWORD p_dwMajor, DWORD p_dwMinor, char * p_szVer)
{
	sprintf(p_szVer, "%d.%d", p_dwMajor, p_dwMinor);
}




class Win95_ServiceManager : public ServiceManager_c
{
public:
    virtual BOOL Create(ServiceProcess_c * svc);
    virtual BOOL Delete(ServiceProcess_c * svc);
    virtual BOOL Start(ServiceProcess_c * svc);
    virtual BOOL Stop(ServiceProcess_c * svc);
    virtual BOOL Pause(ServiceProcess_c * svc);
    virtual BOOL Resume(ServiceProcess_c * svc);
};




BOOL Win95_ServiceManager::Create(ServiceProcess_c * svc)
{
	HKEY								key;

	
	if ( RegCreateKey(HKEY_LOCAL_MACHINE,
						"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
						&key) == ERROR_SUCCESS )
	{
		RegDeleteValue(key, (char *)(const char *)svc->GetName());
		RegCloseKey(key);
	}

	if ( (error = RegCreateKey(HKEY_LOCAL_MACHINE,
								"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices",
								&key)) != ERROR_SUCCESS )
		return FALSE;

	std::string cmd = "\"";
	cmd += svc->GetFile();
	cmd += "\"";

	error = RegSetValueEx(key,
							svc->GetName(),
							0,
							REG_SZ,
							(LPBYTE)(const char *)cmd.c_str(),
							cmd.length() + 1);

	RegCloseKey(key);

	return error == ERROR_SUCCESS;
}




BOOL Win95_ServiceManager::Delete(ServiceProcess_c * svc)
{
	HKEY								key;


	if ( RegCreateKey(HKEY_LOCAL_MACHINE,
						"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
						&key) == ERROR_SUCCESS )
	{
		RegDeleteValue(key, (char *)(const char *)svc->GetName());
		RegCloseKey(key);
	}

	if ( (error = RegCreateKey(HKEY_LOCAL_MACHINE,
								"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices",
								&key)) != ERROR_SUCCESS )
		return FALSE;

	error = RegDeleteValue(key, (char *)(const char *)svc->GetName());

	RegCloseKey(key);

	return error == ERROR_SUCCESS;
}




BOOL Win95_ServiceManager::Start(ServiceProcess_c * service)
{
	if ( IsServiceRunning(service) )
	{
		SPLOG(IFINE_SYSLOG::Info) << "Service already running" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		error = 1;
		return FALSE;
	}

	BOOL ok = _spawnl(_P_DETACH, service->GetFile(), service->GetFile(), NULL) >= 0;
	error = errno;
	return ok;
}




BOOL Win95_ServiceManager::Stop(ServiceProcess_c * service)
{
	HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, service->GetName());
	if ( hEvent == NULL )
	{
		error = ::GetLastError();
		SPLOG(IFINE_SYSLOG::Info) << "Service is not running" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		return FALSE;
	}

	SetEvent(hEvent);
	CloseHandle(hEvent);

	// Wait for process to go away.
	for ( unsigned int i = 0; i < 20; i++ )
	{
		hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, service->GetName());
		if ( hEvent == NULL )
			return TRUE;
		CloseHandle(hEvent);
		::Sleep(500);
	}

	error = 0x10000000;
	return FALSE;
}




BOOL Win95_ServiceManager::Pause(ServiceProcess_c *)
{
	SPLOG(IFINE_SYSLOG::Info) << "Cannot pause service under Windows 95" << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	error = 1;
	return FALSE;
}




BOOL Win95_ServiceManager::Resume(ServiceProcess_c *)
{
	SPLOG(IFINE_SYSLOG::Info) << "Cannot resume service under Windows 95" << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	error = 1;
	return FALSE;
}




/****************************************************************
 *																*
 *					CLASS OF ServiceProcess_c					*
 *																*
 ****************************************************************/
ServiceProcess_c::ServiceProcess_c()
{
	PProcessInstance = this;
	controlWindow = debugWindow = trayWindow = NULL;
	terminationValue = 0;

	p_argv = __argv;
	p_argc = __argc;


	char								szModulePath[MAX_PATH];


	GetModuleFileName(NULL, szModulePath, MAX_PATH);
	executableFile = szModulePath;
}




ServiceProcess_c::ServiceProcess_c(WORD majorVersion, WORD minorVersion, const char * manuf,
									const char * prod_desp, const char * prod_name,
									const char * svc_name, const char * svc_desp)
{
	PProcessInstance = this;
	controlWindow = debugWindow = trayWindow = NULL;
	terminationValue = 0;

	SetManufacturer(manuf);
	SetName(prod_name);
	SetProdDesp(prod_desp);
	SetSvcName(svc_name);
	SetSvcDesp(svc_desp);

	_logLevel = IFINE_SYSLOG::All;


	char								szVer[MAX_NAME_LEN];


	MakeVersionInfo(majorVersion, minorVersion, szVer);
	SetVersion(szVer);

	p_argv = __argv;
	p_argc = __argc;


	char								szModulePath[MAX_PATH];


	GetModuleFileName(NULL, szModulePath, MAX_PATH);
	executableFile = szModulePath;
}




ServiceProcess_c::~ServiceProcess_c()
{
	SPLOG(IFINE_SYSLOG::Info) << "delete global log instance" << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	OUTTOLOG_DESTROY(g_spLog)
	Log("~ServiceProcess_c Destroy");
}




ServiceProcess_c & ServiceProcess_c::Current()
{
	return * PProcessInstance;
}




const char * ServiceProcess_c::GetServiceDependencies() const
{
	return "EventLog\0";
}


BOOL ServiceProcess_c::IsServiceProcess() const
{
	return TRUE;
}




int ServiceProcess_c::mainfunc(void * arg)
{
	_debugMode = p_argc > 1 && stricmp(p_argv[1], "Debug") == 0;
	/************************************************************
	 * if app run mode is debug, then global log instance will  *
	 * be created in CreateControlWindow function. if not,		*
	 * create it here.											*
	 ************************************************************/
	if ( !_debugMode )
	{
		OUTTOWINDOWLOG_CREATE(g_spLog, debugWindow);
		g_spLog->SetLevel(_logLevel);
		OnBeforeStart();
		SPLOG(IFINE_SYSLOG::Info) << "global log created" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
	}


	hInstance = (HINSTANCE)arg;


	OSVERSIONINFO						verinfo;
	char								szVer[MAX_NAME_LEN];


	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	GetVersionEx(&verinfo);
	MakeVersionInfo(verinfo.dwMajorVersion, verinfo.dwMinorVersion, szVer);
	SetOSVersion(szVer);
	SetOSName("Microsoft");
	switch ( verinfo.dwPlatformId )
	{
		case VER_PLATFORM_WIN32_NT :
			isWin95 = FALSE;
			SetOSName("WINDOWS NT");
			break;

		case VER_PLATFORM_WIN32_WINDOWS :
			isWin95 = TRUE;
			SetOSName("WINDOWS 95");
			break;
		
		default :
			MessageBox(NULL, "Unsupported Win32 platform type!", GetName(), MB_TASKMODAL);
			return 1;
	}




	/************************************************************
	 *															*
	 *	App run type is operation mode without edit control		*
	 *															*
	 ************************************************************/
	if ( !_debugMode && p_argc > 1 )
	{
		for ( int a = 1; a < p_argc; a++ )
			ProcessCommand(p_argv[a]);

		if ( controlWindow == NULL || controlWindow == (HWND)-1 )
			return GetTerminationValue();

		if ( debugWindow != NULL && debugWindow != (HWND)-1 )
		{
			::SetLastError(0);
			SPLOG(IFINE_SYSLOG::Info) << "Close window or select another command from the Control menu.\n" << ends << endl;
			SPLOGEND(IFINE_SYSLOG::Info)
		}


		MSG									msg;


		while ( GetMessage(&msg, NULL, 0, 0) != 0 )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return GetTerminationValue();
	}




	/************************************************************
	 *															*
	 *					App run type is service					*
	 *															*
	 ************************************************************/
	if ( !_debugMode && !isWin95 )
	{
		static SERVICE_TABLE_ENTRY dispatchTable[] =
		{
			{ "", ServiceProcess_c::StaticMainEntry },
			{ NULL, NULL }
		};


		dispatchTable[0].lpServiceName = (char *)(const char *)GetSvcName();
		SPLOG(IFINE_SYSLOG::Info) << "service starting..." << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		if ( StartServiceCtrlDispatcher(dispatchTable) )
		{
			SPLOG(IFINE_SYSLOG::Info) << "service return" << ends << endl;
			SPLOGEND(IFINE_SYSLOG::Info)
			Log("service return");
			return GetTerminationValue();
		}

		MessageBox(NULL, "Not run as a service!", GetName(), MB_TASKMODAL);

		return 1;
	}



	/************************************************************
	 *															*
	 *		App run type is debug mode with edit control		*
	 *															*
	 ************************************************************/
	if ( !CreateControlWindow(_debugMode) )
		return 1;

	if ( IsServiceRunning(this) )
	{
		MessageBox(NULL, "Service already running", GetName(), MB_TASKMODAL);
		return 3;
	}
	ShowWindow(controlWindow, SW_SHOWDEFAULT);
	if ( _debugMode )
	{
		::SetLastError(0);
		SPLOG(IFINE_SYSLOG::Info) << "Service simulation started for \"" << GetName()
								  << "\" version " << GetVersion()
								  << "\r\n\t\tClose window to terminate" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
	}

	terminationEvent = CreateEvent(NULL, TRUE, FALSE, GetName());
	//  PAssertOS(terminationEvent != NULL);

	unsigned int						tid;


	_threadHandle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadEntry, this, 0, &tid);
	_maintenanceThreadHandle = (HANDLE)_beginthreadex(NULL, 0, StaticMaintenanceThread, this, CREATE_SUSPENDED, &tid);
	//  PAssertOS(threadHandle != (HANDLE)-1);

	SetTerminationValue(0);


	MSG									msg;

	try{
	msg.message = WM_QUIT + 1; //Want somethingthat is not WM_QUIT
	do
	{
		switch ( MsgWaitForMultipleObjects(1,
											&terminationEvent,
											FALSE,
											INFINITE,
											QS_ALLINPUT) )
		{
			case WAIT_OBJECT_0 + 1:
				while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
				{
					if ( msg.message != WM_QUIT )
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				break;

			default:
				// This is a work around for '95 coming up with an erroneous error
				if ( ::GetLastError() == ERROR_INVALID_HANDLE &&
						WaitForSingleObject(terminationEvent, 0) == WAIT_TIMEOUT )
				break;

			// Else fall into next case
			case WAIT_OBJECT_0:
				if ( !_debugMode || controlWindow == NULL )
					msg.message = WM_QUIT;
				else
				{
					SPLOG(IFINE_SYSLOG::Info) << "Service simulation stopped for \"" 
											  << GetName() 
											  << "\"\r\n\t\tClose window to terminate"
											  << ends << endl;
					SPLOGEND(IFINE_SYSLOG::Info)
					ResetEvent(terminationEvent);
				}
		}
	} while ( msg.message != WM_QUIT );
	}catch(...)
	{
		SPLOG(IFINE_SYSLOG::Info) << "msg loop err" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
	}
	if ( controlWindow != NULL )
		DestroyWindow(controlWindow);

	// Set thread ID for process to this thread
	////activeThreadMutex.Wait();
	////activeThreads.SetAt(threadId, NULL);
	////threadId = GetCurrentThreadId();
	////threadHandle = GetCurrentThread();
	////activeThreads.SetAt(threadId, this);
	////activeThreadMutex.Signal();
	OnStop();
	CloseHandle(_threadHandle);
	CloseHandle(_maintenanceThreadHandle);
	CloseHandle(terminationEvent);
	SPLOG(IFINE_SYSLOG::Info) << "main func return" << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	return GetTerminationValue();
}




enum
{
	ExitMenuID = 100,
	HideMenuID,
	ControlMenuID,
	CopyMenuID,
	CutMenuID,
	DeleteMenuID,
	SelectAllMenuID,
	OutputToMenuID,
	WindowOutputMenuID,
	OpenPropertyMenuID,
	ConfigWinMenuID,
	SvcCmdBaseMenuID = 1000,
	LogLevelBaseMenuID = 2000
};




static const char ServiceSimulationSectionName[] = "Service Simulation Parameters";
static const char WindowLeftKey[] = "Window Left";
static const char WindowTopKey[] = "Window Top";
static const char WindowRightKey[] = "Window Right";
static const char WindowBottomKey[] = "Window Bottom";
static const char SystemLogFileNameKey[] = "System Log File Name";




BOOL ServiceProcess_c::CreateControlWindow(BOOL createDebugWindow)
{
	if ( controlWindow != NULL )
		return TRUE;


	WNDCLASS							wclass;


	wclass.style = CS_HREDRAW|CS_VREDRAW;
	wclass.lpfnWndProc = (WNDPROC)StaticWndProc;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = hInstance;
	wclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
	wclass.hCursor = NULL;
	wclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wclass.lpszMenuName = NULL;
	wclass.lpszClassName = GetName();
	if ( RegisterClass(&wclass) == 0 )
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	HMENU menubar = CreateMenu();
	HMENU menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, OutputToMenuID, "&Output To...");
	AppendMenu(menu, MF_STRING, WindowOutputMenuID, "&Output To Window");
	AppendMenu(menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(menu, MF_STRING, ControlMenuID, "&Control");
	AppendMenu(menu, MF_STRING, HideMenuID, "&Hide");
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdVersion, "&Version");
	AppendMenu(menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(menu, MF_STRING, ExitMenuID, "E&xit");
	AppendMenu(menubar, MF_POPUP, (UINT)menu, "&File");

	menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, CopyMenuID, "&Copy");
	AppendMenu(menu, MF_STRING, CutMenuID, "C&ut");
	AppendMenu(menu, MF_STRING, DeleteMenuID, "&Delete");
	AppendMenu(menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(menu, MF_STRING, SelectAllMenuID, "&Select All");
	AppendMenu(menubar, MF_POPUP, (UINT)menu, "&Edit");

	menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdInstall, "&Install");
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdRemove, "&Remove");
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdDeinstall, "&Deinstall");
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdStart, "&Start");
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdStop, "S&top");
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdPause, "&Pause");
	AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdResume, "R&esume");
	AppendMenu(menubar, MF_POPUP, (UINT)menu, "&Control");

	menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::NoLog,   "&No Log");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::Fatal,   "&Fatal Error");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::Error,   "&Error");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::Warning, "&Warning");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::Info,    "&Information");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::Debug,   "&Debug");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::Debug2,  "Debug &2");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::Debug3,  "Debug &3");
	AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+IFINE_SYSLOG::All,  "All Information");
	AppendMenu(menubar, MF_POPUP, (UINT)menu, "&Log Level");

	if ( CreateWindow(GetName(),
						GetProdDesp(),
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, 0,
						CW_USEDEFAULT, 0, 
						NULL,
						menubar,
						hInstance,
						NULL) == NULL )
		return FALSE;
	
	if ( createDebugWindow && debugWindow == NULL )
	{
		////PConfig cfg(ServiceSimulationSectionName);
		////int l = cfg.GetInteger(WindowLeftKey, -1);
		////int t = cfg.GetInteger(WindowTopKey, -1);
		////int r = cfg.GetInteger(WindowRightKey, -1);
		////int b = cfg.GetInteger(WindowBottomKey, -1);

		int l = 20;
		int t = 20;
		int r = 820;
		int b = 620;

		if ( l > 0 && t > 0 && r > 0 && b > 0 )
			SetWindowPos(controlWindow, NULL, l, t, r-l, b-t, 0);

		debugWindow = CreateWindow("edit",
									"",
									WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE |
									WS_BORDER | ES_MULTILINE | ES_READONLY,
									CW_USEDEFAULT,
									0,
									CW_USEDEFAULT,
									0,
									controlWindow,
									(HMENU)10,
									hInstance,
									NULL);
		OUTTOWINDOWLOG_CREATE(g_spLog, debugWindow);
		g_spLog->SetLevel(_logLevel);
		OnBeforeStart();
		SendMessage(debugWindow, EM_SETLIMITTEXT, isWin95 ? 32000 : 128000, 0);
		DWORD TabStops[] = 
		{
			DATE_WIDTH,
			DATE_WIDTH + THREAD_WIDTH,
			DATE_WIDTH + THREAD_WIDTH + LEVEL_WIDTH,
			DATE_WIDTH + THREAD_WIDTH + LEVEL_WIDTH + PROTO_WIDTH,
			DATE_WIDTH + THREAD_WIDTH + LEVEL_WIDTH + PROTO_WIDTH + ACTION_WIDTH,
			DATE_WIDTH + THREAD_WIDTH + LEVEL_WIDTH + PROTO_WIDTH + ACTION_WIDTH + 32  // Standard tab width
		};
		SendMessage(debugWindow, EM_SETTABSTOPS, 6, (LPARAM)(LPDWORD)TabStops);
		ShowWindow(debugWindow, SW_SHOWDEFAULT);


		RECT								r1;
		GetClientRect(controlWindow, &r1);
		MoveWindow(debugWindow, 0, 0, r1.right, r1.bottom, TRUE);

		////systemLogFileName = cfg.GetString(SystemLogFileNameKey);
		////if (!systemLogFileName)
		////{
		////  PFile::Remove(systemLogFileName);
		////  DebugOutput("Sending all system log output to \"" + systemLogFileName + "\".\n");
		////}
	}

	return TRUE;
}




LPARAM WINAPI ServiceProcess_c::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Current().WndProc(hWnd, msg, wParam, lParam);
}




static void SaveWindowPosition(HWND hWnd)
{
	RECT								r;


	GetWindowRect(hWnd, &r);
////  PConfig cfg(ServiceSimulationSectionName);
  ////cfg.SetInteger(WindowLeftKey, r.left);
  ////cfg.SetInteger(WindowTopKey, r.top);
  ////cfg.SetInteger(WindowRightKey, r.right);
  ////cfg.SetInteger(WindowBottomKey, r.bottom);
}




LPARAM ServiceProcess_c::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	static DWORD						allocationNumber;
#endif
	std::string							a;
	NT_ServiceManager_c					nt;
	DWORD								state;


	switch ( msg )
	{
		case WM_CREATE :
			controlWindow = hWnd;
			break;

		case WM_DESTROY :
			if ( debugWindow == (HWND)-1 )
			{
				NotifyIconData_c nid(hWnd, NIF_TIP);
				nid.Delete(); // This removes the systray icon
			}

			controlWindow = debugWindow = trayWindow = NULL;

			PostQuitMessage(0);
			break;

		case WM_ENDSESSION :
			if ( wParam &&
					(_debugMode || lParam != ENDSESSION_LOGOFF) &&
					debugWindow != (HWND)-1 )
				OnStop();
			return 0;

		case WM_MOVE :
			if ( debugWindow != NULL )
				SaveWindowPosition(hWnd);
			break;

		case WM_SIZE :
			if ( debugWindow != NULL && debugWindow != (HWND)-1 )
			{
				SaveWindowPosition(hWnd);
				MoveWindow(debugWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			}
			break;

		case WM_INITMENUPOPUP :
			if ( _debugMode != 0 )
			{
				int enableItems = MF_BYCOMMAND|(_debugMode ? MF_ENABLED : MF_GRAYED);
				for ( int i = IFINE_SYSLOG::NoLog; i < IFINE_SYSLOG::NumLogLevels; i++ )
				{
					CheckMenuItem((HMENU)wParam, LogLevelBaseMenuID+i, MF_BYCOMMAND|MF_UNCHECKED);
					EnableMenuItem((HMENU)wParam, LogLevelBaseMenuID+i, enableItems);
				}
				CheckMenuItem((HMENU)wParam, LogLevelBaseMenuID+_logLevel, MF_BYCOMMAND|MF_CHECKED);

				enableItems = MF_BYCOMMAND|(_debugMode ? MF_GRAYED : MF_ENABLED);
				EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdStart, enableItems);
				EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdStop, enableItems);
				EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdPause, enableItems);
				EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdResume, enableItems);


				DWORD								start, finish;


				if ( debugWindow != NULL && debugWindow != (HWND)-1 )
					SendMessage(debugWindow, EM_GETSEL, (WPARAM)&start, (LPARAM)&finish);
				else
					start = finish = 0;
				enableItems = MF_BYCOMMAND|(start == finish ? MF_GRAYED : MF_ENABLED);
				EnableMenuItem((HMENU)wParam, CopyMenuID, enableItems);
				EnableMenuItem((HMENU)wParam, CutMenuID, enableItems);
				EnableMenuItem((HMENU)wParam, DeleteMenuID, enableItems);

				enableItems = MF_BYCOMMAND|(IsServiceRunning(this) ? MF_ENABLED : MF_GRAYED);
				EnableMenuItem((HMENU)wParam, ControlMenuID, enableItems);
				break;
			}

		case WM_COMMAND :
			switch ( wParam )
			{
				case ExitMenuID:
					DestroyWindow(hWnd);
					break;

				case OpenPropertyMenuID:
					DialogBox(hInstance,
							  MAKEINTRESOURCE(IDD_CTLWIN),
							  controlWindow,
							  StaticCtlWndProc);
					break;

				case ConfigWinMenuID:
					CreateConfigWin(hInstance, controlWindow);
					break;

				case ControlMenuID:
					if ( IsServiceRunning(this) )
						OnControl();
					break;

				case HideMenuID:
					ShowWindow(hWnd, SW_HIDE);
					break;

				case CopyMenuID :
					if ( debugWindow != NULL && debugWindow != (HWND)-1 )
						SendMessage(debugWindow, WM_COPY, 0, 0);
					break;

				case CutMenuID :
					if ( debugWindow != NULL && debugWindow != (HWND)-1 )
						SendMessage(debugWindow, WM_CUT, 0, 0);
					break;

				case DeleteMenuID :
					if ( debugWindow != NULL && debugWindow != (HWND)-1 )
						SendMessage(debugWindow, WM_CLEAR, 0, 0);
					break;

				case SelectAllMenuID :
					if ( debugWindow != NULL && debugWindow != (HWND)-1 )
						SendMessage(debugWindow, EM_SETSEL, 0, -1);
					break;

				case OutputToMenuID :
					if ( debugWindow != NULL && debugWindow != (HWND)-1 )
					{
						char								fileBuffer[_MAX_PATH];
						OPENFILENAME						fileDlgInfo;


						memset(&fileDlgInfo, 0, sizeof(fileDlgInfo));
						fileDlgInfo.lStructSize = sizeof(fileDlgInfo);
						fileDlgInfo.hwndOwner = hWnd;
						fileDlgInfo.hInstance = hInstance;
						fileBuffer[0] = '\0';
						fileDlgInfo.lpstrFile = fileBuffer;


						char								customFilter[100];
						
						
						strcpy(customFilter, "All Files");
						memcpy(&customFilter[strlen(customFilter)+1], "*.*\0", 5);
						fileDlgInfo.lpstrCustomFilter = customFilter;
						fileDlgInfo.nMaxCustFilter = sizeof(customFilter);
						fileDlgInfo.nMaxFile = sizeof(fileBuffer);
						fileDlgInfo.Flags = OFN_ENABLEHOOK|OFN_HIDEREADONLY|OFN_NOVALIDATE|OFN_EXPLORER|OFN_CREATEPROMPT;
						fileDlgInfo.lCustData = (DWORD)this;

						////if (GetSaveFileName(&fileDlgInfo))
						////{
						////if (systemLogFileName != fileBuffer) {
						////systemLogFileName = fileBuffer;
						////PFile::Remove(systemLogFileName);
						////PConfig cfg(ServiceSimulationSectionName);
						////cfg.SetString(SystemLogFileNameKey, systemLogFileName);
						////DebugOutput("Sending all system log output to \"" + systemLogFileName + "\".\n");
						//PError << "Logging started for \"" << GetName() << "\" version " << GetVersion(TRUE) << ends << endl;
						////}
						////}
					}
					break;

				case WindowOutputMenuID :
					////if (!systemLogFileName)
					////{
					////PError << "Logging stopped." << ends << endl;
					////DebugOutput("System log output to \"" + systemLogFileName + "\" stopped.\n");
					////systemLogFileName = PString();
					////PConfig cfg(ServiceSimulationSectionName);
					////cfg.SetString(SystemLogFileNameKey, "");
					////}
					break;

				default :
					if ( wParam >= LogLevelBaseMenuID+IFINE_SYSLOG::NoLog &&
							wParam < LogLevelBaseMenuID+IFINE_SYSLOG::NumLogLevels)
					{
						_logLevel = (IFINE_SYSLOG::Level)(wParam - LogLevelBaseMenuID);
						g_spLog->SetLevel(_logLevel);
					}
					else if ( wParam >= SvcCmdBaseMenuID &&
								wParam < SvcCmdBaseMenuID + NumSvcCmds )
					{
						const char * cmdname = ServiceCommandNames[wParam-SvcCmdBaseMenuID];
						a = cmdname;
						a += " ";
						a += GetName();
						a += "?";
						if ( wParam == SvcCmdBaseMenuID + SvcCmdVersion ||
								MessageBox(hWnd,
											a.c_str(),
											GetName(),
											MB_ICONQUESTION | MB_YESNO) == IDYES )
							ProcessCommand(cmdname);
					}
			}
			break;

		// Notification of event over sysTray icon
		case UWM_SYSTRAY:
			switch ( lParam )
			{
				case WM_MOUSEMOVE :
					if ( wParam == SYSTRAY_ICON_ID )
					{
					}
					break;

				// Click on icon - display message
				case WM_LBUTTONDBLCLK :
					if ( trayWindow == (HWND)-1 || trayWindow == NULL )
						DialogBox(hInstance,
								  MAKEINTRESOURCE(IDD_CTLWIN),
								  controlWindow,
								  StaticCtlWndProc);
					if ( trayWindow != (HWND)-1 && trayWindow != NULL )
						SetForegroundWindow(trayWindow);
					break;

				// Popup menu
				case WM_RBUTTONUP :
					POINT pt;
					GetCursorPos(&pt);

					HMENU menu = CreatePopupMenu();
					AppendMenu(menu, MF_STRING, OpenPropertyMenuID, "&Open Service Property");
					AppendMenu(menu, MF_STRING, ConfigWinMenuID, "Config...");
					AppendMenu(menu, MF_SEPARATOR, 0, NULL);
					if ( IsServiceRunning(this) )
					{
						MENUITEMINFO inf;
						inf.cbSize = sizeof(inf);
						inf.fMask = MIIM_STATE;
						inf.fState = MFS_DEFAULT;
						SetMenuItemInfo(menu, ControlMenuID, FALSE, &inf);
						AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdStart, "&Start Service");
						AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdStop, "&Stop Service");
						AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdPause, "&Pause Service");
						AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdResume, "&Resume Service");

						if ( nt.Query(this, state) )
						{
							switch ( state )
							{
								case SERVICE_STOPPED:
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStart, MF_ENABLED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStop, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdPause, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdResume, MF_GRAYED);
									break;

								case SERVICE_RUNNING:
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStart, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStop, MF_ENABLED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdPause, MF_ENABLED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdResume, MF_GRAYED);
									break;

								case SERVICE_PAUSED:
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStart, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStop, MF_ENABLED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdPause, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdResume, MF_ENABLED);
									break;

								case SERVICE_START_PENDING:
								case SERVICE_STOP_PENDING:
								case SERVICE_CONTINUE_PENDING:
								case SERVICE_PAUSE_PENDING:
								default:
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStart, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStop, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdPause, MF_GRAYED);
									EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdResume, MF_GRAYED);
									break;
							}
						}
					}
					else
					{
						EnableMenuItem(menu, ControlMenuID, MF_GRAYED);
						AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdStart, "&Start Service");
						AppendMenu(menu, MF_GRAYED, SvcCmdBaseMenuID + SvcCmdStop, "&Stop Service");
						AppendMenu(menu, MF_GRAYED, SvcCmdBaseMenuID + SvcCmdPause, "&Pause Service");
						AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdResume, "&Resume Service");

						if ( nt.Query(this, state) == false )
							EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStart, MF_GRAYED);
						EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdStop, MF_GRAYED);
						EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdPause, MF_GRAYED);
						EnableMenuItem(menu, SvcCmdBaseMenuID + SvcCmdResume, MF_GRAYED);
					}
					//AppendMenu(menu, MF_SEPARATOR, 0, NULL);
					//AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdNoTray, "&Tray Icon");
					//CheckMenuItem(menu, SvcCmdBaseMenuID + SvcCmdNoTray,
					//TrayIconRegistry(this, CheckTrayIcon) ? MF_CHECKED : MF_UNCHECKED);
					AppendMenu(menu, MF_SEPARATOR, 0, NULL);
					AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID + SvcCmdVersion, "&Version");
					AppendMenu(menu, MF_STRING, ExitMenuID, "&Close");

					/* SetForegroundWindow and the ensuing null PostMessage is a
					workaround for a Windows 95 bug (see MSKB article Q135788,
					http://www.microsoft.com/kb/articles/q135/7/88.htm, I think).
					In typical Microsoft style this bug is listed as "by design".
					SetForegroundWindow also causes our MessageBox to pop up in front
					of any other application's windows. */
					SetForegroundWindow(hWnd);
					/* We specifiy TPM_RETURNCMD, so TrackPopupMenu returns the menu
					selection instead of returning immediately and our getting a
					WM_COMMAND with the selection. You don't have to do it this way.
					*/
					WndProc(hWnd,
							WM_COMMAND,
							TrackPopupMenu(menu,
											TPM_RETURNCMD | TPM_RIGHTBUTTON,
											pt.x,
											pt.y,
											0,
											hWnd,
											NULL),
							0);
					PostMessage(hWnd, 0, 0, 0); // see above
					DestroyMenu(menu); // Delete loaded menu and reclaim its resources
					break;
			}

		case WM_TIMER:
			if ( nt.Query(this, state) )
			{
				switch ( state )
				{
					case SERVICE_STOPPED:
						OnStopState();
						break;

					case SERVICE_START_PENDING:
					case SERVICE_STOP_PENDING:
					case SERVICE_CONTINUE_PENDING:
					case SERVICE_PAUSE_PENDING:
						OnUnknownState();
						break;

					case SERVICE_RUNNING:
						OnStartState();
						break;

					case SERVICE_PAUSED:
						OnPauseState();
						break;

					default:
						break;
				}
			}
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}




INT_PTR CALLBACK ServiceProcess_c::StaticCtlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Current().CtlWndProc(hWnd, msg, wParam, lParam);
}




INT_PTR ServiceProcess_c::CtlWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	std::string							sTitle;
	bool								bAuto = false;
	HWND								hwndOwner; 
	RECT								rcOwner, rc;
	LPDRAWITEMSTRUCT					lpdis;
	HDC									hdcMem;
	HWND								hButtonWnd;


	switch ( uMessage )
	{
		case WM_INITDIALOG:
			trayWindow = hWnd;
			sTitle = GetName();
			sTitle += "服务管理器";
			SetWindowText(hWnd, sTitle.c_str());

			hwndOwner = GetDesktopWindow();
			GetWindowRect(hwndOwner, &rcOwner);
			GetWindowRect(hWnd, &rc);
			SetWindowPos(hWnd,
						 HWND_TOP,
						 ( rcOwner.right - rc.right + rc.left ) / 2, 
						 ( rcOwner.bottom - rc.bottom + rc.top ) / 2,
						 0,
						 0,
						 SWP_NOSIZE);

            hbmRun = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_START));
			hbmPause = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_PAUSE));
            hbmStop = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_STOP));

			bAuto = DrawState(GetWindowDC(GetDlgItem(hWnd, IDC_BUTTON_START)),
						NULL,
						NULL,
						(LPARAM)hbmRun,
						NULL,
						1,
						1,
						0,
						0,
						DST_BITMAP);

			if ( IsServiceRunning(this) )
			{
				OnStartState();
			}
			else
			{
				OnUnknownState();
			}
			return TRUE;

		case WM_CLOSE:
			DeleteObject(hbmRun);
			DeleteObject(hbmPause);
			DeleteObject(hbmStop);
			trayWindow = (HWND)-1;
			break;

        case WM_DRAWITEM: 
            lpdis = (LPDRAWITEMSTRUCT)lParam; 
            hdcMem = CreateCompatibleDC(lpdis->hDC); 
 
			if ( ( hButtonWnd = GetDlgItem(hWnd, IDC_BUTTON_START) ) == lpdis->hwndItem )
                SelectObject(hdcMem, hbmRun);
            else if ( ( hButtonWnd = GetDlgItem(hWnd, IDC_BUTTON_PAUSE) ) == lpdis->hwndItem )
                SelectObject(hdcMem, hbmPause);
			else if ( ( hButtonWnd = GetDlgItem(hWnd, IDC_BUTTON_STOP) ) == lpdis->hwndItem )
				SelectObject(hdcMem, hbmStop);
 
            // Destination 
            StretchBlt(lpdis->hDC,         // destination DC 
					   lpdis->rcItem.left, // x upper left 
					   lpdis->rcItem.top,  // y upper left 
					   lpdis->rcItem.right - lpdis->rcItem.left, 
					   lpdis->rcItem.bottom - lpdis->rcItem.top,
					   hdcMem,    // source device context 
					   6, 6,      // x and y upper left 
					   12,        // source bitmap width 
					   12,        // source bitmap height 
					   SRCCOPY);  // raster operation
            DeleteDC(hdcMem);

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

				case IDC_BUTTON_START:
					if ( IsWindowEnabled(GetDlgItem(hWnd, IDC_BUTTON_STOP)) == FALSE )
					{
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_START), FALSE);
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_PAUSE), FALSE);
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_STOP), FALSE);
						if ( ProcessCommand("Start") == TRUE )
						{
							OnStartState();
						}
						else
						{
							OnUnknownState();
						}
					}
					else
					{
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_START), FALSE);
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_PAUSE), FALSE);
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_STOP), FALSE);
						if ( ProcessCommand("Resume") == TRUE )
						{
							OnStartState();
						}
						else
						{
							OnUnknownState();
						}
					}
					return TRUE;

				case IDC_BUTTON_PAUSE:
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_START), FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_PAUSE), FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_STOP), FALSE);
					if ( ProcessCommand("Pause") == TRUE )
					{
						OnPauseState();
					}
					else
					{
						OnUnknownState();
					}
					return TRUE;

				case IDC_BUTTON_STOP:
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_START), FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_PAUSE), FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_STOP), FALSE);
					if ( ProcessCommand("Stop") == TRUE )
					{
						OnStopState();
					}
					else
					{
						OnUnknownState();
					}
					return TRUE;
			}
			switch( HIWORD(wParam) )
			{
				case BN_CLICKED:
					if ( IsDlgButtonChecked(hWnd, IDC_CHECK_AUTOSERVICE) == BST_CHECKED )
						bAuto = true;
					SetAutoRunAsOSStartup(bAuto);
					return TRUE;

				default:
					return FALSE;
			}
			break;
	}

	return FALSE;
}




void ServiceProcess_c::DebugOutput(const char * out)
{
	if ( controlWindow == NULL )
		return;

	if ( debugWindow == NULL || debugWindow == (HWND)-1 )
	{
		for ( int i = 0; i < 3; i++ )
		{
			const char * tab = strchr(out, '\t');
			if ( tab == NULL )
				break;
			out = tab + 1;
		}
		MessageBox(controlWindow, out, GetName(), MB_TASKMODAL);
		return;
	}


	if ( !IsWindowVisible(controlWindow) )
		ShowWindow(controlWindow, SW_SHOWDEFAULT);

	int len = strlen(out);
	int max = isWin95 ? 32000 : 128000;
	while ( GetWindowTextLength(debugWindow) + len >= max )
	{
		SendMessage(debugWindow, WM_SETREDRAW, FALSE, 0);


		DWORD								start, finish;
		
		
		SendMessage(debugWindow, EM_GETSEL, (WPARAM)&start, (LPARAM)&finish);
		SendMessage(debugWindow,
					EM_SETSEL,
					0,
					SendMessage(debugWindow, EM_LINEINDEX, 1, 0));
		SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)"");
		SendMessage(debugWindow, EM_SETSEL, start, finish);
		SendMessage(debugWindow, WM_SETREDRAW, TRUE, 0);
	}

	SendMessage(debugWindow, EM_SETSEL, max, max);


	/*char *								lf;
	const char *						prev = out;

	
	while ( ( lf = strchr(prev, '\n') ) != NULL )
	{
		if ( *(lf - 1) == '\r' )
			prev = lf+1;
		else
		{
			*lf++ = '\0';
			SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)out);
			SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)"\r\n");
			prev = out = lf;
		}
	}*/

	SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)out);
	SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)"\r\n");

	/*if ( *out != '\0' )
	{
		SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)out);
		SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)"\r\n");
	}*/
}




void ServiceProcess_c::StaticMainEntry(DWORD argc, LPTSTR * argv)
{
	Current().MainEntry(argc, argv);
}




void ServiceProcess_c::MainEntry(DWORD argc, LPTSTR * argv)
{
	unsigned int						tid;


	// SERVICE_STATUS members that don't change
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	status.dwServiceSpecificExitCode = 0;

	// register our service control handler:
	statusHandle = RegisterServiceCtrlHandler(GetName(), StaticControlEntry);
	if ( statusHandle == NULL )
	{
		SPLOG(IFINE_SYSLOG::Info) << "RegisterServiceCtrlHandler err" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		Log("RegisterServiceCtrlHandler err");
		return;
	}

	// report the status to Service Control Manager.
	if ( !ReportStatus(SERVICE_START_PENDING, NO_ERROR, 1, 20000) )
	{
		SPLOG(IFINE_SYSLOG::Info) << "ReportStatus err" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		Log("ReportStatus err");
		return;
	}

	// create the stop event object. The control handler function signals
	// this event when it receives the "stop" control code.
	terminationEvent = CreateEvent(NULL, TRUE, FALSE, GetName());
	if ( terminationEvent == NULL )
	{
		SPLOG(IFINE_SYSLOG::Info) << "Create termination event err" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		Log("Create termination event err");
		return;
	}

	startedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if ( startedEvent == NULL )
	{
		SPLOG(IFINE_SYSLOG::Info) << "Create started event err" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		Log("Create started event err");
		return;
	}
	// start the thread that performs the work of the service.
	_threadHandle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadEntry, this, 0, &tid);
	_maintenanceThreadHandle = (HANDLE)_beginthreadex(NULL, 0, StaticMaintenanceThread, this, CREATE_SUSPENDED, &tid);
	SPLOG(IFINE_SYSLOG::Info) << "create thread return handle " << _threadHandle << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	SPLOG(IFINE_SYSLOG::Info) << "create maintenance thread return handle " << _maintenanceThreadHandle << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	if ( _threadHandle != (HANDLE)0 && _maintenanceThreadHandle != (HANDLE)0 )
	{
		while ( WaitForSingleObject(startedEvent, 120000) == WAIT_TIMEOUT )
		{
			SPLOG(IFINE_SYSLOG::Info) << "wait for startedevent timeout " << ends << endl;
			SPLOGEND(IFINE_SYSLOG::Info)
			Log("wait for startedevent timeout");
			if ( ReportStatus(SERVICE_START_PENDING, NO_ERROR, 1, 20000) )
				break;
		}

		Log("wait here for the end...");
		SPLOG(IFINE_SYSLOG::Info) << "wait here for the end..." << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		// Wait here for the end
		WaitForSingleObject(terminationEvent, INFINITE);
		Log("Wait for terminationEvent return");
	}
	else
	{
		SPLOG(IFINE_SYSLOG::Info) << "main thread or maintenance thread 's handle is invalid" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		return;
	}

	CloseHandle(_threadHandle);
	CloseHandle(_maintenanceThreadHandle);
	CloseHandle(startedEvent);
	CloseHandle(terminationEvent);
	ReportStatus(SERVICE_STOPPED, 0);
}




unsigned int _stdcall ServiceProcess_c::StaticThreadEntry(void * arg)
{
	return ((ServiceProcess_c *)arg)->ThreadEntry();
}




unsigned int _stdcall ServiceProcess_c::StaticMaintenanceThread(void * arg)
{
	return ((ServiceProcess_c *)arg)->MaintenanceThreadEntry();
}




unsigned int ServiceProcess_c::ThreadEntry()
{
////  activeThreadMutex.Wait();
////  threadId = GetCurrentThreadId();
////  threadHandle = GetCurrentThread();
////  activeThreads.SetAt(threadId, this);
////  activeThreadMutex.Signal();

	SetTerminationValue(1);
	if ( OnStart() )
	{
		if ( !_debugMode )
			SetEvent(startedEvent);
		ReportStatus(SERVICE_RUNNING);
		SetTerminationValue(0);

		try{
			Main();
		}catch(...)
		{
			SPLOG(IFINE_SYSLOG::Info) << "main err" << ends << endl;
			SPLOGEND(IFINE_SYSLOG::Info)
		}

		ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 1, 30000);
	}


	SPLOG(IFINE_SYSLOG::Info) << "Service OnStart return false" << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	Log("Service OnStart return false");
	//SetEvent(terminationEvent);

	return 1;
}




unsigned int ServiceProcess_c::MaintenanceThreadEntry()
{
	try
	{
		Maintenance();
	}catch(...)
	{
		SPLOG(IFINE_SYSLOG::Info) << "maintenance err" << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
	}

	return 1;
}




void ServiceProcess_c::ResumeMaint(void)
{
	ResumeThread(_maintenanceThreadHandle);
}




void ServiceProcess_c::SuspendMaint(void)
{
	SuspendThread(_maintenanceThreadHandle);
}




void ServiceProcess_c::StaticControlEntry(DWORD code)
{
	Current().ControlEntry(code);
}




void ServiceProcess_c::ControlEntry(DWORD code)
{
	switch ( code )
	{
		case SERVICE_CONTROL_PAUSE: // Pause the service if it is running.
			if ( status.dwCurrentState != SERVICE_RUNNING )
				ReportStatus(status.dwCurrentState);
			else
			{
				if ( OnPause() )
					ReportStatus(SERVICE_PAUSED);
			}
			break;

		case SERVICE_CONTROL_CONTINUE: // Resume the paused service.
			if ( status.dwCurrentState == SERVICE_PAUSED )
			{
				OnContinue();
				ReportStatus(SERVICE_RUNNING);
			}
			else
				ReportStatus(status.dwCurrentState);
			break;

		case SERVICE_CONTROL_STOP: // Stop the service.
			// Report the status, specifying the checkpoint and waithint, before
			// setting the termination event.
			ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 1, 30000);
			OnStop();
			Log("Set termination event");
			Sleep(3500);
			SetEvent(terminationEvent);
			break;

		case SERVICE_CONTROL_INTERROGATE: // Update the service status.
		default :
			ReportStatus(status.dwCurrentState);
	}
}




BOOL ServiceProcess_c::ReportStatus(DWORD dwCurrentState,
									DWORD dwWin32ExitCode,
									DWORD dwCheckPoint,
									DWORD dwWaitHint)
{
	// Disable control requests until the service is started.
	if ( dwCurrentState == SERVICE_START_PENDING )
		status.dwControlsAccepted = 0;
	else
		status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	// These SERVICE_STATUS members are set from parameters.
	status.dwCurrentState	= dwCurrentState;
	status.dwWin32ExitCode	= dwWin32ExitCode;
	status.dwCheckPoint		= dwCheckPoint;
	status.dwWaitHint		= dwWaitHint;

	if ( _debugMode || isWin95 )
		return TRUE;

	// Report the status of the service to the service control manager.
	if ( SetServiceStatus(statusHandle, &status) )
		return TRUE;

	// If an error occurs, stop the service.
	SPLOG(IFINE_SYSLOG::Error) << "SetServiceStatus failed" << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Error)
	return FALSE;
}




BOOL ServiceProcess_c::OnStart()
{
	return TRUE;
}




void ServiceProcess_c::OnStop()
{
}




BOOL ServiceProcess_c::OnPause()
{
	SuspendThread(_threadHandle);
	return TRUE;
}




void ServiceProcess_c::OnContinue()
{
	ResumeThread(_threadHandle);
}




BOOL ServiceProcess_c::ProcessCommand(const char *cmd)
{
	unsigned int								cmdNum = 0;


	while ( stricmp(cmd, ServiceCommandNames[cmdNum]) != 0 )
	{
		if ( ++cmdNum >= NumSvcCmds )
		{
			if ( !CreateControlWindow(TRUE) )
				return FALSE;

			if ( *cmd != '\0' )
			{
				SPLOG(IFINE_SYSLOG::Info) << "Unknown command \"" << cmd << "\"." << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
			}
			else
			{
				SPLOG(IFINE_SYSLOG::Info) << "Could not start service." << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
			}
			
			SPLOG(IFINE_SYSLOG::Info) << "usage: " << GetName() << " [ " << ends << endl;
			SPLOGEND(IFINE_SYSLOG::Info)
			for ( cmdNum = 0; cmdNum < NumSvcCmds-1; cmdNum++ )
			{
				SPLOG(IFINE_SYSLOG::Info) << ServiceCommandNames[cmdNum] << " | " << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
			}
			SPLOG(IFINE_SYSLOG::Info) << ServiceCommandNames[cmdNum] << " ]" << ends << endl;
			SPLOGEND(IFINE_SYSLOG::Info)
			return FALSE;
		}
	}


	NT_ServiceManager_c					nt;
	Win95_ServiceManager				win95;
	ServiceManager_c					*svcManager;
	char								loginfo[4096];


	if ( isWin95 )
		svcManager = &win95;
	else
		svcManager = &nt;

	BOOL good = FALSE;
	switch ( cmdNum )
	{
		case SvcCmdNoWindow:
			if ( controlWindow == NULL )
				controlWindow = (HWND)-1;
			break;

		case SvcCmdTray:
			OnRunType(SvcCmdTray);
			if ( CreateControlWindow(FALSE) )
			{
				NotifyIconData_c nid(controlWindow, NIF_MESSAGE | NIF_ICON, GetName());

				nid.uCallbackMessage = UWM_SYSTRAY;
				nid.hIcon = LoadAppIcon();
				nid.Add();
				SetTimer(controlWindow, 1, 5000, NULL);
				debugWindow = (HWND)-1;
				return TRUE;
			}
			return FALSE;

		case SvcCmdNoTray:
			if ( TrayIconRegistry(this, CheckTrayIcon) )
			{
				TrayIconRegistry(this, DelTrayIcon);
				SPLOG(IFINE_SYSLOG::Info) << "Tray icon removed." << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
			}
			else
			{
				//TrayIconRegistry(this, AddTrayIcon);
				SPLOG(IFINE_SYSLOG::Info) << "Tray icon installed." << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
			}
			return TRUE;

		case SvcCmdVersion:
			::SetLastError(0);
			if ( debugWindow != NULL && debugWindow != (HWND)-1 )
			{
				SPLOG(IFINE_SYSLOG::Info) << GetName() << " - "
						<< GetProdDesp() << " Version " << GetVersion() << "\n"
						<< GetSvcName() << " - " << GetSvcDesp()
						<< " by " << GetManufacturer()
						<< " on " << GetOSClass()   << ' ' << GetOSName()
						<< " ("   << GetOSVersion() << '-' << GetOSHardware() << ')' << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
			}
			else
			{
				sprintf(loginfo,
						"%s - %s Version %s - %s - %s by %s on %s %s (%s-%s)",
						GetName(),
						GetProdDesp(),
						GetVersion(),
						GetSvcName(),
						GetSvcDesp(),
						GetManufacturer(),
						GetOSClass(),
						GetOSName(),
						GetOSVersion(),
						GetOSHardware());
				MessageBox(controlWindow, loginfo, "Product Version Infomation", MB_OK | MB_ICONWARNING);
			}
			return TRUE;

		case SvcCmdInstall:
			good = svcManager->Create(this);
			//TrayIconRegistry(this, AddTrayIcon);
			break;

		case SvcCmdRemove:
			good = svcManager->Delete(this);
			TrayIconRegistry(this, DelTrayIcon);
			break;

		case SvcCmdStart:
			good = svcManager->Start(this);
			break;

		case SvcCmdStop:
			good = svcManager->Stop(this);
			break;

		case SvcCmdPause:
			good = svcManager->Pause(this);
			break;

		case SvcCmdResume:
			good = svcManager->Resume(this);
			break;

		case SvcCmdDeinstall:
			svcManager->Delete(this);
			TrayIconRegistry(this, DelTrayIcon);
			
			
			////PConfig								cfg;


			////PStringList sections = cfg.GetSections();


			////unsigned int								i;
			
			
			////for ( i = 0; i < sections.GetSize(); i++ )
			////	cfg.DeleteSection(sections[i]);
			good = TRUE;
			break;
	}
	SetLastError(0);

	SPLOG(IFINE_SYSLOG::Info) << "Service command \"" << ServiceCommandNames[cmdNum] << "\" " << GetSvcName() << ends << endl;
	SPLOGEND(IFINE_SYSLOG::Info)
	if ( good )
	{
		SPLOG(IFINE_SYSLOG::Info) << "successful." << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
	}
	else
	{
		SPLOG(IFINE_SYSLOG::Info) << "failed - " << ends << endl;
		SPLOGEND(IFINE_SYSLOG::Info)
		switch ( svcManager->GetError() )
		{
			case ERROR_ACCESS_DENIED:
				SPLOG(IFINE_SYSLOG::Info) << "Access denied" << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
				break;

			case 0x10000000:
				SPLOG(IFINE_SYSLOG::Info) << "process still running." << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
				break;

			default:
				SPLOG(IFINE_SYSLOG::Info) << "error code = " << svcManager->GetError() << ends << endl;
				SPLOGEND(IFINE_SYSLOG::Info)
		}
	}


	return TRUE;
}




void ServiceProcess_c::OnStartState(void)
{
	std::string							a = GetName();
	a += " is running";
	NotifyIconData_c nid(controlWindow, NIF_MESSAGE | NIF_ICON, a.c_str());
	nid.uCallbackMessage = UWM_SYSTRAY;
	nid.hIcon = LoadStartIcon();
	nid.Modify();

	if ( trayWindow != (HWND)-1 && trayWindow != NULL )
	{
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_START), FALSE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_PAUSE), TRUE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_STOP), TRUE);
	}
}




void ServiceProcess_c::OnStopState(void)
{
	std::string							a = GetName();
	a += " is stopped";
	NotifyIconData_c nid(controlWindow, NIF_MESSAGE | NIF_ICON, a.c_str());
	nid.uCallbackMessage = UWM_SYSTRAY;
	nid.hIcon = LoadStopIcon();
	nid.Modify();

	if ( trayWindow != (HWND)-1 && trayWindow != NULL )
	{
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_START), TRUE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_PAUSE), FALSE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_STOP), FALSE);
	}
}




void ServiceProcess_c::OnPauseState(void)
{
	std::string							a = GetName();
	a += " is pausing";
	NotifyIconData_c nid(controlWindow, NIF_MESSAGE | NIF_ICON, a.c_str());
	nid.uCallbackMessage = UWM_SYSTRAY;
	nid.hIcon = LoadPauseIcon();
	nid.Modify();

	if ( trayWindow != (HWND)-1 && trayWindow != NULL )
	{
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_START), TRUE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_PAUSE), FALSE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_STOP), TRUE);
	}
}




void ServiceProcess_c::OnUnknownState(void)
{
	std::string							a = GetName();
	a += " is unknown state or hasn't been installed";
	NotifyIconData_c nid(controlWindow, NIF_MESSAGE | NIF_ICON, a.c_str());
	nid.uCallbackMessage = UWM_SYSTRAY;
	nid.hIcon = LoadAppIcon();
	nid.Modify();

	if ( trayWindow != (HWND)-1 && trayWindow != NULL )
	{
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_START), FALSE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_PAUSE), FALSE);
		EnableWindow(GetDlgItem(trayWindow, IDC_BUTTON_STOP), FALSE);
	}
}



void ServiceProcess_c::Log(const char * p_info)
{
}
// END OF FILE