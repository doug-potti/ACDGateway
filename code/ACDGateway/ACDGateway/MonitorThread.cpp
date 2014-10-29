#include "MonitorThread.h"
#include "CstaCommand.h"

MonitorThdPtr     gMonitorThread;

void MonitorThread::AddMonDevToMQ(Dev_t *pDev)
{
	if (pDev == NULL)
	{
		return;
	}

	ICERECMUTEX::Lock lock(mqRecMutex);
	monQueue.push(pDev);
	if (monQueue.size() == 1)
	{
		mqRecMutex.notify();
	}
}

void MonitorThread::run()
{
	while (!isDone)
	{
		ICERECMUTEX::Lock lock(mqRecMutex);
		while(monQueue.size() == 0)
		{
			mqRecMutex.wait();
		}

		try
		{
			Dev_t * tmpDev;
			tmpDev = monQueue.front();
			monQueue.pop();
			//�����طֻ�������command���У�ִ������
			TsapiCommand_t *newCommand = new TsapiCommand_t();
			newCommand->activeDevId = tmpDev->devId;
			switch(tmpDev->devType)
			{
			case Device:
				{
					newCommand->tsapiCmdType = MonitorDevice;
					newCommand->invokeId = AGHelper::GenerateInvokeId(tmpDev->devId,(int)MonitorDevice);
				}
				break;
			case Vdn:
				{
					newCommand->tsapiCmdType = MonitorViaDevice;
					newCommand->invokeId = AGHelper::GenerateInvokeId(tmpDev->devId,(int)MonitorDevice);
				}
				break;
			case Split:
				{
					newCommand->tsapiCmdType = MonitorDevice;
					newCommand->invokeId = AGHelper::GenerateInvokeId(tmpDev->devId,(int)MonitorDevice);
				}
				break;
			}
			if (gCstaCmdThread != NULL)
			{
				gCstaCmdThread->AddTsapiCmdToQueue(newCommand);
			}
		}
		catch(...)
		{
			OUTERRORLOG("monitor thread run occur exception.");
		}
	}
}

void MonitorThread::Stop()
{
	ICERECMUTEX::Lock lock(mqRecMutex);
	isDone = true;
	mqRecMutex.notify();
}