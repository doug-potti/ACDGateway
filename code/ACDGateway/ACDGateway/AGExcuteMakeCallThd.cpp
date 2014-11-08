#include "AGExcuteMakeCallThd.h"
#include "DBHelper.h"
#include "AGUtil.h"

AGExcuteMakeCallThdPtr                     gAGExcuteMakeCall;

void AGExcuteMakeCallThd::AddAGMakeCallTaskToATQ(AGTask *pTask)
{
	if (pTask == NULL)
	{
		return;
	}

	IceUtil::Monitor<IceUtil::RecMutex> ::Lock lock(atqMutex);
	agmcTaskList.push_back(pTask);
	if (agmcTaskList.size() == 1)
	{
		atqMutex.notify();
	}
}

void AGExcuteMakeCallThd::RemoveMakeCallTask(std::string taskId, EnTaskType taskType)
{
	IceUtil::Monitor<IceUtil::RecMutex> ::Lock lock(atqMutex);
	AGMCTASKLIST::iterator iter = agmcTaskList.begin();
	for (; iter != agmcTaskList.end(); ++iter)
	{
		if ((*iter)->GetTaskId() == taskId &&
			(*iter)->GetTaskType() == taskType)
		{
			agmcTaskList.erase(iter);
			return;
		}
	}

}

void AGExcuteMakeCallThd::run()
{
	while(!isDone)
	{
		AGTask *pTmpTask = NULL;
		std::string currentTaskId = "";
		try
		{
			atqMutex.lock();
			while(agmcTaskList.size() == 0)
			{
				atqMutex.wait();
			}

			pTmpTask = FindAgTask();
			atqMutex.unlock();
		}
		catch(...)
		{
			atqMutex.unlock();
		}

		if (pTmpTask != NULL)
		{
			currentTaskId = pTmpTask->GetTaskId();
			try
			{
				switch(pTmpTask->GetTaskType())
				{
				case Distribute:
					{
						
						TaskDev_t *taskDev = NULL;
						while((taskDev = AGHelper::FindIdleTaskDev()) == NULL)
						{
							WaitForSingleObject(waitIdleDevHandle, INFINITE);
						}

						if (AGHelper::IsExistTask(currentTaskId, Distribute))
						{
							if (pTmpTask != NULL)
							{
								pTmpTask->SetRefId(taskDev->monitorRefId);
								pTmpTask->SetTaskDevId(taskDev->taskDevId);
								pTmpTask->ExcuteTask();
							}
						}
						else
						{
							AGHelper::SetIdleTaskDev(taskDev->taskDevId);
						}
						
					}
					break;
				case Transer:
					{
						TaskDev_t *taskDev = NULL;
						while((taskDev = AGHelper::FindIdleTaskDev()) == NULL)
						{
							WaitForSingleObject(waitIdleDevHandle, INFINITE);
						}

						if (AGHelper::IsExistTask(currentTaskId, Transer))
						{
							if (pTmpTask != NULL)
							{
								pTmpTask->SetRefId(taskDev->monitorRefId);
								pTmpTask->SetTaskDevId(taskDev->taskDevId);
								pTmpTask->ExcuteTask();
							}
						}
						else
						{
							AGHelper::SetIdleTaskDev(taskDev->taskDevId);
						}
					}
					break;
				case CancelDistribute:
					{
					}
					break;
				case CancelTransfer:
					{
					}
					break;
				default:
					break;
				}
			}
			catch(...)
			{
				OUTERRORLOG("ACDGateway excute make call thread occur exception.");
			}
		}
	}
}