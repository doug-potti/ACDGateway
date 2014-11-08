#ifndef ACDGW_TASK_H
#define ACDGW_TASK_H

#include <string>

enum EnTaskType
{
	Distribute,
	CancelDistribute,
	Transer,
	CancelTransfer,
};
enum EnTransferTaskType
{
	Make,
	Consult,
};
class  AGTask
{
public:
	AGTask(){}
	AGTask(std::string taskid,EnTaskType tskType):taskId(taskid),taskType(tskType),taskCallId(-1),monRefId(-1)
	{
		taskDevId = "";
	}
	virtual ~AGTask(){}
	virtual void ExcuteTask() = 0;

	std::string GetTaskId()
	{
		return taskId;
	}
	EnTaskType GetTaskType()
	{
		return taskType;
	}

	void SetRefId(long taskRefId)
	{
		monRefId = taskRefId;
	}

	void SetTaskDevId(std::string devId)
	{
		taskDevId = devId;
	}

	void SetTaskCallId(long callId)
	{
		taskCallId = callId;
	}

	std::string GetTaskDevId()
	{
		return taskDevId;
	}

	long GetTaskRefId()
	{
		return monRefId;
	}

	long GetTaskCallId()
	{
		return taskCallId;
	}

	long GetMonRfId()
	{
		return monRefId;
	}


protected:
	EnTaskType  taskType;
	std::string taskId;
	long        monRefId;
	long        taskCallId;
	std::string taskDevId;
};

class DistributeAgentTask:public AGTask
{
public:
	DistributeAgentTask(std::string tskId,
		                std::string medType,
						std::string busType,
						std::string custLevel,
						std::string dialno):AGTask(tskId,Distribute),mediaType(medType),businessType(busType), 
						                    customerLevel(custLevel),dialNo(dialno),agentId("")
	{}
	~DistributeAgentTask(){}
	void ExcuteTask();

	void SetTaskDevId(std::string devId)
	{
		taskDevId = devId;
	}

	void SetDialNo(std::string dialno)
	{
		dialNo = dialno;
	}

	void SetAgentId(std::string agtId)
	{
		agentId = agtId;
	}

	std::string GetAgentId()
	{
		return agentId;
	}
	std::string GetDialNo()
	{
		return dialNo;
	}
	std::string GetMediaType()
	{
		return mediaType;
	}
	std::string GetBusType()
	{
		return businessType;
	}

	std::string GetCustLvl()
	{
		return customerLevel;
	}
	
private:
	std::string GetUui()
	{
		return "";
	}

private:
	std::string mediaType;
	std::string businessType;
	std::string customerLevel;
	std::string dialNo;
	std::string agentId;
};

class CancelDisAgtTask:public AGTask
{
public:
	CancelDisAgtTask(std::string tskId):AGTask(tskId,CancelDistribute)
	{

	}
	~CancelDisAgtTask(){}
	void ExcuteTask();
};

class TransferTask:public AGTask
{
public:
	TransferTask(std::string tskId, 
				 std::string srcId, 
				 std::string dstId):AGTask(tskId, Transer),sourceId(srcId),destId(dstId), heldCallId(-1),activeCallId(-1)
	{

	}
	~TransferTask(){}
	void ExcuteTask();

	void SetTransferTaskType(EnTransferTaskType ttType)
	{
		transferTaskType = ttType;
	}


	void SetHeldCallId(long callId)
	{
		heldCallId= callId;
	}

	void SetActiveCallId(long callId)
	{
		activeCallId = callId;
	}

	EnTransferTaskType GetTransferTaskType()
	{
		return transferTaskType;
	}

	std::string GetDestNo()
	{
		return destId;
	}
	long GetHeldCallId()
	{
		return heldCallId;
	}

	long GetActiveCallId()
	{
		return activeCallId;
	}
private:
	std::string GetUui()
	{
		return "";
	}
private:
	std::string sourceId;
	std::string destId;
	long        heldCallId;
	long        activeCallId;
	EnTransferTaskType transferTaskType;

};

class CancelTransferTask:public AGTask
{
public:
	CancelTransferTask(std::string tskId):AGTask(tskId,CancelTransfer)
	{
	}
	~CancelTransferTask(){}
	void ExcuteTask(); 
};

#endif