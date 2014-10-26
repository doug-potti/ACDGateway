#include "CstaErrorDesc.h"

void CstaErrorDesc::GetTsapiRetErrDes(int cstaErrCode, char *errDesc)
{
	switch ( cstaErrCode )
	{
	case ACSERR_APIVERDENIED:
		strcpy_s(errDesc, 500,"This return indicates that the API Version requested is invalid\r\n\t\t"
			"and not supported by the existing API Client Library.");
		break;

	case ACSERR_BADPARAMETER:
		strcpy_s(errDesc, 500,"One or more of the parameters is invalid.");
		break;							 

	case ACSERR_DUPSTREAM:
		strcpy_s(errDesc, 500,"This return indicates that an ACS Stream is already established\r\n\t\t"
			"with the requested Server.");
		break;

	case ACSERR_NODRIVER:
		strcpy_s(errDesc, 500,"This error return value indicates	that no API Client Library Driver\r\n\t\t"
			"was found or installed on the system.");
		break;

	case ACSERR_NOSERVER:
		strcpy_s(errDesc, 500,"This indicates that the requested	Server is not present in the network.");
		break;

	case ACSERR_NORESOURCE:
		strcpy_s(errDesc, 500,"This return value indicates that there are insufficient resources\r\n\t\t"
			"to open a ACS Stream.");
		break;

	case ACSERR_UBUFSMALL:
		strcpy_s(errDesc, 500,"The user buffer size was smaller than the size of the next available event.");
		break;

	case ACSERR_NOMESSAGE:
		strcpy_s(errDesc, 500,"There were no messages available to return to the application.");
		break;

	case ACSERR_UNKNOWN:
		strcpy_s(errDesc, 500,"The ACS Stream has encountered an unspecified error.");
		break;

	case ACSERR_BADHDL:
		strcpy_s(errDesc, 500,"The ACS Handle is invalid.");
		break;

	case ACSERR_STREAM_FAILED:
		strcpy_s(errDesc, 500,"The ACS Stream has failed	due to network problems.No further operations are possible on this stream.");
		break;

	case ACSERR_NOBUFFERS:
		strcpy_s(errDesc, 500,"There were not enough buffers	available to place an outgoing\r\n\t\t"
			"message on the send queue. No message has been sent.");
		break;

	case ACSERR_QUEUE_FULL:
		strcpy_s(errDesc, 500,"The send queue is full.No message has been sent.");
		break;

	case GENERIC_OPERATION:
		strcpy_s(errDesc, 500,"The CTI protocol has been violated or the service invoked is not consistent\r\n\t\t"
			"with a CTI application association.");
		break;

	case REQUEST_INCOMPATIBLE_WITH_OBJECT:
		strcpy_s(errDesc, 500,"The service request does not correspond to a CTI application association.\r\n\t\t"
			"Check that the CTI link is administered in Communication Manager with type ADJ-IP.");
		break;

	case VALUE_OUT_OF_RANGE:
		strcpy_s(errDesc, 500,"Communication Manager detects that a required parameter is missing\r\n\t\t"
			"in the request or an out-of-range value has been specified.");
		break;

	case OBJECT_NOT_KNOWN:
		strcpy_s(errDesc, 500,"The TSAPI Service detects that a required parameter is missing in the request.");
		break;

	case INVALID_FEATURE:
		strcpy_s(errDesc, 500,"The TSAPI Service detects a CSTA Service request that is not supported by Communication Manager.");
		break;

	case GENERIC_SYSTEM_RESOURCE_AVAILABILITY:
		strcpy_s(errDesc, 500,"The request cannot be executed due to a lack of available switch resources.");
		break;

	case RESOURCE_OUT_OF_SERVICE:
		strcpy_s(errDesc, 500,"An application can receive this error code when a single CSTA Service request is ending abnormally\r\n\t\t"
			"due to protocol error.");
		break;

	case NETWORK_BUSY:
		strcpy_s(errDesc, 500,"Communication Manager is not accepting the request at this time because of processor overload.\r\n\t\t"
			"The application may wish to retry the request but should not do so immediately.");
		break;

	case OUTSTANDING_REQUEST_LIMIT_EXCEEDED:
		strcpy_s(errDesc, 500,"The given request cannot be processed due to the system resource limit on the device.");
		break;

	case GENERIC_UNSPECIFIED_REJECTION:
		strcpy_s(errDesc, 500,"This is a TSAPI Service internal error, but it cannot be any more specific. A system administrator may find\r\n\t\t"
			"more detailed information about this error in the AE Services OAM error logs.");
		break;

	case GENERIC_OPERATION_REJECTION:
		strcpy_s(errDesc, 500,"This is a TSAPI Service internal error, but not a defined error.\r\n\t\t"
			"A system administrator should check the TSAPI Service error logs for more detailed information about this error.");
		break;

	case DUPLICATE_INVOCATION_REJECTION:
		strcpy_s(errDesc, 500,"The TSAPI Service detects that the invokeID in the service request is being used by another outstanding\r\n\t\t"
			"service request. This service request is rejected. The outstanding service request with the same invokeID is still valid.");
		break;

	case RESOURCE_LIMITATION_REJECTION:
		strcpy_s(errDesc, 500,"The TSAPI Service detects that it lacks internal resources such as the memory or data records to\r\n\t\t"
			"process a service request.");
		break;

	case ACS_HANDLE_TERMINATION_REJECTION:
		strcpy_s(errDesc, 500,"The TSAPI Service detects that an acsOpenStream session is terminating.");
		break;

	case SERVICE_TERMINATION_REJECTION:
		strcpy_s(errDesc, 500,"The TSAPI Service detects that it cannot provide the service due to the failure or shutting down of the\r\n\t\t"
			"communication link between the Telephony Server and Communication Manager.");
		break;

	case REQUEST_TIMEOUT_REJECTION:
		strcpy_s(errDesc, 500,"The TSAPI Service did not receive the response of a\r\n\t\t"
			"service request sent to Communication Manager more than 20 seconds ago.");
		break;

	case REQUESTS_ON_DEVICE_EXCEEDED_REJECTION:
		strcpy_s(errDesc, 500,"For a device, the TSAPI Service processes one service request at a time.");
		break;


	default:
		strcpy_s(errDesc, 500,"unknown ts ret code");
		break;
	}
}

void CstaErrorDesc::GetACSUniversalFailureDes(int acsErrCode, char * errDesc)
{
	switch ( acsErrCode )
	{
	case ACSERR_APIVERDENIED:
		strcpy_s(errDesc, 500,"This return indicates that the API Version requested is invalid\r\n\t\t"
			"and not supported by the existing API Client Library.");
		break;

	case ACSERR_BADPARAMETER:
		strcpy_s(errDesc, 500,"One or more of the parameters is invalid.");
		break;							 

	case ACSERR_DUPSTREAM:
		strcpy_s(errDesc, 500,"This return indicates that an ACS Stream is already established\r\n\t\t"
			"with the requested Server.");
		break;

	case ACSERR_NODRIVER:
		strcpy_s(errDesc, 500,"This error return value indicates	that no API Client Library Driver\r\n\t\t"
			"was found or installed on the system.");
		break;

	case ACSERR_NOSERVER:
		strcpy_s(errDesc, 500,"This indicates that the requested	Server is not present in the network.");
		break;

	case ACSERR_NORESOURCE:
		strcpy_s(errDesc, 500,"This return value indicates that there are insufficient resources\r\n\t\t"
			"to open a ACS Stream.");
		break;

	case ACSERR_UBUFSMALL:
		strcpy_s(errDesc, 500,"The user buffer size was smaller than the size of the next available event.");
		break;

	case ACSERR_NOMESSAGE:
		strcpy_s(errDesc, 500,"There were no messages available to return to the application.");
		break;

	case ACSERR_UNKNOWN:
		strcpy_s(errDesc, 500,"The ACS Stream has encountered an unspecified error.");
		break;

	case ACSERR_BADHDL:
		strcpy_s(errDesc, 500,"The ACS Handle is invalid.");
		break;

	case ACSERR_STREAM_FAILED:
		strcpy_s(errDesc, 500,"The ACS Stream has failed	due to network problems.No further operations are possible on this stream.");
		break;

	case ACSERR_NOBUFFERS:
		strcpy_s(errDesc, 500,"There were not enough buffers	available to place an outgoing\r\n\t\t"
			"message on the send queue. No message has been sent.");
		break;

	case ACSERR_QUEUE_FULL:
		strcpy_s(errDesc, 500,"The send queue is full.No message has been sent.");
		break;

	case TSERVER_STREAM_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_STREAM_FAILED");
		break;

	case TSERVER_NO_THREAD:
		strcpy_s(errDesc, 500,"TSERVER_NO_THREAD");
		break;

	case TSERVER_BAD_DRIVER_ID:
		strcpy_s(errDesc, 500,"TSERVER_BAD_DRIVER_ID");
		break;

	case TSERVER_DEAD_DRIVER:
		strcpy_s(errDesc, 500,"TSERVER_DEAD_DRIVER");
		break;

	case TSERVER_MESSAGE_HIGH_WATER_MARK:
		strcpy_s(errDesc, 500,"TSERVER_MESSAGE_HIGH_WATER_MARK");
		break;

	case TSERVER_FREE_BUFFER_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_FREE_BUFFER_FAILED");
		break;

	case TSERVER_SEND_TO_DRIVER:
		strcpy_s(errDesc, 500,"TSERVER_SEND_TO_DRIVER");
		break;

	case TSERVER_RECEIVE_FROM_DRIVER:
		strcpy_s(errDesc, 500,"TSERVER_RECEIVE_FROM_DRIVER");
		break;

	case TSERVER_REGISTRATION_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_REGISTRATION_FAILED");
		break;

	case TSERVER_SPX_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_SPX_FAILED");
		break;

	case TSERVER_TRACE:
		strcpy_s(errDesc, 500,"TSERVER_TRACE");
		break;

	case TSERVER_NO_MEMORY:
		strcpy_s(errDesc, 500,"TSERVER_NO_MEMORY");
		break;

	case TSERVER_ENCODE_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_ENCODE_FAILED");
		break;

	case TSERVER_DECODE_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_DECODE_FAILED");
		break;

	case TSERVER_BAD_CONNECTION:
		strcpy_s(errDesc, 500,"TSERVER_BAD_CONNECTION");
		break;

	case TSERVER_BAD_PDU:
		strcpy_s(errDesc, 500,"TSERVER_BAD_PDU");
		break;

	case TSERVER_NO_VERSION:
		strcpy_s(errDesc, 500,"TSERVER_NO_VERSION");
		break;

	case TSERVER_ECB_MAX_EXCEEDED:
		strcpy_s(errDesc, 500,"TSERVER_ECB_MAX_EXCEEDED");
		break;

	case TSERVER_NO_ECBS:
		strcpy_s(errDesc, 500,"TSERVER_NO_ECBS");
		break;

	case TSERVER_NO_SDB:
		strcpy_s(errDesc, 500,"TSERVER_NO_SDB");
		break;

	case TSERVER_NO_SDB_CHECK_NEEDED:
		strcpy_s(errDesc, 500,"TSERVER_NO_SDB_CHECK_NEEDED");
		break;

	case TSERVER_SDB_CHECK_NEEDED:
		strcpy_s(errDesc, 500,"TSERVER_SDB_CHECK_NEEDED");
		break;

	case TSERVER_BAD_SDB_LEVEL:
		strcpy_s(errDesc, 500,"TSERVER_BAD_SDB_LEVEL");
		break;

	case TSERVER_BAD_SERVERID:
		strcpy_s(errDesc, 500,"TSERVER_BAD_SERVERID");
		break;

	case TSERVER_BAD_STREAM_TYPE:
		strcpy_s(errDesc, 500,"TSERVER_BAD_STREAM_TYPE");
		break;

	case TSERVER_BAD_PASSWORD_OR_LOGIN:
		strcpy_s(errDesc, 500,"TSERVER_BAD_PASSWORD_OR_LOGIN");
		break;

	case TSERVER_NO_USER_RECORD:
		strcpy_s(errDesc, 500,"TSERVER_NO_USER_RECORD");
		break;

	case TSERVER_NO_DEVICE_RECORD:
		strcpy_s(errDesc, 500,"TSERVER_NO_DEVICE_RECORD");
		break;

	case TSERVER_DEVICE_NOT_ON_LIST:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_ON_LIST");
		break;

	case TSERVER_USERS_RESTRICTED_HOME:
		strcpy_s(errDesc, 500,"TSERVER_USERS_RESTRICTED_HOME");
		break;

	case TSERVER_NO_AWAYPERMISSION:
		strcpy_s(errDesc, 500,"TSERVER_NO_AWAYPERMISSION");
		break;

	case TSERVER_NO_HOMEPERMISSION:
		strcpy_s(errDesc, 500,"TSERVER_NO_HOMEPERMISSION");
		break;

	case TSERVER_NO_AWAY_WORKTOP:
		strcpy_s(errDesc, 500,"TSERVER_NO_AWAY_WORKTOP");
		break;

	case TSERVER_BAD_DEVICE_RECORD:
		strcpy_s(errDesc, 500,"TSERVER_BAD_DEVICE_RECORD");
		break;

	case TSERVER_DEVICE_NOT_SUPPORTED:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_SUPPORTED");
		break;

	case TSERVER_INSUFFICIENT_PERMISSION:
		strcpy_s(errDesc, 500,"TSERVER_INSUFFICIENT_PERMISSION");
		break;

	case TSERVER_NO_RESOURCE_TAG:
		strcpy_s(errDesc, 500,"TSERVER_NO_RESOURCE_TAG");
		break;

	case TSERVER_INVALID_MESSAGE:
		strcpy_s(errDesc, 500,"TSERVER_INVALID_MESSAGE");
		break;

	case TSERVER_EXCEPTION_LIST:
		strcpy_s(errDesc, 500,"TSERVER_EXCEPTION_LIST");
		break;

	case TSERVER_NOT_ON_OAM_LIST:
		strcpy_s(errDesc, 500,"TSERVER_NOT_ON_OAM_LIST");
		break;

	case TSERVER_PBX_ID_NOT_IN_SDB:
		strcpy_s(errDesc, 500,"TSERVER_PBX_ID_NOT_IN_SDB");
		break;

	case TSERVER_USER_LICENSES_EXCEEDED:
		strcpy_s(errDesc, 500,"TSERVER_USER_LICENSES_EXCEEDED");
		break;

	case TSERVER_OAM_DROP_CONNECTION:
		strcpy_s(errDesc, 500,"TSERVER_OAM_DROP_CONNECTION");
		break;

	case TSERVER_NO_VERSION_RECORD:
		strcpy_s(errDesc, 500,"TSERVER_NO_VERSION_RECORD");
		break;

	case TSERVER_OLD_VERSION_RECORD:
		strcpy_s(errDesc, 500,"TSERVER_OLD_VERSION_RECORD");
		break;

	case TSERVER_BAD_PACKET:
		strcpy_s(errDesc, 500,"TSERVER_BAD_PACKET");
		break;

	case TSERVER_OPEN_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_OPEN_FAILED");
		break;

	case TSERVER_OAM_IN_USE:
		strcpy_s(errDesc, 500,"TSERVER_OAM_IN_USE");
		break;

	case TSERVER_DEVICE_NOT_ON_HOME_LIST:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_ON_HOME_LIST");
		break;

	case TSERVER_DEVICE_NOT_ON_CALL_CONTROL_LIST:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_ON_CALL_CONTROL_LIST");
		break;

	case TSERVER_DEVICE_NOT_ON_AWAY_LIST:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_ON_AWAY_LIST");
		break;

	case TSERVER_DEVICE_NOT_ON_ROUTE_LIST:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_ON_ROUTE_LIST");
		break;

	case TSERVER_DEVICE_NOT_ON_MONITOR_DEVICE_LIST:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_ON_MONITOR_DEVICE_LIST");
		break;

	case TSERVER_DEVICE_NOT_ON_MONITOR_CALL_DEVICE_LIST:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_NOT_ON_MONITOR_CALL_DEVICE_LIST");
		break;

	case TSERVER_NO_CALL_CALL_MONITOR_PERMISSION:
		strcpy_s(errDesc, 500,"TSERVER_NO_CALL_CALL_MONITOR_PERMISSION");
		break;

	case TSERVER_HOME_DEVICE_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_HOME_DEVICE_LIST_EMPTY");
		break;

	case TSERVER_CALL_CONTROL_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_CALL_CONTROL_LIST_EMPTY");
		break;

	case TSERVER_AWAY_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_AWAY_LIST_EMPTY");
		break;

	case TSERVER_ROUTE_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_ROUTE_LIST_EMPTY");
		break;

	case TSERVER_MONITOR_DEVICE_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_MONITOR_DEVICE_LIST_EMPTY");
		break;

	case TSERVER_MONITOR_CALL_DEVICE_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_MONITOR_CALL_DEVICE_LIST_EMPTY");
		break;

	case TSERVER_USER_AT_HOME_WORKTOP:
		strcpy_s(errDesc, 500,"TSERVER_USER_AT_HOME_WORKTOP");
		break;

	case TSERVER_DEVICE_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_DEVICE_LIST_EMPTY");
		break;

	case TSERVER_BAD_GET_DEVICE_LEVEL:
		strcpy_s(errDesc, 500,"TSERVER_BAD_GET_DEVICE_LEVEL");
		break;

	case TSERVER_DRIVER_UNREGISTERED:
		strcpy_s(errDesc, 500,"TSERVER_DRIVER_UNREGISTERED");
		break;

	case TSERVER_NO_ACS_STREAM:
		strcpy_s(errDesc, 500,"TSERVER_NO_ACS_STREAM");
		break;

	case TSERVER_DROP_OAM:
		strcpy_s(errDesc, 500,"TSERVER_DROP_OAM");
		break;

	case TSERVER_ECB_TIMEOUT:
		strcpy_s(errDesc, 500,"TSERVER_ECB_TIMEOUT");
		break;

	case TSERVER_BAD_ECB:
		strcpy_s(errDesc, 500,"TSERVER_BAD_ECB");
		break;

	case TSERVER_ADVERTISE_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_ADVERTISE_FAILED");
		break;

	case TSERVER_NETWARE_FAILURE:
		strcpy_s(errDesc, 500,"TSERVER_NETWARE_FAILURE");
		break;

	case TSERVER_TDI_QUEUE_FAULT:
		strcpy_s(errDesc, 500,"TSERVER_TDI_QUEUE_FAULT");
		break;

	case TSERVER_DRIVER_CONGESTION:
		strcpy_s(errDesc, 500,"TSERVER_DRIVER_CONGESTION");
		break;

	case TSERVER_NO_TDI_BUFFERS:
		strcpy_s(errDesc, 500,"TSERVER_NO_TDI_BUFFERS");
		break;

	case TSERVER_OLD_INVOKEID:
		strcpy_s(errDesc, 500,"TSERVER_OLD_INVOKEID");
		break;

	case TSERVER_HWMARK_TO_LARGE:
		strcpy_s(errDesc, 500,"TSERVER_HWMARK_TO_LARGE");
		break;

	case TSERVER_SET_ECB_TO_LOW:
		strcpy_s(errDesc, 500,"TSERVER_SET_ECB_TO_LOW");
		break;

	case TSERVER_NO_RECORD_IN_FILE:
		strcpy_s(errDesc, 500,"TSERVER_NO_RECORD_IN_FILE");
		break;

	case TSERVER_ECB_OVERDUE:
		strcpy_s(errDesc, 500,"TSERVER_ECB_OVERDUE");
		break;

	case TSERVER_BAD_PW_ENCRYPTION:
		strcpy_s(errDesc, 500,"TSERVER_BAD_PW_ENCRYPTION");
		break;

	case TSERVER_BAD_TSERV_PROTOCOL:
		strcpy_s(errDesc, 500,"TSERVER_BAD_TSERV_PROTOCOL");
		break;

	case TSERVER_BAD_DRIVER_PROTOCOL:
		strcpy_s(errDesc, 500,"TSERVER_BAD_DRIVER_PROTOCOL");
		break;

	case TSERVER_BAD_TRANSPORT_TYPE:
		strcpy_s(errDesc, 500,"TSERVER_BAD_TRANSPORT_TYPE");
		break;

	case TSERVER_PDU_VERSION_MISMATCH:
		strcpy_s(errDesc, 500,"TSERVER_PDU_VERSION_MISMATCH");
		break;

	case TSERVER_VERSION_MISMATCH:
		strcpy_s(errDesc, 500,"TSERVER_VERSION_MISMATCH");
		break;

	case TSERVER_LICENSE_MISMATCH:
		strcpy_s(errDesc, 500,"TSERVER_LICENSE_MISMATCH");
		break;

	case TSERVER_BAD_ATTRIBUTE_LIST:
		strcpy_s(errDesc, 500,"TSERVER_BAD_ATTRIBUTE_LIST");
		break;

	case TSERVER_BAD_TLIST_TYPE:
		strcpy_s(errDesc, 500,"TSERVER_BAD_TLIST_TYPE");
		break;

	case TSERVER_BAD_PROTOCOL_FORMAT:
		strcpy_s(errDesc, 500,"TSERVER_BAD_PROTOCOL_FORMAT");
		break;

	case TSERVER_OLD_TSLIB:
		strcpy_s(errDesc, 500,"TSERVER_OLD_TSLIB");
		break;

	case TSERVER_BAD_LICENSE_FILE:
		strcpy_s(errDesc, 500,"TSERVER_BAD_LICENSE_FILE");
		break;

	case TSERVER_NO_PATCHES:
		strcpy_s(errDesc, 500,"TSERVER_NO_PATCHES");
		break;

	case TSERVER_SYSTEM_ERROR:
		strcpy_s(errDesc, 500,"TSERVER_SYSTEM_ERROR");
		break;

	case TSERVER_OAM_LIST_EMPTY:
		strcpy_s(errDesc, 500,"TSERVER_OAM_LIST_EMPTY");
		break;

	case TSERVER_TCP_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_TCP_FAILED");
		break;

	case TSERVER_SPX_DISABLED:
		strcpy_s(errDesc, 500,"TSERVER_SPX_DISABLED");
		break;

	case TSERVER_TCP_DISABLED:
		strcpy_s(errDesc, 500,"TSERVER_TCP_DISABLED");
		break;

	case TSERVER_REQUIRED_MODULES_NOT_LOADED:
		strcpy_s(errDesc, 500,"TSERVER_REQUIRED_MODULES_NOT_LOADED");
		break;

	case TSERVER_TRANSPORT_IN_USE_BY_OAM:
		strcpy_s(errDesc, 500,"TSERVER_TRANSPORT_IN_USE_BY_OAM");
		break;

	case TSERVER_NO_NDS_OAM_PERMISSION:
		strcpy_s(errDesc, 500,"TSERVER_NO_NDS_OAM_PERMISSION");
		break;

	case TSERVER_OPEN_SDB_LOG_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_OPEN_SDB_LOG_FAILED");
		break;

	case TSERVER_INVALID_LOG_SIZE:
		strcpy_s(errDesc, 500,"TSERVER_INVALID_LOG_SIZE");
		break;

	case TSERVER_WRITE_SDB_LOG_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_WRITE_SDB_LOG_FAILED");
		break;

	case TSERVER_NT_FAILURE:
		strcpy_s(errDesc, 500,"TSERVER_NT_FAILURE");
		break;

	case TSERVER_LOAD_LIB_FAILED:
		strcpy_s(errDesc, 500,"TSERVER_LOAD_LIB_FAILED");
		break;

	case TSERVER_INVALID_DRIVER:
		strcpy_s(errDesc, 500,"TSERVER_INVALID_DRIVER");
		break;

	case TSERVER_REGISTRY_ERROR:
		strcpy_s(errDesc, 500,"TSERVER_REGISTRY_ERROR");
		break;

	case TSERVER_DUPLICATE_ENTRY:
		strcpy_s(errDesc, 500,"TSERVER_DUPLICATE_ENTRY");
		break;

	case TSERVER_DRIVER_LOADED:
		strcpy_s(errDesc, 500,"TSERVER_DRIVER_LOADED");
		break;

	case TSERVER_DRIVER_NOT_LOADED:
		strcpy_s(errDesc, 500,"TSERVER_DRIVER_NOT_LOADED");
		break;

	case TSERVER_NO_LOGON_PERMISSION:
		strcpy_s(errDesc, 500,"TSERVER_NO_LOGON_PERMISSION");
		break;

	case TSERVER_ACCOUNT_DISABLED:
		strcpy_s(errDesc, 500,"TSERVER_ACCOUNT_DISABLED");
		break;

	case TSERVER_NO_NETLOGON:
		strcpy_s(errDesc, 500,"TSERVER_NO_NETLOGON");
		break;

	case TSERVER_ACCT_RESTRICTED:
		strcpy_s(errDesc, 500,"TSERVER_ACCT_RESTRICTED");
		break;

	case TSERVER_INVALID_LOGON_TIME:
		strcpy_s(errDesc, 500,"TSERVER_INVALID_LOGON_TIME");
		break;

	case TSERVER_INVALID_WORKSTATION:
		strcpy_s(errDesc, 500,"TSERVER_INVALID_WORKSTATION");
		break;

	case TSERVER_ACCT_LOCKED_OUT:
		strcpy_s(errDesc, 500,"TSERVER_ACCT_LOCKED_OUT");
		break;

	case TSERVER_PASSWORD_EXPIRED:
		strcpy_s(errDesc, 500,"TSERVER_PASSWORD_EXPIRED");
		break;

	case DRIVER_DUPLICATE_ACSHANDLE:
		strcpy_s(errDesc, 500,"DRIVER_DUPLICATE_ACSHANDLE");
		break;

	case DRIVER_INVALID_ACS_REQUEST:
		strcpy_s(errDesc, 500,"DRIVER_INVALID_ACS_REQUEST");
		break;

	case DRIVER_ACS_HANDLE_REJECTION:
		strcpy_s(errDesc, 500,"DRIVER_ACS_HANDLE_REJECTION");
		break;

	case DRIVER_INVALID_CLASS_REJECTION:
		strcpy_s(errDesc, 500,"DRIVER_INVALID_CLASS_REJECTION");
		break;

	case DRIVER_GENERIC_REJECTION:
		strcpy_s(errDesc, 500,"DRIVER_GENERIC_REJECTION");
		break;

	case DRIVER_RESOURCE_LIMITATION:
		strcpy_s(errDesc, 500,"DRIVER_RESOURCE_LIMITATION");
		break;

	case DRIVER_ACSHANDLE_TERMINATION:
		strcpy_s(errDesc, 500,"DRIVER_ACSHANDLE_TERMINATION");
		break;

	case DRIVER_LINK_UNAVAILABLE:
		strcpy_s(errDesc, 500,"DRIVER_LINK_UNAVAILABLE");
		break;

	case DRIVER_OAM_IN_USE:
		strcpy_s(errDesc, 500,"DRIVER_OAM_IN_USE");
		break;

	default:
		strcpy_s(errDesc, 500,"unknown acs universal fail error");
		break;
	}
}

std::string CstaErrorDesc::GetCstaUniversalFailureDes(CSTAUniversalFailure_t cstaFailCode)
{
	switch(cstaFailCode)
	{
	case genericUnspecified:
		{
			return "genericUnspecified";
		}
	case genericOperation:
		{
			return "genericOperation";
		}
	case requestIncompatibleWithObject:
		{
			return "requestIncompatibleWithObject";
		}
	case valueOutOfRange:
		{
			return "valueOutOfRange";
		}
	case objectNotKnown:
		{
			return "objectNotKnown";
		}
	case invalidCallingDevice:
		{
			return "invalidCallingDevice";
		}
	case invalidForwardingDestination:
		{
			return "invalidForwardingDestination";
		}
	case privilegeViolationOnSpecifiedDevice:
		{
			return "privilegeViolationOnSpecifiedDevice";
		}
	case privilegeViolationOnCalledDevice:
		{
			return "privilegeViolationOnCalledDevice";
		}
	case privilegeViolationOnCallingDevice: 
		{
			return "privilegeViolationOnCallingDevice ";
		}
	case invalidCstaCallIdentifier:   
		{
			return "invalidCstaCallIdentifier ";
		}
	case invalidCstaDeviceIdentifier:   
		{
			return "invalidCstaDeviceIdentifier ";
		}
	case invalidCstaConnectionIdentifier :  
		{
			return "invalidCstaConnectionIdentifier";
		}
	case invalidDestination:    
		{
			return "invalidDestination";
		}
	case invalidFeature:      
		{
			return "invalidFeature";
		}
	case invalidAllocationState:      
		{
			return "invalidAllocationState";
		}
	case invalidCrossRefId:      
		{
			return "invalidCrossRefId";
		}
	case invalidObjectType:      
		{
			return "invalidObjectType";
		}
	case securityViolation:       
		{
			return "securityViolatio";
		}
	case genericStateIncompatibility:         
		{
			return "genericStateIncompatibility";
		}
	case invalidObjectState:          
		{
			return "invalidObjectState";
		}
	case invalidConnectionIdForActiveCall:           
		{
			return "invalidConnectionIdForActiveCall";
		}
	case noActiveCall:            
		{
			return "noActiveCall";
		}
	case noHeldCall:             
		{
			return "noHeldCall";
		}
	case noCallToClear:              
		{
			return "noCallToClear";
		}
	case noConnectionToClear:               
		{
			return "noConnectionToClear";
		}
	case noCallToAnswer:                
		{
			return "noCallToAnswer";
		}
	case noCallToComplete:                
		{
			return "noCallToComplete";
		}
	case genericSystemResourceAvailability:                 
		{
			return "genericSystemResourceAvailability";
		}
	case serviceBusy:                  
		{
			return "serviceBusy";
		}
	case resourceBusy:                   
		{
			return "resourceBusy";
		}
	case resourceOutOfService:                    
		{
			return "resourceOutOfService";
		}
	case networkBusy:                     
		{
			return "networkBusy";
		}
	case networkOutOfService:                      
		{
			return "networkOutOfService";
		}
	case overallMonitorLimitExceeded:                       
		{
			return "overallMonitorLimitExceeded";
		}
	case conferenceMemberLimitExceeded:                        
		{
			return "conferenceMemberLimitExceeded";
		}
	case genericSubscribedResourceAvailability:                         
		{
			return "genericSubscribedResourceAvailability ";
		}
	case objectMonitorLimitExceeded:                          
		{
			return "objectMonitorLimitExceeded";
		}
	case externalTrunkLimitExceeded:                           
		{
			return "externalTrunkLimitExceeded";
		}
	case outstandingRequestLimitExceeded:                            
		{
			return "outstandingRequestLimitExceeded";
		}
	case genericPerformanceManagement:                             
		{
			return "genericPerformanceManagement";
		}
	case performanceLimitExceeded:                              
		{
			return "performanceLimitExceeded";
		}
	case unspecifiedSecurityError:                               
		{
			return "unspecifiedSecurityError";
		}
	case sequenceNumberViolated:                                
		{
			return "sequenceNumberViolated";
		}
	case timeStampViolated:                                 
		{
			return "timeStampViolated";
		}
	case pacViolated:                                 
		{
			return "pacViolated";
		}
	case sealViolated:                                  
		{
			return "sealViolated";
		}
	case genericUnspecifiedRejection:                                    
		{
			return "genericUnspecifiedRejection";
		}
	case genericOperationRejection:                                    
		{
			return "genericOperationRejection";
		}
	case duplicateInvocationRejection:                                      
		{
			return "duplicateInvocationRejection";
		}
	case unrecognizedOperationRejection:                                       
		{
			return "unrecognizedOperationRejection";
		}
	case mistypedArgumentRejection:                                        
		{
			return "mistypedArgumentRejection";
		}
	case resourceLimitationRejection:                                         
		{
			return "resourceLimitationRejection";
		}
	case acsHandleTerminationRejection:                                          
		{
			return "acsHandleTerminationRejection";
		}
	case serviceTerminationRejection:                                           
		{
			return "serviceTerminationRejection";
		}
	case requestTimeoutRejection:                                            
		{
			return "requestTimeoutRejection";
		}	
	case requestsOnDeviceExceededRejection:                                            
		{
			return "requestsOnDeviceExceededRejection";
		}	
	case unrecognizedApduRejection:                                              
		{
			return "unrecognizedApduRejection";
		}
	case mistypedApduRejection:                                               
		{
			return "mistypedApduRejection";
		}
	case badlyStructuredApduRejection:                                               
		{
			return "badlyStructuredApduRejection";
		}
	case initiatorReleasingRejection:                                                 
		{
			return "initiatorReleasingRejection";
		}
	case unrecognizedLinkedidRejection:                                                  
		{
			return "unrecognizedLinkedidRejection";
		}
	case linkedResponseUnexpectedRejection:                                                   
		{
			return "linkedResponseUnexpectedRejection";
		}
	case unexpectedChildOperationRejection:                                                    
		{
			return "unexpectedChildOperationRejection";
		}
	case mistypedResultRejection:                                                     
		{
			return "mistypedResultRejection";
		}
	case unrecognizedErrorRejection:                                                      
		{
			return "unrecognizedErrorRejection";
		}
	case unexpectedErrorRejection:                                                      
		{
			return "unexpectedErrorRejection";
		}
	case mistypedParameterRejection:                                                        
		{
			return "mistypedParameterRejection";
		}
	case nonStandard:                                                         
		{
			return "nonStandard";
		}
	default:
		{
			return "unkown fali code";
		}
	}
}