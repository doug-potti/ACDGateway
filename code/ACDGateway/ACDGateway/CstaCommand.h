#ifndef ACDGW_CSTA_COMMAND_H
#define ACDGW_CSTA_COMMAND_H

#include <queue>
#include "AGUtil.h"

typedef std::queue<TsapiCommand_t *> CSTACMDQUE;

class CstaCommand:public IceUtil::Thread
{
public:
	CstaCommand():isDone(false){}
	~CstaCommand(){}

	void AddTsapiCmdToQueue(TsapiCommand_t *pCommand);
	void Stop();
private:
	void run();
	void ProcessCstaCmd(TsapiCommand_t *pCommand);
private:
	bool             isDone;
	CSTACMDQUE       cmdQue;
	ICERECMUTEX      cqRecMutex;
};



#endif