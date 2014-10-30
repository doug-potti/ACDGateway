#ifndef ACDGW_HTTP_SERVER_H
#define ACDGW_HTTP_SERVER_H

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include  <Winsock2.h>
#include "AGUtil.h"

class AGHttpServer:public IceUtil::Thread
{
public:
	AGHttpServer(std::string httpAddr, int httpPort):addr(httpAddr),port(httpPort),isDone(false),http_server(NULL)
	{}
	~AGHttpServer();
	void Stop();
	void SendResponse(std::string clientIdent, std::string result);
private:
	void         run();
	int          InitWinSocket();
	friend  void HttpReqCallback(struct evhttp_request *req, void *arg);
private:
	std::string     addr;
	int             port;
	bool            isDone;
	struct evhttp * http_server;
};
typedef IceUtil::Handle<AGHttpServer>       AGHttpServerPtr;
extern  AGHttpServerPtr                     gAGHttpServer;

#endif