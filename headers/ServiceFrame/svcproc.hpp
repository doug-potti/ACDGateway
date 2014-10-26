/****************************************************************
 *																*
 * svcproc.hpp													*
 *																*
 * Copyright (c) 1993-2003 Shanghai Wilcom Information			*
 * Technology Co., Ltd.											*
 *																*
 ****************************************************************/




#ifndef _C_SERVICE_PROCESS__H
#define _C_SERVICE_PROCESS__H



#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <windows.h>


#include <shellapi.h>
#include <commdlg.h>


#include <process.h>
#include <string.h>


#include <thread_cs.hpp>
#include <thread_guard.hpp>
#include <sp_resource.h>
#include <splog.hpp>



const int MAX_VERSION_LEN										= 16;
const int MAX_NAME_LEN											= 64;




enum ServiceCommands_t
{
	SvcCmdTray,
	SvcCmdNoTray,
	SvcCmdVersion,
	SvcCmdInstall,
	SvcCmdRemove,
	SvcCmdStart,
	SvcCmdStop,
	SvcCmdPause,
	SvcCmdResume,
	SvcCmdDeinstall,
	SvcCmdNoWindow,
	NumSvcCmds
};




class ServiceProcess_c
{
public:
	ServiceProcess_c();

    ServiceProcess_c(WORD majorVersion,
					WORD minorVersion,
					const char * manuf,
					const char * prod_desp,
					const char * prod_name,
					const char * svc_name,
					const char * svc_desp);

	~ServiceProcess_c();

    virtual BOOL OnStart();

    virtual void OnStop();

	virtual BOOL OnPause();

    virtual void OnContinue();

    virtual void OnControl(){};

	virtual void Main(){};

	virtual void Maintenance(){};

    static ServiceProcess_c & Current();

    int mainfunc(void * arg = NULL);

    const char * GetServiceDependencies() const;

    BOOL IsServiceProcess() const;

	const char * GetName() { return processName.c_str(); }
	void SetName(const char * p_name)
	{
		memset(shortProcessName, '\0', 23);
		processName = p_name;
		strncpy(shortProcessName, p_name, 20);
		strcat(shortProcessName, "...");
	}

	const char * GetProdDesp() { return _szProcessDesp.c_str(); }
	void SetProdDesp(const char * p_proddesp)
	{
		_szProcessDesp = p_proddesp;
	}

	const char * GetSvcName() { return _szSvcName.c_str(); }
	void SetSvcName(const char * p_svcname)
	{
		_szSvcName = p_svcname;
	}

	const char * GetSvcDesp() { return _szSvcDesp.c_str(); }
	void SetSvcDesp(const char * p_svcdesp)
	{
		_szSvcDesp = p_svcdesp;
	}

	const char * GetFile() { return executableFile.c_str(); }

	const char * GetVersion() { return _szVersion.c_str(); }
	void SetVersion(const char * p_ver)
	{
		_szVersion = p_ver;
	}

	const char * GetManufacturer() { return _szManufacturer.c_str(); }
	void SetManufacturer(const char * p_manu)
	{
		_szManufacturer = p_manu;
	}

	const char * GetOSClass() { return _szOSClass.c_str(); }
	void SetOSClass(const char * p_oscls)
	{
		_szOSClass = p_oscls;
	}

	const char * GetOSName() { return _szOSName.c_str(); }
	void SetOSName(const char * p_osname)
	{
		_szOSName = p_osname;
	}

	const char * GetOSVersion() { return _szOSVersion.c_str(); }
	void SetOSVersion(const char * p_osver)
	{
		_szOSVersion = p_osver;
	}

	const char * GetOSHardware() { return _szOSHardware.c_str(); }
	void SetOSHardware(const char * p_oshard)
	{
		_szOSHardware = p_oshard;
	}

	void ChangeLogName(const char * p_szLogName)
	{
		_szLogFileName = p_szLogName;
		g_spLog->ChangeLogName(_szLogFileName.c_str());
	}

	static char **						p_argv;
	static int							p_argc;

	std::string							_szLogFileName;
	ThreadCriticalSection				_logNameMutex;

	HBITMAP								hbmPause;
	HBITMAP								hbmStop;
	HBITMAP								hbmRun;


protected:
    BOOL								_debugMode;
	std::string							processName;
	std::string							executableFile;

	std::string							_szProcessDesp;

	std::string							_szSvcName;
	std::string							_szSvcDesp;

	std::string							_szVersion;
	std::string							_szManufacturer;
	std::string							_szOSClass;
	std::string							_szOSName;
	std::string							_szOSVersion;
	std::string							_szOSHardware;
    
	int									terminationValue;

    HANDLE								_threadHandle;
	HANDLE								_maintenanceThreadHandle;
    unsigned int						_threadId;


public:
    static void __stdcall StaticMainEntry(DWORD argc, LPTSTR * argv);

    void MainEntry(DWORD argc, LPTSTR * argv);

    static unsigned int _stdcall StaticThreadEntry(void *);

	static unsigned int _stdcall StaticMaintenanceThread(void *);

    unsigned int ThreadEntry();

	unsigned int MaintenanceThreadEntry();

    static void __stdcall StaticControlEntry(DWORD code);

    void ControlEntry(DWORD code);

    static void Control_C(int);

    BOOL ReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode = NO_ERROR, DWORD dwCheckPoint = 0, DWORD dwWaitHint = 0);

    BOOL ProcessCommand(const char * cmd);

    BOOL CreateControlWindow(BOOL createDebugWindow);
    static LPARAM WINAPI StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LPARAM WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK StaticCtlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    INT_PTR CtlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void DebugOutput(const char * out);
	int GetTerminationValue() const { return terminationValue; }
	void SetTerminationValue(int value) { terminationValue = value; }


	virtual void OnBeforeStart() = 0 {}
	virtual void SetAutoRunAsOSStartup(bool) = 0 {}
	virtual HICON LoadAppIcon() = 0 {}
	virtual HICON LoadStartIcon() = 0 {}
	virtual HICON LoadPauseIcon() = 0 {}
	virtual HICON LoadStopIcon() = 0 {}
	virtual void CreateConfigWin(HINSTANCE, HWND) = 0 {}
	virtual void OnRunType(ServiceCommands_t){}


	void OnStartState(void);
	void OnStopState(void);
	void OnPauseState(void);
	void OnUnknownState(void);


	void ResumeMaint(void);
	void SuspendMaint(void);
	virtual void Log(const char *);


    BOOL								isWin95;
    SERVICE_STATUS						status;
    SERVICE_STATUS_HANDLE				statusHandle;
    HANDLE								startedEvent;
    HANDLE								terminationEvent;
    HWND								controlWindow;
    HWND								debugWindow;
	HWND								trayWindow;
	char								shortProcessName[23];
	IFINE_SYSLOG::Level					_logLevel;
	static HINSTANCE					hInstance;
};




#define FDECLARE_MAINFRAME(cls_name, major, minor, manu, prod_des, prod_name, svc_name, svc_des) \
	OUTTOLOG_DECLARE(g_spLog);	\
	static cls_name instance(major, minor, manu, prod_des, prod_name, svc_name, svc_des); \
	int PASCAL WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) \
	{ return instance.mainfunc(hInst);}




#endif
