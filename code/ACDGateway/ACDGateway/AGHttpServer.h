#ifndef ACDGW_HTTP_SERVER_H
#define ACDGW_HTTP_SERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "HttpServer/server.hpp"
#include "AGUtil.h"


//typedef std::map<std::string, struct evhttp_request *> HTTPCLIENTMAP;
class AGHttpServer:public IceUtil::Thread
{
public:
	AGHttpServer(std::string httpAddr, std::string httpPort):addr(httpAddr),port(httpPort),isDone(false),httpServer(NULL)
	{
	}
	~AGHttpServer();
	void         Stop();
	void         SendResponse(std::string clientIdent, std::string result);
	std::string  ConstructFailDesc(std::string mediaId, std::string failDesc);
	std::string  ConstructDisAgtSuc(std::string mediaId, std::string agentId);
	std::string  ConstructCanDASuc(std::string mediaId);
	std::string  ConstructTranSuc(std::string mediaId);
	std::string  ConstructCanTranSuc(std::string mediaId);

private:
	void         run();
private:
	http::server::server* httpServer;                 
	std::string     addr;
	std::string     port;
	bool            isDone;
	IceUtil::Monitor<IceUtil::RecMutex>     cmMutex;
};
typedef IceUtil::Handle<AGHttpServer>       AGHttpServerPtr;
extern  AGHttpServerPtr                     gAGHttpServer;

#endif