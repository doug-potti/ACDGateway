#include "AGTask.h"
#include "AGHttpServer.h"
#include "CstaCommand.h"



void DistributeAgentTask::ExcuteTask()
{
	std::stringstream ssLog("");
	ssLog<<"ACDGateway.DistributeAgentTask begin task mediaId:"<<
			taskId<<" mediaType:"<<
			mediaType<<" bussinessType:"<<
			businessType<< " customerLevel:"<<
			customerLevel<< " taskDevId:"<<
			taskDevId<< " dialNo:"<<
			dialNo<<std::endl;
	OUTDEBUGLOG(ssLog.str());
	ssLog.str("");

	if (dialNo.empty())
	{
		ssLog<<"ACDGateway.DistributeAgentTask not find dialing skill taskId"<<
				taskId<<std::endl;
		OUTERRORLOG(ssLog.str());
		ssLog.str("");
		if (gAGHttpServer != NULL)
		{
			gAGHttpServer->SendResponse(taskId,
										gAGHttpServer->ConstructFailDesc(taskId, "NotFindSkill") );
		}
		AGHelper::SetIdleTaskDev(taskDevId);
		AGHelper::RemoveTaskByTaskId(taskId);
		return;
	}

	TsapiCommand_t *pNewCommand = new TsapiCommand_t();
	if (pNewCommand)
	{
		pNewCommand->activeDevId = taskDevId;
		pNewCommand->destNo = dialNo;
		pNewCommand->uui = GetUui();
		pNewCommand->tsapiCmdType = MakeCall;
		pNewCommand->invokeId = AGHelper::GenerateInvokeId(taskDevId, MakeCall);
		if (gCstaCmdThread != NULL)
		{
			gCstaCmdThread->AddTsapiCmdToQueue(pNewCommand);
		}
	}
}

void CancelDisAgtTask::ExcuteTask()
{
	std::stringstream ssLog("");
	ssLog<<"ACDGateway.CancelDisAgtTask excute task mediaId:"<<
			taskId<<std::endl;
	OUTDEBUGLOG(ssLog.str());
	ssLog.str("");

	AGTask *pTask = AGHelper::FindTaskByTaskId(taskId);
	if (pTask == NULL)
	{
		if (gAGHttpServer != NULL)
		{
			gAGHttpServer->SendResponse(taskId,
				                        gAGHttpServer->ConstructFailDesc(taskId, "NoDistrbuteTask") );
		}
		delete this;
		return;
	}

	if (pTask->GetTaskCallId() == -1)
	{
		AGHelper::RemoveTaskByTaskId(pTask->GetTaskId());
		if (gAGHttpServer != NULL)
		{
			gAGHttpServer->SendResponse(taskId,
										gAGHttpServer->ConstructCanDASuc(taskId) );
		}
		delete this;
		return;
	}

	TsapiCommand_t *pNewCmd = new TsapiCommand_t();
	pNewCmd->activeDevId = pTask->GetTaskDevId();
	pNewCmd->activeCallId = pTask->GetTaskCallId();
	pNewCmd->tsapiCmdType = Release;
	pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, Release);
	if (gCstaCmdThread != NULL)
	{
		gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
	}

}

void TransferTask::ExcuteTask()
{

}

void CancelTransferTask::ExcuteTask()
{

}