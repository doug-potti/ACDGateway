#ifndef ACDGW_CSTA_ERROR_DESC
#define ACDGW_CSTA_ERROR_DESC

#include "AGUtil.h"

class CstaErrorDesc
{
public:
	CstaErrorDesc(void){}
	~CstaErrorDesc(void){}
public:
	static void GetTsapiRetErrDes(int cstaErrCode, char *errDesc);
	static void GetACSUniversalFailureDes(int acsErrCode, char * errDesc);
	static std::string GetCstaUniversalFailureDes(CSTAUniversalFailure_t cstaFailCode);
};

#endif