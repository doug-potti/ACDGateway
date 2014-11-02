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
class  AGTask
{
public:
	AGTask(){}
	AGTask(std::string taskid,EnTaskType tskType):taskId(taskid),taskType(tskType),taskCallId(-1),monRefId(-1)
	{

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
						std::string custLevel):AGTask(tskId,Distribute),mediaType(medType),businessType(busType), 
						                    customerLevel(custLevel)
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

	std::string GetDialNo()
	{
		return dialNo;
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
				 std::string dstId):AGTask(tskId, Transer),sourceId(srcId),destId(dstId)
	{

	}
	~TransferTask(){}
	void ExcuteTask();


	std::string GetDestNo()
	{
		return destId;
	}
private:
	std::string GetUui()
	{
		return "";
	}
private:
	std::string sourceId;
	std::string destId;

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