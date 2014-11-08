#include "AGTask.h"
#include "AGHttpServer.h"
#include "CstaCommand.h"
#include "AGExcuteMakeCallThd.h"


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

	TsapiCommand_t *pNewCommand = new TsapiCommand_t();
	if (pNewCommand)
	{
		pNewCommand->activeDevId = taskDevId;
		pNewCommand->destNo = dialNo;
		pNewCommand->uui = GetUui();
		pNewCommand->tsapiCmdType = DistributeMakeCall;
		pNewCommand->invokeId = AGHelper::GenerateInvokeId(taskDevId, DistributeMakeCall);
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

	AGTask *pTask = AGHelper::FindTaskByTaskId(taskId, Distribute); 

	if (pTask == NULL)
	{
		return;
	}

	if (pTask->GetTaskCallId() == -1) //distribute 任务 还未发起呼叫
	{
		std::string devId = pTask->GetTaskDevId();
		AGHelper::RemoveTaskByTaskId(taskId, Distribute);
		
		if (devId.size() > 0) //已找到关联分机
		{
			AGHelper::SetIdleTaskDev(devId);
		}
		else//等待空闲任务分机
		{
			AGHelper::SetIdleDevHandle();
		}
		if (gAGHttpServer != NULL)
		{
			gAGHttpServer->SendResponse(taskId,
										gAGHttpServer->ConstructCanDASuc(taskId) );
		}
		AGHelper::RemoveTaskByTaskId(taskId, CancelDistribute);
		return;
	}

	TsapiCommand_t *pNewCmd = new TsapiCommand_t();
	pNewCmd->activeDevId = pTask->GetTaskDevId();
	pNewCmd->activeCallId = pTask->GetTaskCallId();
	pNewCmd->tsapiCmdType = DistributeRelease;
	pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, DistributeRelease);
	if (gCstaCmdThread != NULL)
	{
		gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
	}

}

void TransferTask::ExcuteTask()
{
	std::stringstream ssLog("");
	ssLog<<"ACDGateway.TransferTask begin task mediaId:"<<
			taskId<<" sourceId:"<<
			sourceId<<" destId:"<<
			destId<<std::endl;
	OUTDEBUGLOG(ssLog.str());
	ssLog.str("");

	if (transferTaskType == Consult)
	{
		AGTask *disTask = AGHelper::FindTaskByTaskId(taskId, Distribute);
		if (disTask == NULL || disTask->GetTaskCallId() == -1)
		{
			return;
		}

		taskDevId = disTask->GetTaskDevId();
		taskCallId = disTask->GetTaskCallId();
		activeCallId = taskCallId;
		monRefId = disTask->GetMonRfId();
		TsapiCommand_t *pNewCmd = new TsapiCommand_t();
		pNewCmd->activeCallId = activeCallId;
		pNewCmd->activeDevId = taskDevId;
		pNewCmd->destNo = destId;
		pNewCmd->uui = GetUui();
		pNewCmd->tsapiCmdType = TranConsult;
		pNewCmd->invokeId = AGHelper::GenerateInvokeId(taskDevId, TranConsult);
		if (gCstaCmdThread != NULL)
		{
			gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
		}

	}
	else if (transferTaskType == Make)
	{
		TsapiCommand_t *pNewCommand = new TsapiCommand_t();
		if (pNewCommand)
		{
			pNewCommand->activeDevId = taskDevId;
			pNewCommand->destNo = destId;
			pNewCommand->uui = GetUui();
			pNewCommand->tsapiCmdType = TransferMakeCall;
			pNewCommand->invokeId = AGHelper::GenerateInvokeId(taskDevId, TransferMakeCall);
			if (gCstaCmdThread != NULL)
			{
				gCstaCmdThread->AddTsapiCmdToQueue(pNewCommand);
			}
		}
	}
	else
	{

	}


}

void CancelTransferTask::ExcuteTask()
{
	std::stringstream ssLog("");
	ssLog<<"ACDGateway.CancelTransferTask begin task mediaId:"<<
			taskId<<std::endl;
	OUTDEBUGLOG(ssLog.str());
	ssLog.str("");

	AGTask *pTask = AGHelper::FindTaskByTaskId(taskId, Transer);
	if (pTask == NULL)
	{
		return;
	}

	TransferTask* tranTask = dynamic_cast<TransferTask*>(pTask);

	if (tranTask->GetTransferTaskType() == Consult)
	{
		if (tranTask->GetHeldCallId() == -1)
		{
			AGHelper::RemoveTaskByTaskId(taskId, Transer);
			if (gAGHttpServer != NULL)
			{
				gAGHttpServer->SendResponse(taskId,gAGHttpServer->ConstructCanTranSuc(taskId) );
			}
			AGHelper::RemoveTaskByTaskId(taskId, CancelTransfer);
			return;
		}

		TsapiCommand_t *pNewCmd = new TsapiCommand_t();
		pNewCmd->activeDevId = tranTask->GetTaskDevId();
		pNewCmd->activeCallId = tranTask->GetActiveCallId();
		pNewCmd->heldCallId = tranTask->GetHeldCallId();
		pNewCmd->heldDevId = tranTask->GetTaskDevId();
		pNewCmd->tsapiCmdType = CancelTranConsult;
		pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, CancelTranConsult);
		if (gCstaCmdThread != NULL)
		{
			gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
		}
	}
	else if (tranTask->GetTransferTaskType() == Make)
	{
		if (tranTask->GetTaskCallId() == -1)
		{
			std::string devId = tranTask->GetTaskDevId();
			AGHelper::RemoveTaskByTaskId(taskId, Transer);
			if (devId.size() > 0) //已找到关联分机
			{
				AGHelper::SetIdleTaskDev(devId);
			}
			else//等待空闲任务分机
			{
				AGHelper::SetIdleDevHandle();
			}
			if (gAGHttpServer != NULL)
			{
				gAGHttpServer->SendResponse(taskId,
									        gAGHttpServer->ConstructCanTranSuc(taskId) );
			}
			AGHelper::RemoveTaskByTaskId(taskId, CancelTransfer);
			return;
		}
		TsapiCommand_t *pNewCmd = new TsapiCommand_t();
		pNewCmd->activeDevId = tranTask->GetTaskDevId();
		pNewCmd->activeCallId = tranTask->GetTaskCallId();
		pNewCmd->tsapiCmdType = TransferRelease;
		pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, TransferRelease);
		if (gCstaCmdThread != NULL)
		{
			gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
		}
	}
	else
	{

	}

}