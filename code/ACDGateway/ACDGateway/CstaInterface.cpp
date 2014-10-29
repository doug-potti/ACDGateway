#include "CstaInterface.h"
#include "CstaErrorDesc.h"
#include "CstaEventProcess.h"


CstaInterfacePtr             gCstaInterfaceThd; 
CstaInterface*               CstaInterface::cstaInterfaceSelf = NULL;
char                         CstaInterface::arrErrorMsg[512];
char                         CstaInterface::arrPbxEvent[PBXEVENT_SIZE];


CstaInterface::CstaInterface(void):isDone(false),acsHandle(NULL),usNumEvents(0),isRun(false),isPause(false)
{
	usEventBufSize = sizeof(CSTAEvent_t);
	pauseHandler = CreateEvent(NULL,FALSE,FALSE,NULL);
}


CstaInterface::~CstaInterface(void)
{
	if (acsHandle != NULL)
	{
		SysStatusStop();
		acsAbortStream(acsHandle,NULL);
		isDone = true;
		acsHandle = NULL;
	}
}

CstaInterface *CstaInterface::GetCstaInstance()
{
	if (cstaInterfaceSelf == NULL)
	{
		cstaInterfaceSelf = new CstaInterface();
	}
	return cstaInterfaceSelf;
}

void CstaInterface::run()
{
	std::stringstream sslog("");

	while (!isDone)
	{
		if (isPause)
		{
			sslog<<"CstaInterface.run start pause because reconnect aes link."<<std::endl;
			OUTINFOLOG(sslog.str());
			sslog.str("");
			if (WaitForSingleObject(pauseHandler, INFINITE) == WAIT_OBJECT_0)
			{
				sslog<<"CstaInterface.run retrieve run beause reconnect successed."<<std::endl;
				OUTINFOLOG(sslog.str());
				sslog.str("");
			}
		}
		else
		{
			int handlenumber = 0;
			while ((GetEventPoll() == true) && (handlenumber < 521))
			{
				handlenumber ++;
			}
			this->getThreadControl().sleep(IceUtil::Time::milliSeconds(1));
		}
	}
}

void CstaInterface::stoprun()
{
	IceUtil::Monitor<IceUtil::RecMutex>::Lock lock(runMutexEvent);
	isDone = true;
}

void CstaInterface::SetPauseFlag(bool isPause)
{
	if (isPause == true)
	{
		this->isPause = true;
	}
	else
	{
		this->isPause = false;
		SetEvent(pauseHandler);
	}
}

bool CstaInterface::OpenAcsStream(std::string acsServerName, std::string acsUser, std::string acsPassword)
{
	std::stringstream sslog;

	strcpy_s(privateData.vendor, 8,"VERSION");
	privateData.data[0] = PRIVATE_DATA_ENCODING;

	if(attMakeVersionString("2-9", &(privateData.data[1])) > 0 )
	{
		privateData.length =(unsigned short)strlen(&privateData.data[1]) + 2;
	}
	else
	{
		privateData.length = 0;
	}

	retCode = acsOpenStream(&acsHandle,
							APP_GEN_ID,
							2014,
							ST_CSTA,
							(ServerID_t *)acsServerName.c_str(),
							(LoginID_t *)acsUser.c_str(),
							(Passwd_t *)acsPassword.c_str(),
							(AppName_t *)"telserver",
							ACS_LEVEL1,
							(Version_t *)"TS1-2",
							1000,
							10240,
							2000,
							20480,
							(PrivateData_t *)&privateData);

	if( retCode < 0 )
	{
		CstaErrorDesc::GetACSUniversalFailureDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.OpenAcsStream failed  errmsg:"<<
			   arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
		return false;
	}

	if (GetEventBlock() == false)
	{
		acsAbortStream(acsHandle,0);
		OUTERRORLOG("CstaInterface.OpenAcsStream failed GetEventBlock reurn false main return after abort acs stream.");
		return false;
	}

	ACSConfirmationEvent *pAcsConf = NULL;

	pAcsConf = &(cstaEvent.event.acsConfirmation);

	if (cstaEvent.eventHeader.eventClass == ACSCONFIRMATION && 
		cstaEvent.eventHeader.eventType == ACS_OPEN_STREAM_CONF)
	{
		sslog<<"CstaInterface.OpenAcsStream ACS_OPENSTREAM_CONF apiVer:"<< 
			   pAcsConf->u.acsopen.apiVer<<" libVer:"<<
			   pAcsConf->u.acsopen.libVer<<" tsvrVer:"<<
			   pAcsConf->u.acsopen.tsrvVer<<" driVer:"<<
			   pAcsConf->u.acsopen.drvrVer<<std::endl;

		OUTINFOLOG(sslog.str());
		sslog.str("");

		if (SysStatusStart() == false)
		{
			OUTERRORLOG("CstaInterface.OpenAcsStream failed SysStatusStart reurn false main return.");
			return false;
		}
	}
	else
	{
		acsAbortStream(acsHandle, 0);
		CstaErrorDesc::GetACSUniversalFailureDes(pAcsConf->u.failureEvent.error, arrErrorMsg);
		sslog<<"CstaInterface.OpenAcsStream open stream conf evt is incorrect err:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
		return false;
	}
	return true;
}

bool CstaInterface::CloseAcsStream()
{
	CheckAcsHandle;

	retCode = acsCloseStream(acsHandle,
							 2015,
							 NULL);

	if( retCode < 0 )
	{
		std::stringstream sslog("");
		CstaErrorDesc::GetACSUniversalFailureDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.CloseAcsStream failed errmsg:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
		return false;
	}
	return true;
}

bool CstaInterface::SysStatusStart()
{
	CheckAcsHandle;

	SystemStatusFilter_t statusFilter = 0;


	std::stringstream sslog("");

	retCode = cstaSysStatStart(acsHandle,
							   (InvokeID_t)2012,
							   statusFilter,
							   NULL);

	if (retCode != ACSPOSITIVE_ACK)
	{
		CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.SysStatusStart cstaSysStatStart failed errmsg:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
		return false;
	}

	if (GetEventBlock() == false)
	{
		OUTERRORLOG("CstaInterface.SysStatusStart GetEventBlock return false");
		return false;
	}

	if (cstaEvent.eventHeader.eventType ==  CSTA_SYS_STAT_START_CONF && 
		cstaEvent.eventHeader.eventClass == CSTACONFIRMATION)
	{
		OUTINFOLOG("CstaInterface.SysStatusStart invoke sucessed");
		return true;
	}

	CstaErrorDesc::GetTsapiRetErrDes(cstaEvent.event.cstaConfirmation.u.universalFailure.error, arrErrorMsg);

	sslog<<"CstaInterface.SysStatusStart cstaSysStatStart failed errmsg:"<<
			arrErrorMsg<<std::endl;
	OUTERRORLOG(sslog.str());
	sslog.str("");
	return false;
}

bool CstaInterface::SysStatusStop()
{
	CheckAcsHandle;

	std::stringstream sslog("");
	retCode = cstaSysStatStop(acsHandle,
							  (InvokeID_t)2013, 
							  NULL);

	if (retCode != ACSPOSITIVE_ACK)
	{
		CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.SysStatusStop cstaSysStatStop fail err msg:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
		return false;
	}

	return true;
}

bool CstaInterface::GetEventBlock()
{
	CheckAcsHandle;

	std::stringstream sslog("");

	privateData.length = ATT_MAX_PRIVATE_DATA;

	retCode = acsGetEventBlock(acsHandle,
							   (void *)&cstaEvent,
							   &usEventBufSize, 
							   (PrivateData_t *)&privateData, 
							   &usNumEvents);

	if (retCode != ACSPOSITIVE_ACK)
	{
		CstaErrorDesc::GetACSUniversalFailureDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.GetEventBlock failed return falseerrmsg:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");

		return false;
	}
	else
	{
		if (privateData.length > 0)
		{
			retCode = attPrivateData(&privateData, &attEvent);
			if (retCode != ACSPOSITIVE_ACK)
			{
				CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
				sslog<<"CstaInterface.GetEventBlock attPrivateData failed errmsg:"<<
						arrErrorMsg<<std::endl;
				OUTERRORLOG(sslog.str());
				sslog.str("");
			}
			privateData.length = 0;
			return true;
		}
	}
	return false;
}

bool CstaInterface::GetEventPoll()
{
	CheckAcsHandle;

	std::stringstream sslog("");

	privateData.length = ATT_MAX_PRIVATE_DATA;
	usEventBufSize     = PBXEVENT_SIZE;

	retCode = acsGetEventPoll(acsHandle,
						      (void *)arrPbxEvent,
							  &usEventBufSize,
							  (PrivateData_t *)&privateData, 
							  &usNumEvents);

	if (retCode == ACSPOSITIVE_ACK)
	{
		if (privateData.length > 0)
		{
			retCode = attPrivateData(&privateData, &attEvent);
			if (retCode != ACSPOSITIVE_ACK)
			{
				CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
				sslog<<"CstaInterface.GetEventPoll get attPrivateData failed errmsg:"<<
						arrErrorMsg<<std::endl;
				OUTERRORLOG(sslog.str());
				sslog.str("");
				privateData.length = 0;
			}

		}
		
		AvayaPbxEvent_t *newpnxevent = new AvayaPbxEvent_t();
		if (newpnxevent != NULL)
		{
			memcpy(&newpnxevent->cstaEvent,arrPbxEvent,PBXEVENT_SIZE);
			newpnxevent->privateDataLength = privateData.length;
			if (newpnxevent->privateDataLength > 0)
			{
				memcpy(&newpnxevent->attEvent, &attEvent, sizeof(ATTEvent_t));
			}
			//添加事件到队列
			if (gCstaEventProcessThd != NULL)
			{
				gCstaEventProcessThd->AddPbxEventToQueue(newpnxevent);
			}
		}

		return true;
	}
	else if ((retCode == ACSERR_BADHDL) || (retCode == ACSERR_STREAM_FAILED))
	{
		sslog<<"CstaInterface.GetEventPoll Invoke acsGetEventPoll, return code indicates that stream with tserver is failed."<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
	}
	else
	{
		//normal
	}

	return false;
}

bool CstaInterface::MonitorDevice(std::string devId, unsigned long invokeId)
{
	stringstream sslog("");
	sslog<<"CstaInterface.MonitorDevice devid:"<<
		devId<<std::endl;
	OUTDEBUGLOG(sslog.str());
	sslog.str("");

	CheckAcsHandle;

	CSTAMonitorFilter_t noFilter;
	noFilter.call          = 0;
	noFilter.feature       = 0;
	noFilter.maintenance   = 0;
	noFilter.agent         = 0;
	noFilter.privateFilter = 0;

	retCode = cstaMonitorDevice(acsHandle, 
		                        invokeId,
								(DeviceID_t *)devId.c_str(), 
								&noFilter, 
								NULL);

	if (retCode != ACSPOSITIVE_ACK)
	{
		CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.MonitorDevice.cstaMonitorDevice  deviceid:"<<
				devId<<" fail\r\n"<<"fail msg:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");

		if ((retCode == ACSERR_BADHDL) || (retCode == ACSERR_STREAM_FAILED))
		{
			sslog<<"CstaInterface.MonitorDevice deviceid:"<<
					devId<<" fail\r\n"<<"Invoke cstaMonitorDevice, return code indicates that stream with tserver is failed."<<std::endl;
			OUTERRORLOG(sslog.str());
			sslog.str("");
		}
		return false;
	}

	return true;
}

bool CstaInterface::MonitorViaDevice(std::string viaDevId, unsigned long invokeId)
{
	stringstream sslog("");
	sslog<<"CstaInterface.MonitorViaDevice devid:"<<
		   viaDevId<<std::endl;
	OUTDEBUGLOG(sslog.str());
	sslog.str("");

	CheckAcsHandle;

	CSTAMonitorFilter_t noFilter;
	noFilter.call          = 0;
	noFilter.feature       = 0;
	noFilter.maintenance   = 0;
	noFilter.agent         = 0;
	noFilter.privateFilter = 0;

	retCode = cstaMonitorCallsViaDevice(acsHandle, 
										invokeId,
										(DeviceID_t *)viaDevId.c_str(), 
										&noFilter, 
										NULL);

	if (retCode != ACSPOSITIVE_ACK)
	{
		CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.MonitorViaDevice.cstaMonitorCallsViaDevice  deviceid:"<<
				viaDevId<<" fail\r\n"<<"fail msg:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");

		if ((retCode == ACSERR_BADHDL) || (retCode == ACSERR_STREAM_FAILED))
		{
			sslog<<"CstaInterface.MonitorViaDevice deviceid:"<<
					viaDevId<<" fail\r\n"<<"Invoke cstaMonitorCallsViaDevice, return code indicates that stream with tserver is failed."<<std::endl;
			OUTERRORLOG(sslog.str());
			sslog.str("");
		}
		return false;
	}

	return true;
}

bool CstaInterface::QueryDeviceInfo(std::string deviceId, unsigned long invokeId)
{
	return true;
}

bool CstaInterface::CallMake(std::string devId, std::string destNo, std::string userData,unsigned long invokeId)
{
	stringstream sslog("");
	sslog<<"CstaInterface.CallMake devid:"<<
			devId<<" destNo:"<<
			destNo<<" userData:"<<
			userData<<std::endl;
	OUTDEBUGLOG(sslog.str());
	sslog.str("");

	CheckAcsHandle;

	if (userData.length() > 0)
	{
		ATTUserToUserInfo_t uui;
		uui.type = ATTUUIProtocolType_t::uuiUserSpecific;
		uui.data.length = userData.length();
		strcpy((char *)uui.data.value,userData.c_str());

		retCode = attV6MakeCall(&privateData,
								NULL,
								false, 
								&uui);

		if (retCode != ACSPOSITIVE_ACK)
		{
			CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
			sslog<<"TsapiInterface.CallMake.attV6MakeCall set uui  deviceid:"<<
					devId<<" destno:"<<
					destNo<<"  uui:"<<
					userData<<" fail\r\n"<<"fail msg:"<<
					arrErrorMsg<<std::endl;
			OUTERRORLOG(sslog.str());
			sslog.str("");
		}
	}

	DeviceID_t t_callingdevid;
	DeviceID_t t_calleddevid;
	strcpy(t_callingdevid,devId.c_str());
	strcpy(t_calleddevid,destNo.c_str());

	retCode = cstaMakeCall(acsHandle, 
						   invokeId,
						   &t_callingdevid,
						   &t_calleddevid,
						   (PrivateData_t *)&privateData);

	if (retCode != ACSPOSITIVE_ACK)
	{
		CstaErrorDesc::GetTsapiRetErrDes(retCode, arrErrorMsg);
		sslog<<"CstaInterface.CallMake.cstaMakeCall  deviceid:"<<
				devId<<" destno:"<<
				destNo<<"  uui:"<<
				userData<<" fail\r\n"<<"fail msg:"<<
				arrErrorMsg<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");

		if ((retCode == ACSERR_BADHDL) || (retCode == ACSERR_STREAM_FAILED))
		{
			sslog<<"CstaInterface.CallMake deviceid:"<<
					devId<<" fail\r\n"<<"Invoke cstaMakeCall, return code indicates that stream with tserver is failed."<<std::endl;
			OUTERRORLOG(sslog.str());
			sslog.str("");
		}
		return false;
	}

	return true;
}


