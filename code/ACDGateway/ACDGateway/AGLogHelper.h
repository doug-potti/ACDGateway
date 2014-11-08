#ifndef ACDGW_LOG_HELPER_H
#define ACDGW_LOG_HELPER_H

#include <iostream>
#include "log4cplus/configurator.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/stringhelper.h"
#include "log4cplus/loggingmacros.h"


using namespace log4cplus;
using namespace log4cplus::helpers;

class AGLogHelper
{
public:
	AGLogHelper(std::string logCfgPath);
	~AGLogHelper(void);
public:
	void WriteTrace(std::string logMsg);
	void WriteDebug(std::string logMsg);
	void WriteInfo(std::string logMsg);
	void WriteWarn(std::string logMsg);
	void WriteError(std::string logMsg);
	void WriteFatal(std::string logMsg);

private:
	log4cplus::Logger logger;
};
extern AGLogHelper * gLogHelper;

#endif