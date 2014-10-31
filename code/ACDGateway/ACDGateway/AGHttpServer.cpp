#include "AGHttpServer.h"
#include "AGService.h"
#include "AGUtil.h"

std::string ParseHttpMessage(std::string id, std::string gap,std::string &result)
{
	if (result.size() == 0)
	{
		return "";
	}
	std::string ret = "";
	size_t beginPos = result.find(id);
	size_t endPos = result.find(gap);
	if (beginPos >= endPos )
	{
		return ret;
	}
	if (endPos != std::string::npos )
	{
		ret = result.substr(beginPos + id.size(), endPos - beginPos -id.size());
	}
	else
	{
		ret = result.substr(beginPos + id.size());
	}
	
	if (endPos + 1 != std::string::npos)
	{
		result = result.substr(endPos + 1);
	}
	return ret;
}

void HttpReqCallback(struct evhttp_request *req, void *arg)
{
	std::stringstream sslog("");
	std::string cmdtype;
	evhttp_cmd_type httpCmdType = evhttp_request_get_command(req);
	std::string host = evhttp_request_get_host(req);
	switch (httpCmdType) 
	{
		case EVHTTP_REQ_GET: 
			cmdtype = "GET"; 
			break;
		case EVHTTP_REQ_POST: 
			cmdtype = "POST"; 
			break;
		case EVHTTP_REQ_HEAD: 
			cmdtype = "HEAD"; 
			break;
		case EVHTTP_REQ_PUT: 
			cmdtype = "PUT"; 
			break;
		case EVHTTP_REQ_DELETE: 
			cmdtype = "DELETE";
			break;
		case EVHTTP_REQ_OPTIONS: 
			cmdtype = "OPTIONS"; 
			break;
		case EVHTTP_REQ_TRACE: 
			cmdtype = "TRACE"; 
			break;
		case EVHTTP_REQ_CONNECT: 
			cmdtype = "CONNECT"; 
			break;
		case EVHTTP_REQ_PATCH: 
			cmdtype = "PATCH"; 
			break;
		default: 
			cmdtype = "unknown"; 
			break;
	}

	std::string result = evhttp_request_get_uri(req);
	sslog<<"HttpReqCallback recive http message type:"<<
			cmdtype<<"\r\n\t\tmessage:"<<result<<std::endl;
	OUTDEBUGLOG(sslog.str());
	if (httpCmdType == EVHTTP_REQ_GET)
	{
		size_t pos;
		if (( pos = result.find("/DistributeMultimediaCall.html") ) != std::string::npos )
		{



			std::string mediaId = ParseHttpMessage("Id=","&",result);
			std::string meidaType = ParseHttpMessage("Mediatype=","&",result);;
			std::string bussType = ParseHttpMessage("Businesstype=","&",result);
			std::string custlevel = ParseHttpMessage("Customlevel=","&",result);

			std::string dialNo = AGHelper::FindDialNoByBusiness(bussType);

			if (dialNo.size() == 0)
			{
				return;//or return mir
			}
			if (AGHelper::IsExistTask(mediaId, Distribute))
			{
				return;//or return mir
			}

			DistributeAgentTask *disTask = new DistributeAgentTask(mediaId,
																   meidaType,
																   bussType,
																   custlevel,
																   dialNo);
			if (disTask)
			{
				if (gAGService != NULL)
				{
					gAGService->AddTaskToTL(disTask);
				}
			}
				
		}
		else if(( pos = result.find("/CancelDistributeMultimediaCall.html") ) != std::string::npos )
		{
			std::string cancelMedId = ParseHttpMessage("Id=","&",result);
			if (AGHelper::IsExistTask(cancelMedId, CancelDistribute))
			{
				return;//or return mir
			}

			CancelDisAgtTask *cdaTask = new CancelDisAgtTask(cancelMedId);

			if (cdaTask)
			{
				if (gAGService != NULL)
				{
					gAGService->AddTaskToTL(cdaTask);
				}
			}

		}
		else if(( pos = result.find("/Transfer.html") ) != std::string::npos )
		{
			std::string transMedId = ParseHttpMessage("Id=","&",result);
			std::string srcId = ParseHttpMessage("Srcagentid=","&",result);
			std::string destId = ParseHttpMessage("Destinationagentid=","&",result);
			if (AGHelper::IsExistTask(transMedId, Transer))
			{
				return;//or return mir
			}

			TransferTask *ttask = new TransferTask(transMedId, srcId, destId);
			if (ttask)
			{
				if (gAGService != NULL)
				{
					gAGService->AddTaskToTL(ttask);
				}
			}
		}
		else if(( pos = result.find("/CancelTransfer.html") ) != std::string::npos )
		{
			std::string canTransId = ParseHttpMessage("Id=","&",result);
			if (AGHelper::IsExistTask(canTransId, CancelTransfer))
			{
				return;//or return mir
			}

			CancelTransferTask *cttask = new CancelTransferTask(canTransId);
			if (cttask)
			{
				if (gAGService != NULL)
				{
					gAGService->AddTaskToTL(cttask);
				}
			}
		}
		else
		{
			//dump
		}
	}

}

AGHttpServerPtr                     gAGHttpServer;

AGHttpServer::~AGHttpServer()
{
	Stop();
}

int AGHttpServer::InitWinSocket()
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2) , &wsaData) != 0) 
	{
		return -1;
	}
	return 0;
}

void AGHttpServer::run()
{
	try
	{
		int ret = InitWinSocket();
		if (ret != 0)
		{
			OUTERRORLOG("AGHttpServer thread run initialize win socket occur error.");
			return;
		}

		struct event_base * base = event_base_new();
		if (!base)
		{
			OUTERRORLOG("AGHttpServer thread run event_base_new occur error.");
			return;
		}

		http_server = evhttp_new(base);
		if (!http_server)
		{
			OUTERRORLOG("AGHttpServer thread run evhttp_new occur error.");
			return;
		}
		evhttp_set_gencb(http_server, HttpReqCallback, NULL);

		ret = evhttp_bind_socket(http_server,addr.c_str(),port);

		if (ret != 0)
		{
			OUTERRORLOG("AGHttpServer thread run evhttp_bind_socket occur error.");
			return;
		}

		OUTERRORLOG("AGHttpServer thread run start successed.");
		event_base_dispatch(base);
		evhttp_free(http_server);
		WSACleanup();
	}
	catch(...)
	{
		OUTERRORLOG("AGHttpServer thread run occur exception.");
	}

}

void AGHttpServer::SendResponse(std::string clientIdent, std::string result)
{
	std::stringstream sslog("");
	sslog<<"AGHttpServer.SendResponse ident:"<<
			clientIdent<<" result:"<<result<<std::endl;
	OUTDEBUGLOG(sslog.str());
	sslog.str("");
	struct evbuffer *buf = evbuffer_new();
	
	if(!buf)
	{
		sslog<<"AGHttpServer.SendResponse evbuffer_new occur error ident:"<<
				clientIdent<<" result:"<<result<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
		return;
	}
	try
	{
// 		evbuffer_add_printf(buf, "Server Responsed. Requested: finetel", evhttp_request_get_uri(req));
// 		evhttp_send_reply(req, HTTP_OK, "OK", buf);
// 		evbuffer_free(buf);
	}
	catch(...)
	{
		sslog<<"AGHttpServer.SendResponse evhttp_send_reply occur error ident:"<<
				clientIdent<<" result:"<<result<<std::endl;
		OUTERRORLOG(sslog.str());
		sslog.str("");
	}

}

void AGHttpServer::Stop()
{
	evhttp_free(http_server);
	WSACleanup();
}