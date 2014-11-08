
#include "AGHttpServer.h"
#include "AGService.h"

AGHttpServerPtr                     gAGHttpServer;

AGHttpServer::~AGHttpServer()
{
	Stop();
}


void AGHttpServer::run()
{
	try
	{
		if (httpServer == NULL)
		{
			httpServer = new http::server::server(addr, port);
		}

		httpServer->run();
	}
	catch(...)
	{
		OUTERRORLOG("AGHttpServer thread run occur exception.");
	}

}

std::string AGHttpServer::ConstructFailDesc(std::string mediaId, std::string failDesc)
{
	std::string result("{\"Id\":\"");
	result.append(mediaId);
	result.append("\,\"result\":\"fail\",\"errorMsg\":\"");
	result.append(failDesc);
	result.append("\"}");
	return result;
}

std::string AGHttpServer::ConstructDisAgtSuc(std::string mediaId, std::string agentId)
{
	std::string result("{\"Id\":\"");
	result.append(mediaId);
	result.append("\,\"result\":\"suc\",\"agent\":\"");
	result.append(agentId);
	result.append("\"}");
	return result;
}

std::string AGHttpServer::ConstructCanDASuc(std::string mediaId)
{
	std::string result("{\"Id\":\"");
	result.append(mediaId);
	result.append("\,\"result\":\"suc\"}");
	return result;
}

std::string AGHttpServer::ConstructTranSuc(std::string mediaId)
{
	std::string result("{\"Id\":\"");
	result.append(mediaId);
	result.append("\,\"result\":\"suc\"}");
	return result;
}

std::string AGHttpServer::ConstructCanTranSuc(std::string mediaId)
{
	std::string result("{\"Id\":\"");
	result.append(mediaId);
	result.append("\,\"result\":\"suc\"}");
	return result;
}

void AGHttpServer::SendResponse(std::string clientIdent, std::string result)
{
	if (httpServer != NULL)
	{
		httpServer->GetReqHandler().SendHttpResponse(clientIdent, result);
	}
}

void AGHttpServer::Stop()
{
	isDone = true;
}