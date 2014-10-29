#include "CstaReconnect.h"
#include "CstaInterface.h"
#include "MonitorThread.h"

CstaReconnectThreadPtr gCstaReconnectThd;

void CstaReconnectThd::run()
{
	std::stringstream sslog("");
	sslog<<"CstaReconnectThd.start reconnect thread."<<endl;
	OUTINFOLOG(sslog.str());
	sslog.str("");
	while (!isDone)
	{
		if (ReconnectTs())
		{
			sslog<<"CstaReconnectThd.run reconnect successed."<<std::endl;
			OUTINFOLOG(sslog.str());
			sslog.str("");
			if (gCstaInterfaceThd != NULL)
			{
				gCstaInterfaceThd->SetPauseFlag(false);
			}
			//等待下一次重练
			if (WaitForSingleObject(reconnectHandle, INFINITE) == WAIT_OBJECT_0)
			{
				sslog<<"CstaReconnectThd.start reconnect flow."<<std::endl;
				OUTINFOLOG(sslog.str());
				sslog.str("");
			}
		}
		else
		{
			sslog<<"CstaReconnectThd.run reconnect failed."<<std::endl;
			OUTINFOLOG(sslog.str());
			sslog.str("");
			Sleep(10000);
		}
	}
}

void CstaReconnectThd::StartCstaReconnect()
{
	if (gCstaInterfaceThd != NULL)
	{
		gCstaInterfaceThd->SetPauseFlag(true);
	}

	if (!isReconnecting)
	{
		isReconnecting = true;
		if (gCstaReconnectThd != NULL)
		{
			gCstaReconnectThd->start();
		}
	}
	else
	{
		SetEvent(reconnectHandle);
	}
}

void CstaReconnectThd::Stop()
{

}

bool CstaReconnectThd::ReconnectTs()
{
	if ( !gCstaInterfaceThd->OpenAcsStream(AGHelper::sAcdgwCfg.tsSvrId,
		AGHelper::sAcdgwCfg.tsLoginId,
		AGHelper::sAcdgwCfg.tsPasswnd))
	{
		OUTERRORLOG("ACDGateway Csta reconnect thread  reopen acs stream failed.");
		return false;
	}

	std::vector<Dev_t*>::iterator iter = AGHelper::sDevList.begin();
	for (; iter != AGHelper::sDevList.end(); ++iter)
	{
		if (gMonitorThread != NULL)
		{
			gMonitorThread->AddMonDevToMQ(*iter);
		}
	}
}