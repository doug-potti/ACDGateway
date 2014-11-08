//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp> 


namespace http 
{
	namespace server 
	{

		struct reply;
		struct request;

		class  connection;

		struct HttpClient_t
		{
			reply & rep;
			connection *pConnection;
			HttpClient_t(reply &rp):pConnection(NULL), rep(rp)
			{}
		};

		typedef std::map<std::string, HttpClient_t> HTTPCLIENTMAP;

		class request_handler: private boost::noncopyable
		{
		public:

			explicit request_handler();
			void         handle_request(const request& req, reply& rep, connection *conPtr);
			void         SendHttpResponse(std::string mediaId, std::string rspMsg);
		private:

			HTTPCLIENTMAP     clientMap;
			boost::mutex      cmMutex;
			static bool  url_decode(const std::string& in, std::string& out);
			void         AddHttpClientToCM(std::string taskId, HttpClient_t client);
			std::string  ConstructFailDesc(std::string mediaId, std::string failDesc);
			std::string  ParseHttpMessage(std::string id, std::string gap,std::string &result);
		};

	} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
