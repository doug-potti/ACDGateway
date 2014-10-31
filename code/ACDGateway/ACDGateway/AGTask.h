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
	AGTask(std::string taskid,EnTaskType tskType):taskId(taskid),taskType(tskType)
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

protected:
	EnTaskType  taskType;
	std::string taskId;

};

class DistributeAgentTask:public AGTask
{
public:
	DistributeAgentTask(std::string tskId,
		                std::string medType,
						std::string busType,
						std::string custLevel,
						std::string dlNo):AGTask(tskId,Distribute),mediaType(medType),businessType(busType), 
						                    customerLevel(custLevel),dialNo(dlNo)
	{}
	~DistributeAgentTask(){}
	void ExcuteTask();

	void SetTaskDevId(std::string devId)
	{
		taskDevId = devId;
	}

	void SetTaskCallId(long callId)
	{
		taskCallId = callId;
	}

	std::string GetDialNo()
	{
		return dialNo;
	}

	std::string GetTaskDevId()
	{
		return taskDevId;
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
	std::string taskDevId;
	long        taskCallId;
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
	void SetTaskDevId(std::string devId)
	{
		taskDevId = devId;
	}

	void SetTaskCallId(long callId)
	{
		taskCallId = callId;
	}

	std::string GetDestNo()
	{
		return destId;
	}

	std::string GetTaskDevId()
	{
		return taskDevId;
	}
private:
	std::string GetUui()
	{
		return "";
	}
private:
	std::string sourceId;
	std::string destId;
	std::string taskDevId;
	long        taskCallId;

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