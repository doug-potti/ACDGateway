#include "CstaCommand.h"
#include "CstaInterface.h"

CstaCmdThreadPtr                   gCstaCmdThread;

void CstaCommand::AddTsapiCmdToQueue(TsapiCommand_t *pCommand)
{
	if (pCommand == NULL)
	{
		return;
	}

	ICERECMUTEX::Lock lock(cqRecMutex);
	cmdQue.push(pCommand);
	if (cmdQue.size() == 1)
	{
		cqRecMutex.notify();
	}
}

void CstaCommand::run()
{
	while(!isDone)
	{
		ICERECMUTEX::Lock lock(cqRecMutex);
		while(cmdQue.size() == 0)
		{
			cqRecMutex.wait();
		}

		TsapiCommand_t *pTempCmd = NULL;
		try
		{
			pTempCmd = cmdQue.front();
			cmdQue.pop();
			ProcessCstaCmd(pTempCmd);
			if (pTempCmd != NULL)
			{
				delete pTempCmd;
			}
		}
		catch(...)
		{
			OUTERRORLOG("CstaCommandThread run occur exception.");
			if (pTempCmd != NULL)
			{
				delete pTempCmd;
			}
		}
	}
}

void CstaCommand::ProcessCstaCmd(TsapiCommand_t *pCommand)
{
	if (pCommand == NULL)
	{
		return;
	}

	switch(pCommand->tsapiCmdType)
	{
	case MonitorDevice:
		{
			if (gCstaInterfaceThd != NULL)
			{
				gCstaInterfaceThd->MonitorDevice(pCommand->activeDevId, 
												 pCommand->invokeId);
			}
		}
		break;
	case MonitorViaDevice:
		{
			if (gCstaInterfaceThd != NULL)
			{
				gCstaInterfaceThd->MonitorViaDevice(pCommand->activeDevId, 
													pCommand->invokeId);
			}
		}
		break;
	case MakeCall:
		{
			if (gCstaInterfaceThd != NULL)
			{
				gCstaInterfaceThd->CallMake(pCommand->activeDevId, 
											pCommand->destNo, 
											pCommand->uui, 
											pCommand->invokeId);
			}
		}
		break;
	case Release:
		{
			if (gCstaInterfaceThd != NULL)
			{
				gCstaInterfaceThd->CallRelease(pCommand->activeDevId,
											   pCommand->activeCallId,
											   pCommand->invokeId);
			}
		}
		break;
	}
}

void CstaCommand::Stop()
{
	ICERECMUTEX::Lock lock(cqRecMutex);
	isDone = true;
	cqRecMutex.notify();
}

