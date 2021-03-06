#include "AGService.h"
#include "sp_resource.h"
#include "DBHelper.h"
#include "CstaCommand.h"
#include "CstaEventProcess.h"
#include "CstaInterface.h"
#include "MonitorThread.h"
#include "CstaReconnect.h"
#include "AGHttpServer.h"
#include "AGExcuteMakeCallThd.h"
#include "AGUtil.h"

AGService *gAGService;


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
										 AGHelper::sAcdgwCfg.httpSvrPort);
	}

	if (gAGExcuteMakeCall == NULL)
	{
		gAGExcuteMakeCall = new AGExcuteMakeCallThd();
	}
	return TRUE;
}




void AGService::OnStop()
{
	isDone = true;
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
	gAGExcuteMakeCall->start();
	OUTINFOLOG("ACDGateway start make call task thread.");
	StratMonitorDevice();

	while (!isDone)
	{
		AGTask *pTmpTask = NULL;
		try
		{
			tlMutex.lock();
			while(taskList.size() == 0)
			{
				tlMutex.wait();
			}
			pTmpTask = taskList.front();
			taskList.pop();
			tlMutex.unlock();
		}
		catch(...)
		{
			tlMutex.unlock();
		}

		if (pTmpTask != NULL)
		{
			try
			{
				switch(pTmpTask->GetTaskType())
				{
				case Distribute:
					{
						
					}
					break;
				case Transer:
					{
						TransferTask *transferTask = dynamic_cast<TransferTask *>(pTmpTask);
						if (transferTask->GetTransferTaskType() == Consult)
						{
							pTmpTask->ExcuteTask();
						}
					}
					break;
				case CancelDistribute:
					{
						pTmpTask->ExcuteTask();
					}
					break;
				case CancelTransfer:
					{
						pTmpTask->ExcuteTask();
					}
					break;
				default:
					break;
				}
			}
			catch(...)
			{
				OUTERRORLOG("ACDGateway service run thread occur exception.");
			}
		}
	}

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

void AGService::AddTaskToTL(AGTask *agTask)
{
	if (agTask == NULL)
	{
		return;
	}

	IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(tlMutex);
	taskList.push(agTask);
	if (taskList.size() == 1)
	{
		tlMutex.notify();
	}

}
std::string AGService::GetDialNo(std::string medType, std::string busType, std::string cystLvl)
{
	if (gDBHelper != NULL)
	{
		return gDBHelper->GetDialNo(medType, busType, cystLvl);
	}
	return "";
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

FDECLARE_MAINFRAME(AGService,
				   ACDGW_MAJOR_VERSION,
				   ACDGW_MINOR_VERSION,
				   ACDGW_MANUFACTURER,
				   ACDGW_PRODUCT_DESCRIPTION,
				   ACDGW_PRODUCT_NAME,
				   ACDGW_SERVICE_NAME,
				   ACDGW_SERVICE_DESCRIPTION)