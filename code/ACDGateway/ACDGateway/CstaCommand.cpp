#include "CstaCommand.h"


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

		}
		break;
	case MonitorViaDevice:
		{

		}
		break;
	case MakeCall:
		{

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

