/**
 *
 *  HttpServerContext.h
 *  An Tao
 *  
 *  Copyright 2018, An Tao.  All rights reserved.
 *  https://github.com/an-tao/drogon
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Drogon
 *
 */

#pragma once

#include "HttpRequestImpl.h"
#include <trantor/utils/MsgBuffer.h>
#include <drogon/WebSocketConnection.h>
#include <drogon/HttpResponse.h>
#include <list>
#include <mutex>
#include <trantor/net/TcpConnection.h>

using namespace trantor;
namespace drogon
{
class HttpServerContext
{
  public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpServerContext(const trantor::TcpConnectionPtr &connPtr);

    // return false if any error
    bool parseRequest(MsgBuffer *buf);

    bool gotAll() const
    {
        return state_ == kGotAll;
    }

    void reset()
    {
        state_ = kExpectRequestLine;
        request_.reset(new HttpRequestImpl);
    }

    const HttpRequestPtr request() const
    {
        return request_;
    }

    HttpRequestPtr request()
    {
        return request_;
    }

    HttpRequestImplPtr requestImpl()
    {
        return request_;
    }

    bool firstReq()
    {
        if (_firstRequest)
        {
            _firstRequest = false;
            return true;
        }
        return false;
    }
    WebSocketConnectionPtr webSocketConn()
    {
        return _websockConnPtr;
    }
    void setWebsockConnection(const WebSocketConnectionPtr &conn)
    {
        _websockConnPtr = conn;
    }
    //to support request pipelining(rfc2616-8.1.2.2)
    std::mutex &getPipeLineMutex();
    void pushRquestToPipeLine(const HttpRequestPtr &req);
    HttpRequestPtr getFirstRequest() const;
    HttpResponsePtr getFirstResponse() const;
    void popFirstRequest();
    void pushResponseToPipeLine(const HttpRequestPtr &req, const HttpResponsePtr &resp);

  private:
    bool processRequestLine(const char *begin, const char *end);

    HttpRequestParseState state_;
    HttpRequestImplPtr request_;

    bool _firstRequest = true;
    WebSocketConnectionPtr _websockConnPtr;

    std::list<std::pair<HttpRequestPtr, HttpResponsePtr>> _requestPipeLine;
    std::shared_ptr<std::mutex> _pipeLineMutex;

    std::weak_ptr<trantor::TcpConnection> _conn;
};

} // namespace drogon