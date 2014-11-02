#ifndef ACDGW_HTTP_SERVER_H
#define ACDGW_HTTP_SERVER_H


#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include  <Winsock2.h>
#include "AGUtil.h"



typedef std::map<std::string, struct evhttp_request *> HTTPCLIENTMAP;
class AGHttpServer:public IceUtil::Thread
{
public:
	AGHttpServer(std::string httpAddr, int httpPort):addr(httpAddr),port(httpPort),isDone(false),http_server(NULL)
	{}
	~AGHttpServer();
	void         Stop();
	void         SendResponse(std::string clientIdent, std::string result);
	std::string  ConstructFailDesc(std::string mediaId, std::string failDesc);
	std::string  ConstructDisAgtSuc(std::string mediaId, std::string agentId);
	std::string  ConstructCanDASuc(std::string mediaId);
	void         SendResponseWithReq(struct evhttp_request *req, std::string response);

	void         AddHttpClientToHCM(std::string clientId, struct evhttp_request * req);

private:
	void         run();
	int          InitWinSocket();
	friend  void HttpReqCallback(struct evhttp_request *req, void *arg);
	
	
private:
	std::string     addr;
	int             port;
	bool            isDone;
	struct evhttp * http_server;
	HTTPCLIENTMAP   clientMap;
	IceUtil::Monitor<IceUtil::RecMutex>     cmMutex;
};
typedef IceUtil::Handle<AGHttpServer>       AGHttpServerPtr;
extern  AGHttpServerPtr                     gAGHttpServer;

#endif