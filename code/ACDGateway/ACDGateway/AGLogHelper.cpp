#include "AGLogHelper.h"

AGLogHelper *gLogHelper;

AGLogHelper::AGLogHelper(std::string logCfgPath)
{
	LogLog::getLogLog()->setInternalDebugging(true);
	logger = Logger::getRoot();
	try 
	{
		ConfigureAndWatchThread configureThread(LOG4CPLUS_TEXT(logCfgPath), 5000);
		LOG4CPLUS_WARN(logger, "load log config ....");
	}
	catch(...) 
	{
		std::cout << "Exception..." << std::endl;
		LOG4CPLUS_FATAL(logger, "load log config exception occured...");
	}
}

AGLogHelper::~AGLogHelper(void)
{

}

void AGLogHelper::WriteTrace(std::string logMsg)
{
	LOG4CPLUS_TRACE_METHOD(logger,logMsg);
}

void AGLogHelper::WriteDebug(std::string logMsg)
{
	LOG4CPLUS_DEBUG(logger, logMsg);
}

void AGLogHelper::WriteInfo(std::string logMsg)
{
	LOG4CPLUS_INFO(logger,logMsg);
}

void AGLogHelper::WriteWarn(std::string logMsg)
{
	LOG4CPLUS_WARN(logger,logMsg);
}

void AGLogHelper::WriteError(std::string logMsg)
{	
	LOG4CPLUS_ERROR(logger, logMsg);
}

void AGLogHelper::WriteFatal(std::string logMsg)
{
	LOG4CPLUS_FATAL(logger, logMsg);
}
