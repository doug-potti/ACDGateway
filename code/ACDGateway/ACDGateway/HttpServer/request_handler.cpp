//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "connection.hpp"
#include "..\AGUtil.h"
#include "..\AGService.h"
#include "..\AGExcuteMakeCallThd.h"

namespace http 
{
	namespace server 
	{


		request_handler::request_handler()
		{}

		void request_handler::AddHttpClientToCM(std::string taskId, HttpClient_t client)
		{
			boost::mutex::scoped_lock lock(cmMutex);  
			HTTPCLIENTMAP::iterator findIter = clientMap.find(taskId);
			if (findIter != clientMap.end())
			{
				clientMap.erase(findIter);
			}
			clientMap.insert(std::pair<std::string, HttpClient_t>(taskId, client));	
		}

		void request_handler::handle_request(const request& req, reply& rep, connection *conPtr)
		{
			std::string request_path;
			if (!url_decode(req.uri, request_path))
			{
				rep = reply::stock_reply(reply::bad_request);
				conPtr->SendResponse();
				return;
			}

			if (request_path.empty() || 
				request_path[0] != '/'|| 
				request_path.find("..") != std::string::npos)
			{
				rep = reply::stock_reply(reply::bad_request);
				conPtr->SendResponse();
				return;
			}

			size_t pos;
			if (( pos = request_path.find("/DistributeMultimediaCall.html") ) != std::string::npos )
			{
				std::string mediaId = ParseHttpMessage("Id=","&",request_path);
				std::string meidaType = ParseHttpMessage("Mediatype=","&",request_path);;
				std::string bussType = ParseHttpMessage("Businesstype=","&",request_path);
				std::string custlevel = ParseHttpMessage("Customlevel=","&",request_path);
				if (mediaId.empty())
				{
					rep.status = reply::ok;
					rep.content.append("MediaIdIsNull");
					conPtr->SendResponse();
					return;
				}

				if (meidaType.empty())
				{
					rep.status = reply::ok;
					rep.content.append("MediaTypeIsNull");
					conPtr->SendResponse();
					return;
				}

				if (bussType.empty())
				{
					rep.status = reply::ok;
					rep.content.append("BussinessTypeIsNull");
					conPtr->SendResponse();
					return;
				}

				if (custlevel.empty())
				{
					rep.status = reply::ok;
					rep.content.append("CustomerLevelIsNull");
					conPtr->SendResponse();
					return;
				}

				if (AGHelper::IsExistTask(mediaId, Distribute))
				{
					rep.status = reply::ok;
					rep.content.append(ConstructFailDesc(mediaId, "TaskIsExist"));
					conPtr->SendResponse();
					return;
				}

				std::string dialNo = "";
				if (gAGService != NULL)
				{
					dialNo = gAGService->GetDialNo(meidaType, bussType, custlevel);
				}
				if (dialNo.size() == 0)
				{
					rep.status = reply::ok;
					rep.content.append(ConstructFailDesc(mediaId, "NotFindSkill"));
					conPtr->SendResponse();
					return;
				}
				HttpClient_t hc(rep);
				hc.pConnection = conPtr;
				AddHttpClientToCM(mediaId,hc);

				DistributeAgentTask *disTask = new DistributeAgentTask(mediaId,
																		meidaType,
																		bussType,
																		custlevel,
																		dialNo);
				if (disTask)
				{
					AGHelper::AddTaskToTL(disTask);
					if (gAGExcuteMakeCall != NULL)
					{
						gAGExcuteMakeCall->AddAGMakeCallTaskToATQ(disTask);
					}
				}

			}
			else if(( pos = request_path.find("/CancelDistributeMultimediaCall.html") ) != std::string::npos )
			{
				std::string cancelMedId = ParseHttpMessage("Id=","&",request_path);
				if (cancelMedId.empty())
				{
					rep.status = reply::ok;
					rep.content.append("MediaIdIsNull");
					conPtr->SendResponse();
					return;
				}

				if (!AGHelper::IsExistTask(cancelMedId, Distribute))
				{
					rep.status = reply::ok;
					rep.content.append(ConstructFailDesc(cancelMedId, "NoDistributeTask"));
					conPtr->SendResponse();
					return;//or return mir
				}

				if (AGHelper::IsExistTask(cancelMedId, CancelDistribute))
				{
					rep.status = reply::ok;
					rep.content.append(ConstructFailDesc(cancelMedId, "TaskIsExist"));
					conPtr->SendResponse();
					return;
				}

				HttpClient_t hc(rep);
				hc.pConnection = conPtr;
				AddHttpClientToCM(cancelMedId,hc);

				CancelDisAgtTask *cdaTask = new CancelDisAgtTask(cancelMedId);

				if (cdaTask)
				{
					AGHelper::AddTaskToTL(cdaTask);
					if (gAGService != NULL)
					{
						gAGService->AddTaskToTL(cdaTask);
					}
				}
			}
			else if(( pos = request_path.find("/Transfer.html") ) != std::string::npos )
			{
				std::string transMedId = ParseHttpMessage("Id=","&",request_path);
				std::string srcId = ParseHttpMessage("Srcagentid=","&",request_path);
				std::string destId = ParseHttpMessage("Destinationagentid=","&",request_path);
				if (transMedId.empty())
				{
					rep.status = reply::ok;
					rep.content.append("MediaIdIsNull");
					conPtr->SendResponse();
					return;
				}
				if (srcId.empty())
				{
					rep.status = reply::ok;
					rep.content.append("SourceIdIsNull");
					conPtr->SendResponse();
					return;
				}
				if (destId.empty())
				{
					rep.status = reply::ok;
					rep.content.append("DestinationagentidIsNull");
					conPtr->SendResponse();
					return;
				}

				if (AGHelper::IsExistTask(transMedId, Transer))
				{
					rep.status = reply::ok;
					rep.content.append(ConstructFailDesc(transMedId, "TaskIsExist"));
					conPtr->SendResponse();
					return;
				}

				HttpClient_t hc(rep);
				hc.pConnection = conPtr;
				AddHttpClientToCM(transMedId,hc);

				TransferTask *ttask = new TransferTask(transMedId,
													   srcId,
													   destId);

				if ( AGHelper::IsExistTaskByTaskIdAgtId(transMedId, srcId))
				{
					ttask->SetTransferTaskType(Consult);
					if (ttask)
					{
						AGHelper::AddTaskToTL(ttask);
						if (gAGService != NULL)
						{
							gAGService->AddTaskToTL(ttask);
						}
					}
				}
				else
				{
					ttask->SetTransferTaskType(Make);
					if (ttask)
					{
						AGHelper::AddTaskToTL(ttask);
						if (gAGExcuteMakeCall != NULL)
						{
							gAGExcuteMakeCall->AddAGMakeCallTaskToATQ(ttask);
						}
					}
				}




			}
			else if(( pos = request_path.find("/CancelTransfer.html") ) != std::string::npos )
			{
				std::string canTransId = ParseHttpMessage("Id=","&",request_path);
				if (canTransId.empty())
				{
					rep.status = reply::ok;
					rep.content.append("MediaIdIsNull");
					conPtr->SendResponse();
					return;
				}

				if (AGHelper::IsExistTask(canTransId, CancelTransfer))
				{
					rep.status = reply::ok;
					rep.content.append(ConstructFailDesc(canTransId, "TaskIsExist"));
					conPtr->SendResponse();
					return;
				}

				if (!AGHelper::IsExistTask(canTransId, Transer))
				{
					rep.status = reply::ok;
					rep.content.append(ConstructFailDesc(canTransId, "NoTransferTask"));
					conPtr->SendResponse();
					return;
				}

				HttpClient_t hc(rep);
				hc.pConnection = conPtr;
				AddHttpClientToCM(canTransId,hc);

				CancelTransferTask *cttask = new CancelTransferTask(canTransId);
				if (cttask)
				{
					AGHelper::AddTaskToTL(cttask);
					if (gAGService != NULL)
					{
						gAGService->AddTaskToTL(cttask);
					}
				}
			}
			else
			{
				rep = reply::stock_reply(reply::not_found);
				conPtr->SendResponse();
			}
		}

		std::string request_handler::ParseHttpMessage(std::string id, std::string gap,std::string &result)
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

		void request_handler::SendHttpResponse(std::string mediaId, std::string rspMsg)
		{
			std::stringstream sslog("");
			sslog<<"AGHttpServer.SendHttpResponse ident:"<<
					mediaId<<" result:"<<
					rspMsg<<std::endl;
			OUTDEBUGLOG(sslog.str());
			sslog.str("");

			boost::mutex::scoped_lock lock(cmMutex);
			HTTPCLIENTMAP::iterator findIter = clientMap.find(mediaId);
			if (findIter != clientMap.end())
			{
				try
				{
					reply rpl = (*findIter).second.rep;
					rpl.status = reply::ok;
					rpl.content.append(rspMsg);
					(*findIter).second.pConnection->SendResponse(rpl);
					clientMap.erase(findIter);
				}
				catch(...)
				{
					sslog<<"AGHttpServer.SendHttpResponse occur error ident:"<<
							mediaId<<" result:"<<
							rspMsg<<std::endl;
					OUTERRORLOG(sslog.str());
					sslog.str("");
					clientMap.erase(findIter);
				}
			}
		}

		std::string request_handler::ConstructFailDesc(std::string mediaId, std::string failDesc)
		{
			std::string result("{\"Id\":\"");
			result.append(mediaId);
			result.append("\,\"result\":\"fail\",\"errorMsg\":\"");
			result.append(failDesc);
			result.append("\"}");
			return result;
		}

		bool request_handler::url_decode(const std::string& in, std::string& out)
		{
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				if (in[i] == '%')
				{
					if (i + 3 <= in.size())
					{
						int value = 0;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							out += static_cast<char>(value);
							i += 2;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (in[i] == '+')
				{
					out += ' ';
				}
				else
				{
					out += in[i];
				}
			}
			return true;
		}

	}
}
