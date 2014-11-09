#include "AGHttpServer.h"
#include "CstaEventProcess.h"
#include "CstaErrorDesc.h"
#include "MonitorThread.h"
#include "CstaReconnect.h"
#include "CstaCommand.h"
//

CstaEventProcessPtr gCstaEventProcessThd;

char CstaEventProcess::arrErrMsg[512];

void CstaEventProcess::run()
{
	while (!isDone)
	{
		IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(mutexQueue);
		while (pbxEventQueue.size()==0)
		{
			mutexQueue.wait();
		}

		try
		{
			AvayaPbxEvent_t *tempEvent = NULL;
			tempEvent = pbxEventQueue.front();
			pbxEventQueue.pop();
			ProcessAvayaPbxEvent(tempEvent);
		}
		catch (...)
		{
			OUTERRORLOG("CstaEventProcess.run thread occur exception.");
		}
	}
}

void CstaEventProcess::AddPbxEventToQueue(AvayaPbxEvent_t *pbxEvent)
{
	if (pbxEvent == NULL)
	{
		return;
	}

	IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(mutexQueue);
	pbxEventQueue.push(pbxEvent);
	if (pbxEventQueue.size() == 1)
	{
		mutexQueue.notify();
	}
}

void CstaEventProcess::ProcessAvayaPbxEvent(AvayaPbxEvent_t *pbxEventData)
{
	CheckEventData(pbxEventData);

	CSTAEvent_t *cstaEvent = (CSTAEvent_t *)pbxEventData->cstaEvent;
	try
	{
		switch (cstaEvent->eventHeader.eventClass)
		{
		case ACSREQUEST:
			break;
		case ACSUNSOLICITED:
			ProcessACSUnsolicited(pbxEventData);
			break;
		case ACSCONFIRMATION:
			ProcessACSConfirmation(pbxEventData);
			break;
		case CSTAREQUEST:
			break;
		case CSTAUNSOLICITED:
			ProcessCSTAUnsolicited(pbxEventData);
			break;
		case CSTACONFIRMATION:
			ProcessCSTAConfirmation(pbxEventData);
			break;
		case CSTAEVENTREPORT:
			ProcessCSTAEventReport(pbxEventData);
			break;
		}
	}
	catch(...)
	{
		OUTERRORLOG("CstaEventProcess.ProcessAvayaPbxEvent occur exception.");
		if (pbxEventData)
		{
			delete pbxEventData;
			pbxEventData = NULL;
		}

	}

	if (pbxEventData)
	{
		delete pbxEventData;
		pbxEventData = NULL;
	}
}

void CstaEventProcess::ProcessACSConfirmation(AvayaPbxEvent_t * pbxEventData)
{

}

void CstaEventProcess::ProcessACSUnsolicited(AvayaPbxEvent_t * pbxEventData)
{
	CheckEventData(pbxEventData);
	CSTAEvent_t *pcstaevent =                (CSTAEvent_t *)(pbxEventData->cstaEvent);
	ACSUnsolicitedEvent					     * pacsUnsoli;
	int									     errCode;
	std::stringstream                        sslog;
	sslog.str("");
	pacsUnsoli = &(pcstaevent->event.acsUnsolicited);
	switch (pcstaevent->eventHeader.eventType)
	{

	case ACS_UNIVERSAL_FAILURE:
		errCode = pacsUnsoli->u.failureEvent.error;

		CstaErrorDesc::GetACSUniversalFailureDes(errCode, arrErrMsg);
		sslog<<"CstaEventProcess.ProcessACSUnsolicited.ACS_UNIVERSAL_FAILURE \r\n code"<<
				arrErrMsg<<std::endl;

		OUTERRORLOG(sslog.str());
		sslog.str("");

		if ( pacsUnsoli->u.failureEvent.error == TSERVER_STREAM_FAILED )
		{
			sslog<<"CstaEventProcess.ProcessACSUnsolicited Stream to TServer is failed stop ts service and start restart thread"<<std::endl;
			OUTERRORLOG(sslog.str());
			sslog.str("");
			if (gCstaReconnectThd == NULL)
			{
				gCstaReconnectThd = new CstaReconnectThd();
			}
			gCstaReconnectThd->StartCstaReconnect();

		}
		break;
	default:
		OUTERRORLOG("CstaEventProcess.ProcessACSUnsolicited ACS UNSOLICITED EVENT    UNKNOWN EVENT TYPE\r\n");
		break;
	}
}

void CstaEventProcess::ProcessCSTAConfirmation(AvayaPbxEvent_t * pbxEventData)
{
	CheckEventData(pbxEventData);
	CSTAEvent_t *pCstaEvent = (CSTAEvent_t *)(pbxEventData->cstaEvent);
	ATTEvent_t *pAttevent = &(pbxEventData->attEvent);
	CSTAConfirmationEvent *pConEvent = &(pCstaEvent->event.cstaConfirmation);

	std::stringstream sslog("");
	switch(pCstaEvent->eventHeader.eventType)
	{
	case CSTA_QUERY_DEVICE_INFO_CONF:
		{

		}
		break;
	case CSTA_MONITOR_CONF:
		{
			std::string devId;
			long refid = pConEvent->u.monitorStart.monitorCrossRefID;
			int reqType = AGHelper::AnalyzeInvokeId(pConEvent->invokeID, devId);
			sslog<<"CstaEventProcess.ProcessCSTAConfirmation.CSTA_MONITOR_CONF monitor device conf deviceid:"<<
					devId<<" monitorrefid:"<<
					refid<<std::endl;
			OUTINFOLOG(sslog.str());
			sslog.str("");

			std::vector<Dev_t *>::iterator iter = AGHelper::sDevList.begin();
			for (; iter != AGHelper::sDevList.end(); ++iter)
			{
				if ((*iter)->devId == devId)
				{
					(*iter)->monRefId = refid;
					AGHelper::AddMonDevToMDM(refid, (*iter));
					if ((*iter)->devType == Device)
					{
						TaskDev_t *newTaskDev = new TaskDev_t(devId,refid);
						if (newTaskDev)
						{
							AGHelper::AddTaskDevToTDL(newTaskDev);
						}		
					}
				}
			}
		}
		break;
	case CSTA_SET_AGENT_STATE_CONF:
		{

		}
		break;
	case CSTA_UNIVERSAL_FAILURE_CONF:
		{
			std::string deviD;
			int cstaReqType = AGHelper::AnalyzeInvokeId(pConEvent->invokeID, deviD);
			std::string errdesc = CstaErrorDesc::GetCstaUniversalFailureDes(pConEvent->u.universalFailure.error);
			sslog<<"CstaEventProcess.ProcessCSTAConfirmation CSTA_UNIVERSAL_FAILURE_CONF csta conf failuer devId:"<<
					deviD <<" requestType:"<<
					AGHelper::ConvertTaspiCmdToString((EnTsapiCmdType)cstaReqType)<<" errmsg:"<<
					errdesc<<std::endl;
			OUTERRORLOG(sslog.str());
			sslog.str("");
			if (cstaReqType == (int)MonitorDevice ||
				cstaReqType == (int)MonitorViaDevice)
			{
				std::vector<Dev_t *>::iterator iter = AGHelper::sDevList.begin();
				for (; iter != AGHelper::sDevList.end(); ++ iter)
				{
					if ((*iter)->devId == deviD)
					{
						if (gMonitorThread !=NULL)
						{
							gMonitorThread->AddMonDevToMQ(*iter);
						}
					}
				}
			}
			else if (cstaReqType == (int)DistributeMakeCall)
			{
				AGTask *agTask = AGHelper::FindTaskByDevId(deviD, Distribute);
				if (agTask != NULL)
				{
					std::string tskId = agTask->GetTaskId();
					if (gAGHttpServer != NULL)
					{
						gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructFailDesc(tskId, "ResourceBusy"));
					}
					AGHelper::RemoveTaskByDevId(deviD, Distribute);
					AGHelper::SetIdleTaskDev(deviD);
				}
			}
			else if (cstaReqType == (int)TransferMakeCall)
			{
				AGTask *agTask = AGHelper::FindTaskByDevId(deviD, Transer);
				if (agTask != NULL)
				{
					std::string tskId = agTask->GetTaskId();
					if (gAGHttpServer != NULL)
					{
						gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructFailDesc(tskId, "ResourceBusy"));
					}
					AGHelper::RemoveTaskByDevId(deviD, Transer);
					AGHelper::SetIdleTaskDev(deviD);	
				}
			}
			else if(cstaReqType == (int)TranConsult)
			{
				AGTask *agTask = AGHelper::FindTaskByDevId(deviD, Transer);
				if (agTask != NULL)
				{
					std::string tskId = agTask->GetTaskId();
					TransferTask *tTask = dynamic_cast<TransferTask *>(agTask);
					if (gAGHttpServer != NULL)
					{
						gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructFailDesc(tskId, "ResourceBusy"));
					}
					//»Ö¸´×´Ì¬
					TsapiCommand_t *pNewCmd = new TsapiCommand_t();
					pNewCmd->activeDevId = tTask->GetTaskDevId();
					pNewCmd->activeCallId = tTask->GetActiveCallId();
					pNewCmd->heldCallId = tTask->GetHeldCallId();
					pNewCmd->heldDevId = tTask->GetTaskDevId();
					pNewCmd->tsapiCmdType = CancelTranConsult;
					pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, CancelTranConsult);
					if (gCstaCmdThread != NULL)
					{
						gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
					}
					
				}
			}
			else if (cstaReqType == (int)CancelTranConsult)
			{
				AGTask *agTask = AGHelper::FindTaskByDevId(deviD, CancelTransfer);
				if (agTask != NULL)
				{
					std::string tskId = agTask->GetTaskId();
					if (gAGHttpServer != NULL)
					{
						gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructFailDesc(tskId, "ResourceBusy"));
					}
					//»Ö¸´×´Ì¬
					AGHelper::RemoveTaskByDevId(deviD, CancelTransfer);
				}
			}
			else if (cstaReqType == (int)TranTransfer)
			{
				AGTask *agTask = AGHelper::FindTaskByDevId(deviD, Transer);
				if (agTask != NULL)
				{
					std::string tskId = agTask->GetTaskId();
					if (gAGHttpServer != NULL)
					{
						gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructFailDesc(tskId, "ResourceBusy"));
					}
					//»Ö¸´×´Ì¬
				}
			}
			else if (cstaReqType == (int)DistributeRelease)
			{

			}
			else if (cstaReqType == (int)TransferRelease)
			{

			}
		}
		break;
	default:
		break;
	}
}

void CstaEventProcess::ProcessCSTAUnsolicited(AvayaPbxEvent_t * pbxEventData)
{
	CheckEventData(pbxEventData);
	WriteCSTAUnsolicited(pbxEventData);
	CSTAEvent_t *pCstaEvent         = (CSTAEvent_t *)(pbxEventData->cstaEvent);
	CSTAUnsolicitedEvent *pCstaUnse = &(pCstaEvent->event.cstaUnsolicited);
	ATTEvent_t           *pAttevent = &(pbxEventData->attEvent);
	std::stringstream sslog("");
	switch (pCstaEvent->eventHeader.eventType)
	{
	case CSTA_CALL_CLEARED:
		{
			
		}
		break;
	case CSTA_CONFERENCED:
		{
			
		}
		break;
	case CSTA_CONNECTION_CLEARED:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			std::string tskId = "";
			AGTask *agTask = AGHelper::FindTaskByTaskDevRefId(refId, Distribute);
			if (agTask != NULL)
			{
				if (agTask->GetTaskCallId() == pCstaUnse->u.connectionCleared.droppedConnection.callID)
				{
					if (agTask->GetTaskDevId() == pCstaUnse->u.connectionCleared.releasingDevice.deviceID)
					{
						tskId = agTask->GetTaskId();
						AGTask *canTask = AGHelper::FindTaskByTaskId(tskId, CancelDistribute);
						if (canTask != NULL)
						{
							if (gAGHttpServer != NULL)
							{
								gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructCanDASuc(tskId));
							}
							AGHelper::RemoveTaskByTaskId(tskId, CancelDistribute);
						}
						AGHelper::RemoveTaskByRefId(refId, Distribute);
						AGHelper::SetIdleTaskDev(pCstaUnse->u.connectionCleared.releasingDevice.deviceID);
					}
				}
			}

			agTask = AGHelper::FindTaskByTaskDevRefId(refId,Transer);
			if (agTask != NULL)
			{
				TransferTask *tTask = dynamic_cast<TransferTask *>(agTask);

				if (tTask->GetTransferTaskType() == Make)
				{
					if (agTask->GetTaskDevId() == pCstaUnse->u.connectionCleared.releasingDevice.deviceID)
					{
						tskId = agTask->GetTaskId();
						AGTask *canTransferTask = AGHelper::FindTaskByTaskId(tskId, CancelTransfer);
						if (canTransferTask != NULL)
						{
							if (gAGHttpServer != NULL)
							{
								gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructCanDASuc(tskId));
							}
							AGHelper::RemoveTaskByTaskId(tskId, CancelTransfer);
						}
						AGHelper::RemoveTaskByRefId(refId, Transer);
						AGHelper::SetIdleTaskDev(pCstaUnse->u.connectionCleared.releasingDevice.deviceID);
					}
				}
				else if (tTask->GetTransferTaskType() == Consult)
				{
					if (agTask->GetTaskDevId() == pCstaUnse->u.connectionCleared.releasingDevice.deviceID)
					{
						tskId = agTask->GetTaskId();
						AGTask *canTransferTask = AGHelper::FindTaskByTaskId(tskId, CancelTransfer);
						if (canTransferTask != NULL)
						{
							if (gAGHttpServer != NULL)
							{
								gAGHttpServer->SendResponse(tskId,gAGHttpServer->ConstructCanDASuc(tskId));
							}
							AGHelper::RemoveTaskByTaskId(tskId, CancelTransfer);
						}
						AGHelper::RemoveTaskByRefId(refId, Transer);
						AGHelper::SetIdleTaskDev(pCstaUnse->u.connectionCleared.releasingDevice.deviceID);
					}
				}
			}
			
		}
		break;
	case CSTA_DELIVERED:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			AGTask *tranTask = AGHelper::FindTaskByTaskDevRefId(refId, Transer);
			if (tranTask != NULL)
			{
				TransferTask *ttask = dynamic_cast<TransferTask *>(tranTask);

				if (ttask->GetTransferTaskType() == Make)
				{

					std::string deliverId = pCstaUnse->u.delivered.alertingDevice.deviceID;
					if (!deliverId.empty())
					{
						std::string agentId = AGHelper::FindAgentIdByTerId(deliverId);
						if (agentId != "NOFIND")
						{
							gAGHttpServer->SendResponse(ttask->GetTaskId(),
								gAGHttpServer->ConstructTranSuc(ttask->GetTaskId()));
						}
						else
						{
							if (ttask->GetDestNo() == deliverId)
							{
								gAGHttpServer->SendResponse(ttask->GetTaskId(),
															gAGHttpServer->ConstructTranSuc(ttask->GetTaskId()));
							}
						}
					}
				}
				else if (ttask->GetTransferTaskType() == Consult)
				{
					std::string deliverId = pCstaUnse->u.delivered.alertingDevice.deviceID;
					if (!deliverId.empty())
					{
						std::string agentId = AGHelper::FindAgentIdByTerId(deliverId);
						if (agentId != "NOFIND")
						{
							TsapiCommand_t *pNewCmd = new TsapiCommand_t();
							pNewCmd->activeCallId = ttask->GetActiveCallId();
							pNewCmd->activeDevId = ttask->GetTaskDevId();
							pNewCmd->heldCallId = ttask->GetHeldCallId();
							pNewCmd->heldDevId = ttask->GetTaskDevId();
							pNewCmd->tsapiCmdType = TranTransfer;
							pNewCmd->invokeId = AGHelper::GenerateInvokeId(ttask->GetTaskDevId(), TranTransfer);
							if (gCstaCmdThread != NULL)
							{
								gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
							}
						}
						else
						{
							if (ttask->GetDestNo() == deliverId)
							{
								TsapiCommand_t *pNewCmd = new TsapiCommand_t();
								pNewCmd->activeCallId = ttask->GetActiveCallId();
								pNewCmd->activeDevId = ttask->GetTaskDevId();
								pNewCmd->heldCallId = ttask->GetHeldCallId();
								pNewCmd->heldDevId = ttask->GetTaskDevId();
								pNewCmd->tsapiCmdType = TranTransfer;
								pNewCmd->invokeId = AGHelper::GenerateInvokeId(ttask->GetTaskDevId(), TranTransfer);
								if (gCstaCmdThread != NULL)
								{
									gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
								}
							}
						}
					}
				}
				else
				{

				}
			}
			else
			{
				AGTask *agTask = AGHelper::FindTaskByTaskDevRefId(refId,Distribute);
				if (agTask != NULL)
				{
					std::string deliverId = pCstaUnse->u.delivered.alertingDevice.deviceID;
					if (!deliverId.empty())
					{
						std::string agentId = AGHelper::FindAgentIdByTerId(deliverId);
						if (agentId != "NOFIND")
						{
							if (gAGHttpServer != NULL)
							{
								DistributeAgentTask *dsaTask = dynamic_cast<DistributeAgentTask *>(agTask);
								dsaTask->SetAgentId(agentId);
								gAGHttpServer->SendResponse(agTask->GetTaskId(),
															gAGHttpServer->ConstructDisAgtSuc(agTask->GetTaskId(),
															agentId));
							}
						}
					}
				}
			}
		}
		break;
	case CSTA_DIVERTED:
		{
			
		}
		break;
	case CSTA_ESTABLISHED:
		{
			
		}
		break;
	case CSTA_FAILED:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			AGTask *agTask = AGHelper::FindTaskByTaskDevRefId(refId,Distribute);
			if (agTask != NULL)
			{
				TsapiCommand_t *pNewCmd = new TsapiCommand_t();
				pNewCmd->activeDevId = agTask->GetTaskDevId();
				pNewCmd->activeCallId = pCstaUnse->u.failed.failedConnection.callID;
				pNewCmd->tsapiCmdType = DistributeRelease;
				pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, DistributeRelease);

				if (gCstaCmdThread != NULL)
				{
					gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
				}
				if (gAGHttpServer != NULL)
				{

					gAGHttpServer->SendResponse(agTask->GetTaskId(),
						gAGHttpServer->ConstructFailDesc(agTask->GetTaskId(),"CstaCallFailed"));
				}
			}
			AGTask *transferTask = AGHelper::FindTaskByTaskDevRefId(refId,Transer);
			if (transferTask != NULL)
			{
				TransferTask* tTask = dynamic_cast<TransferTask*>(transferTask);
				if (tTask->GetTransferTaskType() == Consult)
				{
					TsapiCommand_t *pNewCmd = new TsapiCommand_t();
					pNewCmd->activeDevId = tTask->GetTaskDevId();
					pNewCmd->activeCallId = pCstaUnse->u.failed.failedConnection.callID;
					pNewCmd->tsapiCmdType = TransferRelease;
					pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, TransferRelease);

					if (gCstaCmdThread != NULL)
					{
						gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
					}
					if (gAGHttpServer != NULL)
					{
						gAGHttpServer->SendResponse(tTask->GetTaskId(),
													gAGHttpServer->ConstructFailDesc(tTask->GetTaskId(),"CstaCallFailed"));
					}
					
				}
				else if (tTask->GetTransferTaskType() == Make)
				{
					TsapiCommand_t *pNewCmd = new TsapiCommand_t();
					pNewCmd->activeDevId = tTask->GetTaskDevId();
					pNewCmd->activeCallId = pCstaUnse->u.failed.failedConnection.callID;
					pNewCmd->tsapiCmdType = TransferRelease;
					pNewCmd->invokeId = AGHelper::GenerateInvokeId(pNewCmd->activeDevId, TransferRelease);

					if (gCstaCmdThread != NULL)
					{
						gCstaCmdThread->AddTsapiCmdToQueue(pNewCmd);
					}
					if (gAGHttpServer != NULL)
					{
						gAGHttpServer->SendResponse(tTask->GetTaskId(),
													gAGHttpServer->ConstructFailDesc(tTask->GetTaskId(),"CstaCallFailed"));
					}
				}	
			}
		}
		break;
	case CSTA_HELD:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			AGTask *agTask = AGHelper::FindTaskByTaskDevRefId(refId, Transer);
			if (agTask != NULL)
			{
				TransferTask *tranTask = dynamic_cast<TransferTask*>(agTask);
				tranTask->SetHeldCallId(pCstaUnse->u.held.heldConnection.callID);	
			}

		}
		break;
	case CSTA_MONITOR_ENDED:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			Dev_t *dev = AGHelper::FindDevByRefId(refId);
			if (dev != NULL)
			{
				sslog<<"CstaEventProcess.ProcessCSTAUnsolicited devId:"<<
					   dev->devId<<" refId:"<<
					   refId<<std::endl;
				OUTERRORLOG(sslog.str());
				sslog.str("");

				if (gMonitorThread)
				{
					gMonitorThread->AddMonDevToMQ(dev);
				}
				AGHelper::RemoveMonDevByRefId(refId);
			}
		}
		break;
	case CSTA_NETWORK_REACHED:
		{

		}
		break;
	case CSTA_ORIGINATED:
		{
			
		}
		break;
	case CSTA_QUEUED:
		{

		}
		break;
	case CSTA_RETRIEVED:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			AGTask *agTask = AGHelper::FindTaskByTaskDevRefId(refId, Transer);
			if (agTask != NULL)
			{
				TransferTask *tranTask = dynamic_cast<TransferTask*>(agTask);
				tranTask->SetActiveCallId(pCstaUnse->u.retrieved.retrievedConnection.callID);
			}
		}
		break;
	case CSTA_SERVICE_INITIATED:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			AGTask *agTask = AGHelper::FindTaskByTaskDevRefId(refId, Distribute);
			if (agTask != NULL)
			{
				if (agTask->GetTaskCallId() == -1)
				{
					agTask->SetTaskCallId(pCstaUnse->u.serviceInitiated.initiatedConnection.callID);
				}
			}
			AGTask *tranTask = AGHelper::FindTaskByTaskDevRefId(refId, Transer);
			if (tranTask != NULL)
			{
				TransferTask *tTask = dynamic_cast<TransferTask*>(tranTask);
				tTask->SetActiveCallId(pCstaUnse->u.serviceInitiated.initiatedConnection.callID);
			}
		}
		break;
	case CSTA_TRANSFERRED:
		{
			long refId = pCstaUnse->monitorCrossRefId;
			AGTask *agTask = AGHelper::FindTaskByTaskDevRefId(refId,Transer);
			if (agTask != NULL)
			{
				if (agTask->GetTaskDevId() == pCstaUnse->u.transferred.transferringDevice.deviceID)
				{
					gAGHttpServer->SendResponse(agTask->GetTaskId(),
												gAGHttpServer->ConstructTranSuc(agTask->GetTaskId()));
					AGHelper::RemoveTaskByTaskId(agTask->GetTaskId(),Distribute);
					AGHelper::RemoveTaskByTaskId(agTask->GetTaskId(),Transer);
					AGHelper::SetIdleTaskDev(pCstaUnse->u.transferred.transferringDevice.deviceID);
				}
			}
			else
			{
			}

		}
		break;
	case CSTA_LOGGED_ON:
		{
			std::string devId = pCstaUnse->u.loggedOn.agentDevice.deviceID;
			std::string agentId = pCstaUnse->u.loggedOn.agentID;
			AGHelper::AddTerAgtToLM(devId, agentId);
		}
		break;
	case CSTA_LOGGED_OFF:
		{
			std::string devId = pCstaUnse->u.loggedOff.agentDevice.deviceID;
			AGHelper::RemoveTerAgtFromLM(devId);
		}
		break;
	default:
		break;
	}
}

void CstaEventProcess::ProcessCSTAEventReport(AvayaPbxEvent_t * pbxEventData)
{
	CheckEventData(pbxEventData);
	CSTAEvent_t *pcstaevent = (CSTAEvent_t *)(pbxEventData->cstaEvent);
	ATTEvent_t *pAttEvent   = (ATTEvent_t *)&(pbxEventData->attEvent);

	switch (pcstaevent->eventHeader.eventType)
	{
	case CSTA_SYS_STAT:
		{
			SystemStatus_t sst = pcstaevent->event.cstaEventReport.u.sysStat.systemStatus;
			switch (sst)
			{
			case SS_ENABLED:
				OUTERRORLOG("Link(Avaya AES & CM) is up");				
				break;
			case SS_NORMAL:
				OUTINFOLOG("Link(Avaya AES & CM) is normal");
				break;
			case SS_DISABLED:
				OUTERRORLOG("Link(Avaya AES & CM) is down start ts reconnect thread.");
				if (gCstaReconnectThd == NULL)
				{
					gCstaReconnectThd = new CstaReconnectThd();
				}
				gCstaReconnectThd->StartCstaReconnect();
				break;
			default:
				break;
			}
		}
		break;
	case CSTA_SYS_STAT_ENDED:
		break;
	case CSTA_PRIVATE:
		break;
	default:
		break;
	}
}

void CstaEventProcess::WriteCSTAUnsolicited(AvayaPbxEvent_t *pbxEventData)
{
	CheckEventData(pbxEventData);
	std::stringstream sslog;
	sslog.str("");
	try
	{
		CSTAEvent_t *pcstaevent = (CSTAEvent_t *)(pbxEventData->cstaEvent);
		ATTEvent_t *pAttevent   = (ATTEvent_t *)&(pbxEventData->attEvent);
		CSTAUnsolicitedEvent *pCstaUnse;
		pCstaUnse = &(pcstaevent->event.cstaUnsolicited);

		sslog<<"--------------------------------monitor refid:"<<pCstaUnse->monitorCrossRefId<<"---------------------------------\r\n"<<std::endl;
		switch (pcstaevent->eventHeader.eventType)
		{
		case CSTA_MONITOR_ENDED:
			{
				sslog<<"CSTA_MONITOR_ENDED"<<"\r\n"<<"monitor reference id:"<<
						pCstaUnse->monitorCrossRefId<<" cause"<<
						pCstaUnse->u.monitorEnded.cause<<std::endl;
			}
			break;
		case CSTA_CALL_CLEARED:
			{
				sslog<<"CSTA_CALL_CLEARED"<<"\r\n"<<"clear call callid:"<<
						pCstaUnse->u.callCleared.clearedCall.callID<<" deviceid:"<<
						pCstaUnse->u.callCleared.clearedCall.deviceID<<"\r\n"<<"local call info:"<<
						pCstaUnse->u.callCleared.localConnectionInfo<<"\r\n"<<"cause:"<<
						pCstaUnse->u.callCleared.cause<<std::endl;
				if ((pbxEventData->privateDataLength > 0) && (pbxEventData->attEvent.eventType == ATT_CALL_CLEARED ))
				{
					sslog<<"---------private data-----------"<<"\r\n"<<"private reason:"<<
							pbxEventData->attEvent.u.callClearedEvent.reason;

				}
				sslog<<std::endl;
			}
			break;
		case CSTA_CONFERENCED:
			{
				sslog<<"CSTA_CONFERENCED"<<"\r\n"<<"primary call id:"<<
						pCstaUnse->u.conferenced.primaryOldCall.callID<<" priDevid:"<<
						pCstaUnse->u.conferenced.primaryOldCall.deviceID<<" pridev type:"<<
						pCstaUnse->u.conferenced.primaryOldCall.devIDType<<"\r\n"<<"second call id:"<<
						pCstaUnse->u.conferenced.secondaryOldCall.callID<<" secDevid:"<<
						pCstaUnse->u.conferenced.secondaryOldCall.deviceID<<" secDev type:"<<
						pCstaUnse->u.conferenced.secondaryOldCall.devIDType<<"\r\n"<<"control conference devid:"<<
						pCstaUnse->u.conferenced.confController.deviceID<<" add party devid:"<<
						pCstaUnse->u.conferenced.addedParty.deviceID<<"\r\nlocal call info:"<<
						pCstaUnse->u.conferenced.localConnectionInfo<<"\r\ncause:"<<
						pCstaUnse->u.conferenced.cause<<"\r\nconnection list :"<<std::endl;

				int conncount = pCstaUnse->u.conferenced.conferenceConnections.count;
				for (int i = 1; i <= conncount; ++i)
				{
					sslog<<" {connection"<<
							i<<", connid:"<<
							pCstaUnse->u.conferenced.conferenceConnections.connection[i-1].party.callID<<" devid:"<<
							pCstaUnse->u.conferenced.conferenceConnections.connection[i-1].party.deviceID<<" devtype:"<<
							pCstaUnse->u.conferenced.conferenceConnections.connection[i-1].party.devIDType<<"}\r\n"<<std::endl;
				}

				if (pbxEventData->privateDataLength > 0)
				{
					sslog<<"---------private data-----------"<<"\r\n"<<"ori call reason:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.reason<<"\r\n"<<"ori call calling devid:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.callingDevice.deviceID<<" ori call calling devtype:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.callingDevice.deviceIDType<<" ori call calling devstatus:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.callingDevice.deviceIDStatus<<"\r\n"<<"ori call called devid"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.calledDevice.deviceID<<" ori call called devtype:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.calledDevice.deviceIDType<<" ori called devstatus:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.calledDevice.deviceIDStatus<<"\r\n"<<" ori call trunk group:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.trunkGroup<<" ori call trunk member:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.trunkMember<<"\r\n"<<"ori call ucid:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.ucid<<"\r\nori call userinfo data legth:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.userInfo.data.length<<" ori call userinfo data value:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.userInfo.data.value<<" ori call userinfo data type:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.userInfo.type<<"\r\nori call loocaheadinfo hours:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.lookaheadInfo.hours<<" ori call loocaheadinfo minutes:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.lookaheadInfo.minutes<<" ori call loocaheadinfo proority:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.lookaheadInfo.priority<<" ori call loocaheadinfo seconds:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.lookaheadInfo.seconds<<" ori call loocaheadinfo sourceVdn:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.lookaheadInfo.sourceVDN<<"\r\nori call user enter code type:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.userEnteredCode.type<<" ori call user enter code data:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.userEnteredCode.data<<" ori call user enter code indicator:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.userEnteredCode.indicator<<" ori call user enter code type:"<<
							pbxEventData->attEvent.u.conferencedEvent.originalCallInfo.userEnteredCode.collectVDN<<"\r\ndistributing collectVDN:"<<
							pbxEventData->attEvent.u.conferencedEvent.distributingDevice.deviceID<<" distributing devtype:"<<
							pbxEventData->attEvent.u.conferencedEvent.distributingDevice.deviceIDType<<" distributing devstatus:"<<
							pbxEventData->attEvent.u.conferencedEvent.distributingDevice.deviceIDStatus<<"\r\ndistributing vdn vdnid:"<<
							pbxEventData->attEvent.u.conferencedEvent.distributingVDN.deviceID<<" distributing vdn devtype:"<<
							pbxEventData->attEvent.u.conferencedEvent.distributingVDN.deviceIDType<<" distributing vdn devstatus:"<<
							pbxEventData->attEvent.u.conferencedEvent.distributingVDN.deviceIDStatus<<"\r\n new ucid:"<<
							pbxEventData->attEvent.u.conferencedEvent.ucid<<std::endl;

				}
			}
			break;
		case CSTA_CONNECTION_CLEARED:
			{
				sslog<<"CSTA_CONNECTION_CLEARED"<<"\r\n"<<"drop connnection id:"<<
						pCstaUnse->u.connectionCleared.droppedConnection.callID<<" drop connection devid:"<<
						pCstaUnse->u.connectionCleared.droppedConnection.deviceID<<" drop connection devtype:"<<
						pCstaUnse->u.connectionCleared.droppedConnection.devIDType<<"\r\n releaseing devid:"<<
						pCstaUnse->u.connectionCleared.releasingDevice.deviceID<<" releasing devtype:"<<
						pCstaUnse->u.connectionCleared.releasingDevice.deviceIDType<<" releasing devstatus:"<<
						pCstaUnse->u.connectionCleared.releasingDevice.deviceIDStatus<<"\r\nlocal connection info:"<<
						pCstaUnse->u.connectionCleared.localConnectionInfo<<"\r\ncause:"<<
						pCstaUnse->u.connectionCleared.cause<<std::endl;
				if (pbxEventData->privateDataLength > 0 )
				{
					sslog<<"--------private data---------"<<"\r\nuserinfo length:"<<
							pbxEventData->attEvent.u.connectionClearedEvent.userInfo.data.length<<" userinfo data:"<<
							pbxEventData->attEvent.u.connectionClearedEvent.userInfo.data.value<<" userinfo type:"<<
							pbxEventData->attEvent.u.connectionClearedEvent.userInfo.type<<std::endl;
				}


			}
			break;
		case CSTA_DIVERTED:
			{
				sslog<<"CSTA_DIVERTED"<<"\r\n"<<"connection id:"<<
						pCstaUnse->u.diverted.connection.callID<<" connection devid:"<<
						pCstaUnse->u.diverted.connection.deviceID<<" connection devtype:"<<
						pCstaUnse->u.diverted.connection.devIDType<<"\r\ndiverting devid:"<<
						pCstaUnse->u.diverted.divertingDevice.deviceID<<" diverting devtype:"<<
						pCstaUnse->u.diverted.divertingDevice.deviceIDType<<" diverting devstatus:"<<
						pCstaUnse->u.diverted.divertingDevice.deviceIDStatus<<"\r\nlocal connection info:"<<
						pCstaUnse->u.diverted.localConnectionInfo<<"\r\ndestination devid:"<<
						pCstaUnse->u.diverted.newDestination.deviceID<<" destination devtype:"<<
						pCstaUnse->u.diverted.newDestination.deviceIDType<<" destiination devstatus:"<<
						pCstaUnse->u.diverted.newDestination.deviceIDStatus<<"\r\ncause:"<<
						pCstaUnse->u.diverted.cause<<std::endl;
			}
			break;
		case CSTA_DELIVERED:
			{
				sslog<<"CSTA_DELIVERED"<<"\r\nconnection id:"<<
						pCstaUnse->u.delivered.connection.callID<<" connection devid:"<<
						pCstaUnse->u.delivered.connection.deviceID<<" connection devtype:"<<
						pCstaUnse->u.delivered.connection.devIDType<<"\r\nalerting devid:"<<
						pCstaUnse->u.delivered.alertingDevice.deviceID<<" alerting devtype:"<<
						pCstaUnse->u.delivered.alertingDevice.deviceIDType<<" alerting devstatus:"<<
						pCstaUnse->u.delivered.alertingDevice.deviceIDStatus<<"\r\ncalling devid:"<<
						pCstaUnse->u.delivered.callingDevice.deviceID<<"calling devtype:"<<
						pCstaUnse->u.delivered.callingDevice.deviceIDType<<" calling devstatus:"<<
						pCstaUnse->u.delivered.callingDevice.deviceIDStatus<<"\r\ncalled devid:"<<
						pCstaUnse->u.delivered.calledDevice.deviceID<<" called devtype:"<<
						pCstaUnse->u.delivered.calledDevice.deviceIDType<<" called devstatus:"<<
						pCstaUnse->u.delivered.calledDevice.deviceIDStatus<<"\r\nlast redirect devid:"<<
						pCstaUnse->u.delivered.lastRedirectionDevice.deviceID<<" last redirect devtype:"<<
						pCstaUnse->u.delivered.lastRedirectionDevice.deviceIDType<<" last redirect devstatus:"<<
						pCstaUnse->u.delivered.lastRedirectionDevice.deviceIDStatus<<"\r\nlocal con info:"<<
						pCstaUnse->u.delivered.localConnectionInfo<<"\r\ncause:"<<
						pCstaUnse->u.delivered.cause<<std::endl;

				if (pbxEventData->privateDataLength > 0 )
				{
					sslog<<"--------private data------------\r\n"<<"ori call calling devid:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.callingDevice.deviceID<<" ori call calling devtype:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.callingDevice.deviceIDType<<" ori call calling devstatus:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.callingDevice.deviceIDStatus<<"\r\nori call called devid:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.calledDevice.deviceID<<" ori call called devtype:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.calledDevice.deviceIDType<<" ori call called devstatus:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.calledDevice.deviceIDStatus<<"\r\nori call call ori type:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.callOriginatorInfo.callOriginatorType<<"\r\nori call trunk group:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.trunkGroup<<" trunk member:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.trunkMember<<"\r\nucid:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.ucid<<"\r\nuser enter code data:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.userEnteredCode.data<<" type:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.userEnteredCode.type<<" indicator:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.userEnteredCode.indicator<<" collectVDN:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.userEnteredCode.collectVDN<<"\r\n user info data length:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.userInfo.data.length<<" value:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.userInfo.data.value<<" type:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.userInfo.type<<"\r\n locckageadinfo hours:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.lookaheadInfo.hours<<" minutes:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.lookaheadInfo.minutes<<" priority:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.lookaheadInfo.priority<<" seconds:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.lookaheadInfo.seconds<<" sourceVdn:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.lookaheadInfo.sourceVDN<<" \r\n reason:"<<
							pAttevent->u.deliveredEvent.originalCallInfo.reason<<"\r\n delivered type:"<<
							pAttevent->u.deliveredEvent.deliveredType<<"\r\ndustributing devid:"<<
							pAttevent->u.deliveredEvent.distributingDevice.deviceID<<" devtype:"<<
							pAttevent->u.deliveredEvent.distributingDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.deliveredEvent.distributingDevice.deviceIDStatus<<"\r\ndistributing vdnid::"<<
							pAttevent->u.deliveredEvent.distributingVDN.deviceID<<" devtype:"<<
							pAttevent->u.deliveredEvent.distributingVDN.deviceIDType<<" devstatus:"<<
							pAttevent->u.deliveredEvent.distributingVDN.deviceIDStatus<<"\r\n lookaheadInfo hours:"<<
							pAttevent->u.deliveredEvent.lookaheadInfo.hours<<" minutes:"<<
							pAttevent->u.deliveredEvent.lookaheadInfo.minutes<<" priority:"<<
							pAttevent->u.deliveredEvent.lookaheadInfo.priority<<" seconds:"<<
							pAttevent->u.deliveredEvent.lookaheadInfo.seconds<<" sourceVdn:"<<
							pAttevent->u.deliveredEvent.lookaheadInfo.sourceVDN<<"\r\n reason:"<<
							pAttevent->u.deliveredEvent.reason<<"\r\n split:"<<
							pAttevent->u.deliveredEvent.split<<"\r\n trunkgroup:"<<
							pAttevent->u.deliveredEvent.trunkGroup<<" trunkmember:"<<
							pAttevent->u.deliveredEvent.trunkMember<<"\r\n ucid:"<<
							pAttevent->u.deliveredEvent.ucid<<"\r\n userinfo data length:"<<
							pAttevent->u.deliveredEvent.userInfo.data.length<<" value:"<<
							pAttevent->u.deliveredEvent.userInfo.data.value<<" type:"<<
							pAttevent->u.deliveredEvent.userInfo.type<<"\r\n user enter code date:"<<
							pAttevent->u.deliveredEvent.userEnteredCode.data<<" type:"<<
							pAttevent->u.deliveredEvent.userEnteredCode.type<<" indicator:"<<
							pAttevent->u.deliveredEvent.userEnteredCode.indicator<<" collectVDN:"<<
							pAttevent->u.deliveredEvent.userEnteredCode.collectVDN<<std::endl;
				}
			}
			break;
		case CSTA_ESTABLISHED:
			{
				sslog<<"CSTA_ESTABLISHED"<<"\r\n"<<"establisged connection callid:"<<
						pCstaUnse->u.established.establishedConnection.callID<<" devid:"<<
						pCstaUnse->u.established.establishedConnection.deviceID<<" devtype:"<<
						pCstaUnse->u.established.establishedConnection.devIDType<<"\r\nanswering devid:"<<
						pCstaUnse->u.established.answeringDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.established.answeringDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.established.answeringDevice.deviceIDStatus<<"\r\ncalling deviceid:"<<
						pCstaUnse->u.established.callingDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.established.callingDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.established.callingDevice.deviceIDStatus<<"\r\ncalled deviceid:"<<
						pCstaUnse->u.established.calledDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.established.calledDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.established.calledDevice.deviceIDStatus<<"\r\nlast redirect devid:"<<
						pCstaUnse->u.established.lastRedirectionDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.established.lastRedirectionDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.established.lastRedirectionDevice.deviceIDStatus<<"\r\nlocal connectioninfo:"<<
						pCstaUnse->u.established.localConnectionInfo<<"\r\ncause:"<<
						pCstaUnse->u.established.cause<<std::endl;

				if (pbxEventData->privateDataLength > 0)
				{
					sslog<<"----------private data------------\r\n"<<"ori call info calling devid:"<<
							pAttevent->u.establishedEvent.originalCallInfo.callingDevice.deviceID<<" devtype:"<<
							pAttevent->u.establishedEvent.originalCallInfo.callingDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.establishedEvent.originalCallInfo.callingDevice.deviceIDStatus<<"\r\n"<<"ori call info called devid:"<<
							pAttevent->u.establishedEvent.originalCallInfo.calledDevice.deviceID<<" devtype:"<<
							pAttevent->u.establishedEvent.originalCallInfo.calledDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.establishedEvent.originalCallInfo.calledDevice.deviceIDStatus<<"\r\n"<<"ori call info calloriinfo type:"<<
							pAttevent->u.establishedEvent.originalCallInfo.callOriginatorInfo.callOriginatorType<<"\r\n"<<"ori call info lookaheadinfo hours:"<<
							pAttevent->u.establishedEvent.originalCallInfo.lookaheadInfo.hours<<" minutes:"<<
							pAttevent->u.establishedEvent.originalCallInfo.lookaheadInfo.minutes<<" priority:"<<
							pAttevent->u.establishedEvent.originalCallInfo.lookaheadInfo.priority<<" seconds:"<<
							pAttevent->u.establishedEvent.originalCallInfo.lookaheadInfo.seconds<<" sourcevdn:"<<
							pAttevent->u.establishedEvent.originalCallInfo.lookaheadInfo.sourceVDN<<"\r\n"<<"ori call info reason:"<<
							pAttevent->u.establishedEvent.originalCallInfo.reason<<"\r\n"<<"ori call info ucid:"<<
							pAttevent->u.establishedEvent.originalCallInfo.ucid<<"\r\n"<<" ori call info userentercode data:"<<
							pAttevent->u.establishedEvent.originalCallInfo.userEnteredCode.data<<" type:"<<
							pAttevent->u.establishedEvent.originalCallInfo.userEnteredCode.type<<" indicator:"<<
							pAttevent->u.establishedEvent.originalCallInfo.userEnteredCode.indicator<<" collectvdn:"<<
							pAttevent->u.establishedEvent.originalCallInfo.userEnteredCode.collectVDN<<"\r\n"<<"ori call info trunk group:"<<
							pAttevent->u.establishedEvent.originalCallInfo.trunkGroup<<" trunk member:"<<
							pAttevent->u.establishedEvent.originalCallInfo.trunkMember<<"\r\n"<<"ori call info userinfo length:"<<
							pAttevent->u.establishedEvent.originalCallInfo.userInfo.data.length<<" value:"<<
							pAttevent->u.establishedEvent.originalCallInfo.userInfo.data.value<<" type:"<<
							pAttevent->u.establishedEvent.originalCallInfo.userInfo.type<<"\r\ndistributing devid:"<<
							pAttevent->u.establishedEvent.distributingDevice.deviceID<<" devtype:"<<
							pAttevent->u.establishedEvent.distributingDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.establishedEvent.distributingDevice.deviceIDStatus<<"\r\ndistributing vdn id:"<<
							pAttevent->u.establishedEvent.distributingVDN.deviceID<<" devtype:"<<
							pAttevent->u.establishedEvent.distributingVDN.deviceIDType<<" devstatus:"<<
							pAttevent->u.establishedEvent.distributingVDN.deviceIDStatus<<"\r\nlookaheadinfo hours:"<<
							pAttevent->u.establishedEvent.lookaheadInfo.hours<<" minutes:"<<
							pAttevent->u.establishedEvent.lookaheadInfo.minutes<<" priority:"<<
							pAttevent->u.establishedEvent.lookaheadInfo.priority<<" seconds:"<<
							pAttevent->u.establishedEvent.lookaheadInfo.seconds<<" sourceVdn:"<<
							pAttevent->u.establishedEvent.lookaheadInfo.sourceVDN<<"\r\nreason:"<<
							pAttevent->u.establishedEvent.reason<<"\r\nsplit:"<<
							pAttevent->u.establishedEvent.split<<"\r\ntrunk group:"<<
							pAttevent->u.establishedEvent.trunkGroup<<" trunk member:"<<
							pAttevent->u.establishedEvent.trunkMember<<"\r\n ucid:"<<
							pAttevent->u.establishedEvent.ucid<<"\r\n user info length:"<<
							pAttevent->u.establishedEvent.userInfo.data.length<<" value:"<<
							pAttevent->u.establishedEvent.userInfo.data.value<<" type:"<<
							pAttevent->u.establishedEvent.userInfo.type<<"\r\n userentercode data:"<<
							pAttevent->u.establishedEvent.userEnteredCode.data<<" type:"<<
							pAttevent->u.establishedEvent.userEnteredCode.type<<" indicator:"<<
							pAttevent->u.establishedEvent.userEnteredCode.indicator<<" collectVDN:"<<
							pAttevent->u.establishedEvent.userEnteredCode.collectVDN<<std::endl;

				}

			}
			break;
		case CSTA_FAILED:
			{
				sslog<<"CSTA_FAILED\r\n"<<"failed connection id:"<<
						pCstaUnse->u.failed.failedConnection.callID<<" devid:"<<
						pCstaUnse->u.failed.failedConnection.deviceID<<" devtype:"<<
						pCstaUnse->u.failed.failedConnection.devIDType<<"\r\nfailing devid:"<<
						pCstaUnse->u.failed.failingDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.failed.failingDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.failed.failingDevice.deviceIDStatus<<"\r\ncalled devid:"<<
						pCstaUnse->u.failed.calledDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.failed.calledDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.failed.calledDevice.deviceIDStatus<<"\r\ncause:"<<
						pCstaUnse->u.failed.cause<<"\r\nlocal caonnection info:"<<
						pCstaUnse->u.failed.localConnectionInfo<<std::endl;
				if(pbxEventData->privateDataLength > 0)
				{
					sslog<<"-----------------private data-------------------\r\n"<<"calling devid:"<<
							pAttevent->u.failedEvent.callingDevice.deviceID<<" devtype:"<<
							pAttevent->u.failedEvent.callingDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.failedEvent.callingDevice.deviceIDStatus<<std::endl;
				}
			}
			break;
		case CSTA_HELD:
			{
				sslog<<"CSTA_HELD\r\n"<<"held connection id:"<<
						pCstaUnse->u.held.heldConnection.callID<<" devid:"<<
						pCstaUnse->u.held.heldConnection.deviceID<<" devtype:"<<
						pCstaUnse->u.held.heldConnection.devIDType<<"\r\n holding devid:"<<
						pCstaUnse->u.held.holdingDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.held.holdingDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.held.holdingDevice.deviceIDStatus<<"\r\ncause:"<<
						pCstaUnse->u.held.cause<<"\r\nlocal call info:"<<
						pCstaUnse->u.held.localConnectionInfo<<std::endl;

			}
			break;
		case CSTA_NETWORK_REACHED:
			{
				sslog<<"CSTA_NETWORK_REACHED\r\n"<<"networkreached connid:"<<
						pCstaUnse->u.networkReached.connection.callID<<" devid:"<<
						pCstaUnse->u.networkReached.connection.deviceID<<" devtype:"<<
						pCstaUnse->u.networkReached.connection.devIDType<<"\r\ncalled devid:"<<
						pCstaUnse->u.networkReached.calledDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.networkReached.calledDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.networkReached.calledDevice.deviceIDStatus<<"\r\ncause:"<<
						pCstaUnse->u.networkReached.cause<<"\r\ntrunk usedid:"<<
						pCstaUnse->u.networkReached.trunkUsed.deviceID<<" devtype:"<<
						pCstaUnse->u.networkReached.trunkUsed.deviceIDType<<" dev status:"<<
						pCstaUnse->u.networkReached.trunkUsed.deviceIDStatus<<"\r\nlocal call info:"<<
						pCstaUnse->u.networkReached.localConnectionInfo<<std::endl;

				if(pbxEventData->privateDataLength > 0)
				{
					sslog<<"----------private data-------------"<<"\r\nprogressDescription"<<
							pAttevent->u.networkReachedEvent.progressDescription<<"\r\nprogressLocation"<<
							pAttevent->u.networkReachedEvent.progressLocation<<"\r\ntrunk group:"<<
							pAttevent->u.networkReachedEvent.trunkGroup<<" trunk member:"<<
							pAttevent->u.networkReachedEvent.trunkMember<<std::endl;
				}
			}
			break;
		case CSTA_ORIGINATED:
			{
				sslog<<"CSTA_ORIGINATED\r\n"<<"originated connection id:"<<
						pCstaUnse->u.originated.originatedConnection.callID<<" devid:"<<
						pCstaUnse->u.originated.originatedConnection.deviceID<<" devtype:"<<
						pCstaUnse->u.originated.originatedConnection.devIDType<<"\r\ncalling devid:"<<
						pCstaUnse->u.originated.callingDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.originated.callingDevice.deviceIDStatus<<"\r\ncalled devid:"<<
						pCstaUnse->u.originated.calledDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.originated.calledDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.originated.calledDevice.deviceIDStatus<<"\r\ncause:"<<
						pCstaUnse->u.originated.cause<<"\r\nlocal call info:"<<
						pCstaUnse->u.originated.localConnectionInfo<<std::endl;

				if(pbxEventData->privateDataLength > 0)
				{
					sslog<<"----------private data----------------\r\n"<<"localagent:"<<
							pAttevent->u.originatedEvent.logicalAgent<<"\r\n user info length:"<<
							pAttevent->u.originatedEvent.userInfo.data.length<<"  value:"<<
							pAttevent->u.originatedEvent.userInfo.data.value<<" type:"<<
							pAttevent->u.originatedEvent.userInfo.type<<std::endl;
				}
			}
			break;
		case CSTA_QUEUED:
			{
				sslog<<"CSTA_QUEUED\r\n"<<"queue connection id:"<<
						pCstaUnse->u.queued.queuedConnection.callID<<" devid:"<<
						pCstaUnse->u.queued.queuedConnection.deviceID<<" devtype:"<<
						pCstaUnse->u.queued.queuedConnection.devIDType<<"\r\r queue id:"<<
						pCstaUnse->u.queued.queue.deviceID<<" devtype:"<<
						pCstaUnse->u.queued.queue.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.queued.queue.deviceIDStatus<<"\r\n numberqueued:"<<
						pCstaUnse->u.queued.numberQueued<<"\r\n calling device id:"<<
						pCstaUnse->u.queued.callingDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.queued.callingDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.queued.callingDevice.deviceIDStatus<<"\r\n called device id:"<<
						pCstaUnse->u.queued.calledDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.queued.calledDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.queued.calledDevice.deviceIDStatus<<"\r\n last redirect device id:"<<
						pCstaUnse->u.queued.lastRedirectionDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.queued.lastRedirectionDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.queued.lastRedirectionDevice.deviceIDStatus<<"\r\n cause:"<<
						pCstaUnse->u.queued.cause<<"\r\n local caonnnection info:"<<
						pCstaUnse->u.queued.localConnectionInfo<<std::endl;

			}
			break;
		case CSTA_RETRIEVED:
			{
				sslog<<"CSTA_RETRIEVED\r\n"<<"retrieve connection id:"<<
						pCstaUnse->u.retrieved.retrievedConnection.callID<<" devid:"<<
						pCstaUnse->u.retrieved.retrievedConnection.deviceID<<" devtype:"<<
						pCstaUnse->u.retrieved.retrievedConnection.devIDType<<"\r\n retrieving device id:"<<
						pCstaUnse->u.retrieved.retrievingDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.retrieved.retrievingDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.retrieved.retrievingDevice.deviceIDStatus<<"\r\ncause:"<<
						pCstaUnse->u.retrieved.cause<<"\r\n local connection info:"<<
						pCstaUnse->u.retrieved.localConnectionInfo<<std::endl;

			}
			break;
		case CSTA_SERVICE_INITIATED:
			{
				sslog<<"CSTA_SERVICE_INITIATED\r\n"<<"initiated connection id:"<<
						pCstaUnse->u.serviceInitiated.initiatedConnection.callID<<" devid:"<<
						pCstaUnse->u.serviceInitiated.initiatedConnection.deviceID<<" devtype:"<<
						pCstaUnse->u.serviceInitiated.initiatedConnection.devIDType<<"\r\ncause:"<<
						pCstaUnse->u.serviceInitiated.cause<<"\r\nlocal connection info:"<<
						pCstaUnse->u.serviceInitiated.localConnectionInfo<<std::endl;

				if(pbxEventData->privateDataLength > 0)
				{
					sslog<<"----------private data--------------\r\n"<<"ucid:"<<
							pAttevent->u.serviceInitiated.ucid<<std::endl;
				}
			}
			break;
		case CSTA_TRANSFERRED:
			{
				sslog<<"CSTA_TRANSFERRED\r\n"<<"primary call id:"<<
						pCstaUnse->u.transferred.primaryOldCall.callID<<" devid:"<<
						pCstaUnse->u.transferred.primaryOldCall.deviceID<<" devtype:"<<
						pCstaUnse->u.transferred.primaryOldCall.devIDType<<"\r\nsecond call id:"<<
						pCstaUnse->u.transferred.secondaryOldCall.callID<<" devid:"<<
						pCstaUnse->u.transferred.secondaryOldCall.deviceID<<" devtype:"<<
						pCstaUnse->u.transferred.secondaryOldCall.devIDType<<"\r\ntransfering device id:"<<
						pCstaUnse->u.transferred.transferringDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.transferred.transferringDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.transferred.transferringDevice.deviceIDStatus<<"\r\ntransfered device id:"<<
						pCstaUnse->u.transferred.transferredDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.transferred.transferredDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.transferred.transferredDevice.deviceIDStatus<<"\r\ntransfer connections list:"<<std::endl;

				for(unsigned int i = 0; i < pCstaUnse->u.transferred.transferredConnections.count; ++i)
				{
					sslog<<" {connection"<<
							(i+1)<<", connid:"<<
							pCstaUnse->u.transferred.transferredConnections.connection[i].party.callID<<" devid:"<<
							pCstaUnse->u.transferred.transferredConnections.connection[i].party.deviceID<<" devtype:"<<
							pCstaUnse->u.transferred.transferredConnections.connection[i].party.devIDType<<"}\r\n"<<std::endl;				
				}

				sslog<<"cause"<<
						pCstaUnse->u.transferred.cause<<" local connection info:"<<
						pCstaUnse->u.transferred.localConnectionInfo<<std::endl;

				if(pbxEventData->privateDataLength > 0)
				{
					sslog<<"-------------private data--------------\r\n"<<"ori call info calling device id:"<<
							pAttevent->u.transferredEvent.originalCallInfo.callingDevice.deviceID<<" devtype:"<<
							pAttevent->u.transferredEvent.originalCallInfo.callingDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.transferredEvent.originalCallInfo.callingDevice.deviceIDStatus<<"\r\nori call info called device id:"<<
							pAttevent->u.transferredEvent.originalCallInfo.calledDevice.deviceID<<" devtype:"<<
							pAttevent->u.transferredEvent.originalCallInfo.calledDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.transferredEvent.originalCallInfo.calledDevice.deviceIDStatus<<"\r\nori call info trunk group:"<<
							pAttevent->u.transferredEvent.originalCallInfo.trunkGroup<<" trunk member:"<<
							pAttevent->u.transferredEvent.originalCallInfo.trunkMember<<"\r\n lookaheadinfo hours:"<<
							pAttevent->u.transferredEvent.originalCallInfo.lookaheadInfo.hours<<" minutes:"<<
							pAttevent->u.transferredEvent.originalCallInfo.lookaheadInfo.minutes<<" priority:"<<
							pAttevent->u.transferredEvent.originalCallInfo.lookaheadInfo.priority<<" seconds:"<<
							pAttevent->u.transferredEvent.originalCallInfo.lookaheadInfo.seconds<<" sourceVdn:"<<
							pAttevent->u.transferredEvent.originalCallInfo.lookaheadInfo.sourceVDN<<"\r\nori call info:reason:"<<
							pAttevent->u.transferredEvent.originalCallInfo.reason<<"\r\nori call info ucid:"<<
							pAttevent->u.transferredEvent.originalCallInfo.ucid<<"\r\nori call info user enter code data:"<<
							pAttevent->u.transferredEvent.originalCallInfo.userEnteredCode.data<<" type:"<<
							pAttevent->u.transferredEvent.originalCallInfo.userEnteredCode.type<<" indicator:"<<
							pAttevent->u.transferredEvent.originalCallInfo.userEnteredCode.indicator<<" collectVdn:"<<
							pAttevent->u.transferredEvent.originalCallInfo.userEnteredCode.collectVDN<<"\r\nori call info user info legtn:"<<
							pAttevent->u.transferredEvent.originalCallInfo.userInfo.data.length<<" value:"<<
							pAttevent->u.transferredEvent.originalCallInfo.userInfo.data.value<<" type:"<<
							pAttevent->u.transferredEvent.originalCallInfo.userInfo.type<<"\r\ncallOriginatorInfo type:"<<
							pAttevent->u.transferredEvent.originalCallInfo.callOriginatorInfo.callOriginatorType<<"\r\ndistrbuting device id:"<<
							pAttevent->u.transferredEvent.distributingDevice.deviceID<<" devtype:"<<
							pAttevent->u.transferredEvent.distributingDevice.deviceIDType<<" devstatus:"<<
							pAttevent->u.transferredEvent.distributingDevice.deviceIDStatus<<"\r\ndistributing vdn id:"<<
							pAttevent->u.transferredEvent.distributingVDN.deviceID<<" devtype:"<<
							pAttevent->u.transferredEvent.distributingVDN.deviceIDType<<" devstatus:"<<
							pAttevent->u.transferredEvent.distributingVDN.deviceIDStatus<<"\r\nucid:"<<
							pAttevent->u.transferredEvent.ucid<<std::endl;					
				}
			}
			break;
		case CSTA_LOGGED_OFF:
			{
				sslog<<"CSTA_LOGGED_OFF\r\n"<<" agentid:"<<
						pCstaUnse->u.loggedOff.agentID<<"\r\nagent deviceid:"<<
						pCstaUnse->u.loggedOff.agentDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.loggedOff.agentDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.loggedOff.agentDevice.deviceIDStatus<<"\r\nagent group:"<<
						pCstaUnse->u.loggedOff.agentGroup<<std::endl;

				if(pbxEventData->privateDataLength > 0)
				{
					sslog<<"\r\n-----------private data---------------\r\n"<<"logoff reason code:"<<
						pAttevent->u.loggedOffEvent.reasonCode<<std::endl;
				}
			}
			break;
		case CSTA_LOGGED_ON:
			{
				sslog<<"CSTA_LOGGED_ON\r\n"<<" agentid:"<<
						pCstaUnse->u.loggedOn.agentID<<"\r\nagent deviceid:"<<
						pCstaUnse->u.loggedOn.agentDevice.deviceID<<" devtype:"<<
						pCstaUnse->u.loggedOn.agentDevice.deviceIDType<<" devstatus:"<<
						pCstaUnse->u.loggedOn.agentDevice.deviceIDStatus<<"\r\nagent group:"<<
						pCstaUnse->u.loggedOn.agentGroup<<std::endl;
				if(pbxEventData->privateDataLength > 0)
				{
					sslog<<"---------private data-----------\r\n"<<"workmode:"<<
							pAttevent->u.loggedOnEvent.workMode<<std::endl;
				}
			}
			break;
		default:
			{
				sslog<<"\r\nUNKONWN_CSTA_Unsolicited\r\n"<<"eventtype:"<<
					pcstaevent->eventHeader.eventType<<std::endl;

			}
			break;
		}

		OUTDEBUGLOG(sslog.str());
	}
	catch(...)
	{
		OUTERRORLOG("-------------------TsapiEventProcess.WriteCSTAUnsolicited exception---------------------------");
	}
}