#include "AGUtil.h"

unsigned long AGHelper::GenerateInvokeId(std::string identId, int requestType)
{
	unsigned long invokeid = strtoul(identId.c_str(),NULL,10);
	invokeid  = invokeid << 8;
	invokeid |= requestType;
	return invokeid;
}

int AGHelper::AnalyzeInvokeId(unsigned long invokeId, std::string &identId)
{
	int tempType = 0;
	tempType &= 0xFF;
	tempType  = invokeId & 0xFF;
	unsigned long deviceid = invokeId >> 8;
	identId = ConvertULToString(deviceid);
	return tempType;
}

std::string AGHelper::ConvertULToString(unsigned long ulInput)
{
	std::stringstream ss("");
	ss << ulInput;
	std::string strResult;
	ss >> strResult;
	ss.str("");
	return strResult;
}